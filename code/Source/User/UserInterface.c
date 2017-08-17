#include "includes.h"

uint8_t terminal_status;

/*
******************************************************************************************
* 命令格式：1字节+1字节+n字节数据部分 =》数据类型+数据长度+数据
******************************************************************************************
*/

uint16_t cmdId=0;

//发送缓存,固定命令长度+可变命令长度+保险
uint8_t target[14+80+10];

zigbee_struct* zgb=NULL;

void UserInterface(zigbee_struct* zigbee,void* persist){
	zgb=zigbee;
}

/*
*********************************************************************************************************
* Description: 向接收缓存中放入自定义字节，可被getNextCmd()等获取
*
* Arguments  : type 数据类型
*              data 要放入的的字节数组，可以是NULL值
*              数据部分长度，也就是data的长度（存入缓存的数据结构，数据类型+数据部分长度+数据部分）
*
* Returns    : 0 表示放入失败
*              1 表示放入成功
*********************************************************************************************************
*/
int postData(uint8_t type,uint8_t* data,uint8_t dataLen){
	return excuteCmd(type,data,dataLen,NULL);
}

/*
*********************************************************************************************************
* Description: 获取下一个命令，如果没有会堵塞
*
* Arguments  : err          is a pointer to where an error message will be deposited.  Possible error
*                            messages are:
*
*                            OS_ERR_NONE         The call was successful and your task received a
*                                                message.
*                            OS_ERR_TIMEOUT      A message was not received within the specified 'timeout'.
*                            OS_ERR_PEND_ABORT   The wait on the mailbox was aborted.
*                            OS_ERR_EVENT_TYPE   Invalid event type
*                            OS_ERR_PEND_ISR     If you called this function from an ISR and the result
*                                                would lead to a suspension.
*                            OS_ERR_PEVENT_NULL  If 'pevent' is a NULL pointer
*                            OS_ERR_PEND_LOCKED  If you called this function when the scheduler is locked
*                            BSP_ERR_IWDG        下一条数据非命令格式，为喂狗类型命令
*                            BSP_ERR_ARRAY       下一条数据非命令格式，为数组类型命令
*                            BSP_ERR_EXIT        下一条数据非命令格式，为外部中断类型命令
*                            BSP_ERR_ALIVE       下一条数据非命令格式，为心跳包类型命令
*                            BSP_ERR_REGISTER    下一条数据非命令格式，为注册类型命令
*                            BSP_ERR_UNDEFINE    下一条数据非命令格式，没有被定义的类型
*							 timeout      设置超时时间，单位节拍数
*
* Returns    :  命令结构体
*********************************************************************************************************
*/
cmd_line getNextCmd(uint8_t* err,uint8_t** data,INT32U timeout){
	uint8_t i=0,len,t;
	cmd_line cmd;
	(*data)=pullElement(err,NULL);
	if((*data)==NULL) return cmd;
	t=(*data)[0];
	if((t&TYPE_CMD)==0){
		if(t==TYPE_IWDG){
			*err=BSP_ERR_IWDG;
			return cmd;
		}else if(t==TYPE_ARRAY){
			*err=BSP_ERR_ARRAY;
			return cmd;
		}else if(t==TYPE_EXIT){
			*err=BSP_ERR_EXIT;
			return cmd;
		}else if(t==TYPE_ALIVE){
			*err=BSP_ERR_ALIVE;
			return cmd;
		}else if(t==TYPE_REGISTER){
			*err=BSP_ERR_REGISTER;
			return cmd;
		}else{
			*err=BSP_ERR_UNDEFINE;
			return cmd;
		}
	}
	
	if((*data)[1]<14){
		*err=BSP_ERR_UNDEFINE;
		return cmd;
	}
	
#ifdef CRYPTEN
	//解密
	if( (t&TYPE_CMD_NOCHECK)==0 && terminal_status!=REGISTER_NET_STATUS && terminal_status!=REGISTER_KEY_STATUS ){						//注册网络和请求秘钥不加密
		len=(*data)[1];
		if(len>16) len=16;
		bsp_decrypt((*data)+3,len);
	}
#endif
	
	//必要的数据检验，查看命令是否完整，(*data)[1]=收到的命令字节数，(*data)[10]=命令给出的整条命令字节数
	if( (*data)[1]<(*data)[10] || (*data)[10]<14 ){
		*err=BSP_ERR_UNDEFINE;
		return cmd;
	}
	
#ifdef CRCEN
	//冗余校验
	if((t&TYPE_CMD_NOCHECK)==0 && CRC32_Check( (*data)+3 , (*data)[10] ) !=0){																						//根据读取命令格式，(*data)+3表示数据部分首地址，根据自定义命令格式(*data)[10]表示数据部分长度，该长度为发送方给出，可能与(*data)[1]不一致
		*err=BSP_ERR_CRC;
		return cmd;
	}
#endif
	
	i=3;
	cmd.cmdID=(*data)[i++]<<8;
	cmd.cmdID+=(*data)[i++];
	cmd.from=(*data)[i++]<<8;
	cmd.from+=(*data)[i++];
	cmd.to=(*data)[i++]<<8;
	cmd.to+=(*data)[i++];
	cmd.type=(*data)[i++];
	cmd.length=(*data)[i++];
	cmd.subtyp=(*data)[i++];
	cmd.route=(*data)[i++];
	len=cmd.length-14;
	for(t=0;t<len;t++){
		cmd.value[t]=(*data)[i++];
	}
	
	return cmd;
}

/*
*********************************************************************************************************
* Description: 使用BSP_USART(USART2)串口发送命令，使用标志位判断发送是否成功
*
* Arguments  : cmd 要发送的字节
*              err CMD_ERR命令错误  CMD_NO_ERR命令正确发送成功
*
* Returns    : 
*
* Tip        : 由于使用的getCmdStream由线程安全变为不安全，该方法也非线程安全，
*              多任务同时使用该方法可能造成发送数据不一致现象
*********************************************************************************************************
*/
uint8_t sendCmd(uint8_t headType,cmd_line cmd,void *persist){
	uint8_t *in;  																				//命令字节流  数组
	uint8_t len;

	int size=cmd.length;
	
	if(headType<0x80) headType=CMD_HEAD_TYPE1;
	
	if(size==0) {
		return CMD_ERR;
	}
	
	in=getCmdStream(cmd,&len);  	
	
	if(addToSendBuff(in,len,&headType)==0){
		resetSendBuff(NULL);
		if(addToSendBuff(in,len,&headType)==0) return CMD_ERR;
	}
	 
	return CMD_NO_ERR;

}

/*
*********************************************************************************************************
* Description: 根据命令结构体获取命令的字节数组，包括加密，生成crc冗余码
*
* Arguments  : source 命令的结构体
*
* Returns    : 命令的字节数组，不包括头尾字节
*
* Tip        : 由于使用了全局发送缓存，不支持多任务同时调用
*********************************************************************************************************
*/
uint8_t* getCmdStream(cmd_line source,uint8_t* len){
	uint8_t size=source.length-14;												//实际命令字条数，即可变部分数组大小
	uint8_t i=0,j=0;
	//uint8_t* target=malloc((len+4)*sizeof(uint8_t));//分配len+4个字节,+4表示命令的头和尾默认加入
	//head
	//*(target+i++)=0xfe;
	//*(target+i++)=0xff;
	
	//定长部分
	*(target+i++)=source.cmdID>>8;
	*(target+i++)=source.cmdID;
	*(target+i++)=source.from>>8;
	*(target+i++)=source.from;
	*(target+i++)=source.to>>8;
	*(target+i++)=source.to;
	*(target+i++)=source.type;
	*(target+i++)=source.length;
	*(target+i++)=source.subtyp;
	*(target+i++)=source.route;
	
	//可变部分
	for(;j<size;j++){
		*(target+i++)=source.value[j];
	}
	
	/*
	 *先校验再加密——由于网关需要，该方法暂做保留，采用先解密再校验
	if(ENCRYPT_EN) i=bsp_encrypt(target,i);
	
	i+=4;
	
	if(CRC_EN) CRC32_Fill(target,i);
	*/
	i+=4;  																									//加上冗余码长度
	
#ifdef CRCEN
	CRC32_Fill(target,i);
#endif
	
#ifdef CRYPTEN
	if(terminal_status!=REGISTER_NET_STATUS&&terminal_status!=REGISTER_KEY_STATUS) bsp_encrypt(target,i,2);
#endif
	
	*len=i;
	return target;
}

/*
*********************************************************************************************************
* Description: 获取默认命令，id自增，自动填充from（发送方），to（接收方），subtype（设备类型），其余全部置0
*
* Arguments  : 
*
* Returns    : 命令的结构体
*********************************************************************************************************
*/
cmd_line getDefaultCmd(){
	cmd_line c;
	c.cmdID=++cmdId;
#ifdef USINNETID
	c.from=getNetId();
#else
	c.from=(zgb->MyAddr[0])<<8;
	c.from+=zgb->MyAddr[1];
#endif
	c.to=(zgb->DstAddr[0])<<8;
	c.to+=zgb->DstAddr[1];
	c.type=0;
	c.length=0x0e;
#ifdef TERMINAL_TYPE
	c.subtyp=TERMINAL_TYPE;
#else
	c.subtyp=0;
#endif
	c.route=0;
	return c;
}

cmd_line getCmd(cmd_line copy){
	cmd_line c;
	c.cmdID=copy.cmdID;
	c.from=getNetId();
	c.to=copy.from;
	c.type=copy.type;
	c.length=0x0e;
#ifdef TERMINAL_TYPE
	c.subtyp=TERMINAL_TYPE;
#else
	c.subtyp=copy.subtyp;
#endif
	c.route=copy.route;
	return c;
}

/*
*********************************************************
* flash
*/  

/*
* len表示字节长度
*/
uint8_t flashCopyToOtherPage(uint32_t fromAddr,uint32_t toAddr,uint16_t len,void* persist){
	uint32_t data;
	int i;
	
	//地址必须是4的倍数
	if((fromAddr&0x03)!=0||(toAddr&0x03)!=0) return 0;
	
	//长度不能超过一片，即不能复制到下一片的flash中,复制来源可以是下一片
	if(len>(0x00000400-(toAddr&0x000003ff))) return 0;
	
	//开始复制
	for(i=0;i<len;i+=4){
		read_itn_flash_data(&data,1,fromAddr+i);
		if(write_itn_flash_data(&data,1,toAddr+i)==0) return 0;
	}
	
	return 1;
}

uint8_t writekeyStatus(uint8_t status[6][12],void* persist){
	int i,j;
	uint32_t data;
	
	if(erasePage(FLASHADDRESSOFKEYSTATUS)==0) return 0;
	
	//写入标志位，表示已有数据
	data=1;
	if(write_itn_flash_data(&data,1,FLASHADDRESSOFKEYSTATUS)==0) return 0;
	
	//写入按键状态
	for(i=0;i<6;i++){
		for(j=0;j<12;j++){
			if((j&0x03)==0) data=0;
			data=data<<8;
			data+=status[i][j];
			if((j&0x03)==3){
				if(write_itn_flash_data(&data,1,FLASHADDRESSOFKEYSTATUS+4+4*((i*3)+(j>>2)))==0) return 0;
			}
		}
	}
	
	return 1;
}

uint8_t readKeyStatus(uint8_t status[6][12],void* persist){
	int i,j;
	uint32_t data;
	
	//读取标志位，如果已有数据则填充
	read_itn_flash_data(&data,1,FLASHADDRESSOFKEYSTATUS);
	if(data!=1) return 1;
	
	//写入按键状态
	for(i=0;i<6;i++){
		for(j=0;j<3;j++){
			read_itn_flash_data(&data,1,FLASHADDRESSOFKEYSTATUS+4+4*((i*3)+j));
			status[i][j*4+0]=data>>24;
			status[i][j*4+1]=data>>16;
			status[i][j*4+2]=data>>8;
			status[i][j*4+3]=data;
		}
	}
	
	return 1;
}

uint8_t checkWritable(uint32_t addr,void* persist){
	uint32_t i,len;
	uint32_t data;
	
	//地址必须是4的倍数
	if((addr&0x03)!=0||(addr&0x03)!=0) return 0;
	
	len=FLASHPAGESIZE-(addr&0x000003ff);
	for(i=0;i<len;i+=4){
		read_itn_flash_data(&data,1,addr+i);
		if(data!=0xffffffff) return 0;
	}
	
	return 1;
}

uint8_t findNextAvaliablePage(uint32_t* addr,void* persist){
	
	*addr=FLASHADDRESSOFSCENE;
	
	//寻找可用的片flash，一片=1kB
	for(;*addr<FLASHFORCOPING;*addr+=FLASHPAGESIZE){
		if(checkWritable(*addr,NULL)) break;
	}
	
	//循环查找结束仍未找到可用的片
	if(*addr>=FLASHFORCOPING){
		return 0;
	}
	
	return 1;
}

uint8_t writeSceneHead(uint8_t *sceneId,uint32_t* addr,void* persist){
	uint32_t data;
	
	//寻找下一个可用的片flash，并把首地址存入addr中，如果失败返回0
	if(findNextAvaliablePage(addr,NULL)==0) return 0;
	
	//情景Id+保留字段（0）—— 4个字节
	data=sceneId[0]<<24;
	data+=sceneId[1]<<16;
	
	//写入情景id+保留字段（0）
	if(write_itn_flash_data(&data,1,*addr)==0) return 0;
	
	return 1;
	
}

uint8_t readSceneHead(uint8_t *sceneId,uint32_t addr,void* persist){
	uint32_t data;
	
	read_itn_flash_data(&data,1,addr);
	
	sceneId[0]=data>>24;
	sceneId[1]=(data>>16)&0x000000ff;
	
	return 1;
}

uint8_t addSceneItem(uint8_t *item,uint32_t addrOfHead,void* persist){
	uint8_t  sceneId[2];
	uint8_t sceneItem[12];
	uint32_t i;
	
	if(persist!=NULL){
		
		readSceneHead(sceneId,addrOfHead,NULL);
		
		//校验场景id值，如果不同返回失败（0）
		if(sceneId[0]!=((uint8_t*)persist)[0]||sceneId[1]!=((uint8_t*)persist)[1]) return 0;
		
	}
	
	//将地址移到场景记录的首地址
	addrOfHead+=4;
	
	//将地址移到最后一条场景记录之后的位置
	while((i=readSceneItem(sceneItem,addrOfHead,NULL))!=0){
		addrOfHead=i;
	}
	
	if((addrOfHead&0x000003ff)>=0x000003f4){
		if((addrOfHead+12)>=FLASHFORCOPING) return 0;
		addrOfHead=((addrOfHead+12)&0xfffffc00);
		i=addrOfHead-4;
		if(findNextAvaliablePage(&addrOfHead,NULL)==0) return 0;
		if(write_itn_flash_data(&addrOfHead,1,i)==0) return 0;
	}
	
	//写入一条场景记录
	if(write_itn_flash_data((uint32_t*)item,3,addrOfHead)==0) return 0;
	
	return 1;
}

uint8_t deleteSceneItem(uint8_t *item,uint32_t addrOfHead,void* persist){
	uint8_t tmp[12];
	uint32_t i=0,j,data;
	uint8_t  sceneId[2];
	
	if(persist!=NULL){
		
		readSceneHead(sceneId,addrOfHead,NULL);
		
		//校验场景id值，如果不同返回失败（0）
		if(sceneId[0]!=((uint8_t*)persist)[0]||sceneId[1]!=((uint8_t*)persist)[1]) return 0;
		
	}
	
	//将地址移到场景记录的首地址
	addrOfHead+=4;
	
	while((i=readSceneItem(tmp,addrOfHead,NULL))!=0){
		
		for(j=0;j<12;j++){
			if(tmp[j]!=item[j]) break;
		}
		
		if(j==12){
			break;
		}
		
		addrOfHead=i;
	}
	
	if(i==0) return 0;
	
	//复制该片的其他已写入的数据到保留区
	//addrOfHead:需要被删除记录的首地址
	
	//该片的首地址
	i=(addrOfHead&0xfffffc00);
	
	erasePage(FLASHFORCOPING);
	//复制该片首地址开始到要删除位置结束到保留区
	if(flashCopyToOtherPage(i,FLASHFORCOPING,addrOfHead-i,NULL)==0) return 0;
	//j:复制到保留区数据的地址
	j=FLASHFORCOPING+(addrOfHead-i);
	//addrOfHead:被复制数据的地址
	addrOfHead=readSceneItem(tmp,addrOfHead,NULL);
	
	//i:表示复制回去的起点
	
	//复制要被删除数据之后的内容——可能会超过一片的长度
	do{
		//读取一条记录
		if((addrOfHead=readSceneItem(tmp,addrOfHead,NULL))==0){
			erasePage(i);
			//将保留区内的数据复制回去
			if(flashCopyToOtherPage(FLASHFORCOPING,i,FLASHPAGESIZE,NULL)==0) return 0;
			break;
		}
		//复制该记录到缓存区
		if(write_itn_flash_data((uint32_t*)tmp,3,j)==0) return 0;
		j+=12;
		if(j>=(FLASHTOPADDRESS-12)){
			//如果不是最后一条数据就复制地址
			if(readSceneItem(tmp,addrOfHead,NULL)!=0){
				//将地址复制到保留区末尾
				read_itn_flash_data(&data,1,i+FLASHPAGESIZE-4);
				write_itn_flash_data(&data,1,(j&0xfffffc00)+FLASHPAGESIZE-4);
			}
			
			erasePage(i);
			//将保留区内的数据复制回去
			if(flashCopyToOtherPage(FLASHFORCOPING,i,FLASHPAGESIZE,NULL)==0) return 0;
			i=(addrOfHead&0xfffffc00);
			j=FLASHFORCOPING;
			erasePage(FLASHFORCOPING);
		}
	}while(1);
	
	return 1;
}

uint8_t alertSceneItem(uint8_t *item,uint32_t addr,void* persist){
	//暂不实现，临时使用先删除再增加的方式
	return 0;
}

uint32_t readSceneItem(uint8_t *item,uint32_t addr,void* persist){
	
		
	//如果是该片的最后则读取该片最后一个双字数据作为下一片数据地址
	if((addr&0x000003ff)>=0x000003f4){
		
		//计算出最后一个双字地址
		addr=((addr|0x000003ff)-3);
		
		read_itn_flash_data(&addr,1,addr);
		
		//判断地址的合理性,低10位为0，在场景flash范围内——该判定除防止一些运行时错误也是对一个片正好写满的判断，该片正好写满是不会在最后一个字节添加地址而保持0xffffffff
		if((addr&0x000003ff)!=0||addr<FLASHADDRESSOFSCENE||addr>=FLASHFORCOPING) return 0;
		
	}
	
	//读取该记录
	read_itn_flash_data((uint32_t*)item,3,addr);
	
	if(item[11]!=0) return 0;
	
	return addr+12;
}

uint8_t  deleteScene(uint8_t *sceneId,uint32_t addrOfHead,void* persist){
	uint8_t  sceneIdCheck[2];
	uint8_t  item[12],statu;
	uint32_t cmp;
	
	readSceneHead(sceneIdCheck,addrOfHead,NULL);
		
	//校验场景id值，如果不同返回失败（0）
	if(sceneId[0]!=sceneIdCheck[0]||sceneId[1]!=sceneIdCheck[1]) return 0;
	
	addrOfHead+=4;
	
	//该片最大范围
	cmp=(addrOfHead|0x000003ff)-11;
	
	//1:需要擦除该片
	statu=0;
	
	while((addrOfHead=readSceneItem(item,addrOfHead,NULL))!=0){
		if(statu==1){
			if(erasePage(cmp&0xfffffc00)==0) return 0;
			cmp=(addrOfHead|0x000003ff)-11;
			statu=0;
		}
		if(addrOfHead>=cmp){
			statu=1;
		}
	}
	if(erasePage(cmp&0xfffffc00)==0) return 0;
	
	return 1;
}

