#include "includes.h"

uint8_t hasDataInSendBuff=0;

/*
******************************************************************************************
* 读取命令格式：1字节+1字节+n字节数据部分 =》数据类型+数据长度+数据
******************************************************************************************
*/

OS_EVENT	*SEND_SYN_SEM;     //发送信号量，初值应为1
OS_EVENT	*SEND_POINT_SYN_SEM;     //发送缓存中指针的信号量，初值应为1

/*
*********************************************************
* 输入模块  
*/

USART_TypeDef* MyIOUsart;																						//用于接收和发送的串口

int readStatu=READ_ARRAY;																						//当前读取状态

int READ_ARRAY_LENGTH=READ_LOCAL_ZIGBEE_CONFIG_MESSAGE_SIZE;        //如果是类似于READ_ARRAY这种给定长度的读取，表示每个包的长度

uint8_t* dataArray;																									//

uint8_t  cmd_buff[2]={0};																							//读取命令的缓存，由于命令根据头尾判断包，无法事先申请长度（可以事先申请足够大的长度）

extern uint8_t  receive_buff[RECEIVEBUFFSIZE+2];

int dataIndex=-1;                               										

int cmdStatu=-1;  																									//-1表示未开始读取

//不使用堆栈原因：正常情况下栈底元素都是首先被移出，可能会产生大量复制
extern uint8_t  send_buff[SENDCMDBUFFSIZE+128];																//128表示单次申请最大字节数，避免越界，cmd长度最大94，base64转码也不超过128

int headOfSendPoint=0,tailOfSendPoint=0,sendPoint=0;

uint8_t receiveErrorTimes=0;

void MyIO(void* persist){
	
	SEND_SYN_SEM=OSSemCreate(1);
	
	SEND_POINT_SYN_SEM=OSSemCreate(1);
	
	USART_ITConfig(MyIOUsart, USART_IT_TXE, DISABLE);
	
	changeReadStatu(READ_CMD,0);
}

void IOInit(USART_TypeDef* usart,void (*initFuc)(void* persist),void (*disableFuc)(void* persist),void* persist){
	if(disableFuc!=NULL) disableFuc(NULL);
	
	if(initFuc!=NULL) initFuc(NULL);
	
	MyIOUsart=usart;
}

/*
*********************************************************************************************************
* Description: 改变读取状态
*
* Arguments  : statu 需要改变到的状态  对外提供READ_ARRAY和READ_CMD
*							 len 如果是READ_ARRAY，还需要给出每个包的大小
*
* Returns    : 
*********************************************************************************************************
*/

void changeReadStatu(int statu,int len){
#if OS_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
    OS_CPU_SR  cpu_sr = 0u;
#endif
	OS_ENTER_CRITICAL();
	if(statu!=READ_ARRAY&&statu!=READ_CMD&&statu!=CHANGING){								  //改变的状态正确性判断
		OS_EXIT_CRITICAL();
		return;																																	//错误
	}
	if(readStatu==statu){                                                     //状态相同，无需处理
		OS_EXIT_CRITICAL();
		return;																																	//正确
	}
	readStatu=CHANGING;                 																			//读取方式改变中，不再处理接收到的数据
	
	removeAllElement(NULL);       																												//清空原状态下所有数据缓存
	
	READ_ARRAY_LENGTH=len;
	
	//全部标志回复原状
	cmdStatu=-1;
	dataIndex=-1;
	
	readStatu=statu;																													 //改变状态
	
	OS_EXIT_CRITICAL();
}

/*
*********************************************************************************************************
* Description: 接收一个字节的数据
*
* Arguments  : 
*
* Returns    : 
*********************************************************************************************************
*/
void receiceByte(){
	if (USART_GetFlagStatus(MyIOUsart, USART_FLAG_ORE) != RESET){
		USART_ReceiveData(MyIOUsart);
		USART_ClearFlag(MyIOUsart, USART_FLAG_ORE);
	}
	if(USART_GetITStatus(MyIOUsart, USART_IT_RXNE) != RESET){
		uint8_t data=USART_ReceiveData(MyIOUsart);
		switch(readStatu){
			case READ_ARRAY:
				readArray(data);
				break;
			case READ_CMD:
				readCmd(data);
				break;
			default:
				break;
		}
	}
}



/*
*********************************************************************************************************
* Description: 读取固定字节数模式下的读取函数
*
* Arguments  : data 接收到的字节
*
* Returns    : 
*********************************************************************************************************
*/
void readArray(uint8_t data){
	if(dataIndex<0||dataArray==NULL){
		if(READ_ARRAY_LENGTH<=RECEIVEBUFFSIZE) dataArray=receive_buff;			//如果接受用的缓存足够，则不加锁，缺点需要将接收缓存中的数据复制到队列中
		else dataArray=getAvailableElement(2+READ_ARRAY_LENGTH,(int*)1);					//加锁队列，使队列无法加入元素（循环等等解锁），优点无须复制，缺点不稳定
		dataIndex=0;
		dataArray[dataIndex++]=TYPE_ARRAY;
		dataArray[dataIndex++]=READ_ARRAY_LENGTH;
	}
	dataArray[dataIndex++]=data;
	if(dataIndex>=(READ_ARRAY_LENGTH+2)){
		if(dataArray==receive_buff) pushElement(dataArray,dataIndex,NULL);
		else unlockMyMalloc(NULL);																					//解锁队列
		dataIndex=-1;
	}
}


/*
*********************************************************************************************************
* Description: 读取命令模式下的读取函数
*
* Arguments  : data 接收到的字节
*
* Returns    : 
*********************************************************************************************************
*/
void readCmd(uint8_t data){
	if((data&0xc0)==0){
		if(dataIndex>=RECEIVEBUFFSIZE){
			dataIndex=-1;
		}else if(dataIndex>0) receive_buff[dataIndex++]=data;
	}else if((data&0x80)==0x80){
		receive_buff[0]=data;
		dataIndex=1;
	}else if((data&0x40)==0x40){
		dataIndex=base64Decode(receive_buff+1,dataIndex-1,NULL);
		dataIndex=-1;
		if(!(receive_buff[5]==0&&receive_buff[6]==0)){ //to是0表示广播
#ifdef USINNETID
		if(receive_buff[5]!=0||receive_buff[6]!=getNetId()) return ;
#else
		data=1;
		if(receive_buff[5]!=getZigBee(&data)->MyAddr[0]||receive_buff[6]!=getZigBee(&data)->MyAddr[1]) return;
#endif
		}
		switch(receive_buff[0]){
			case CMD_HEAD_TYPE1:
				excuteCmd(TYPE_CMD,receive_buff+1,receive_buff[8],NULL);
			break;
			case CMD_HEAD_TYPE2:
				if(addToReceiveBuff(receive_buff+1,receive_buff[8],NULL)) excuteCmd(TYPE_CMD,receive_buff+1,receive_buff[8],NULL);
			break;
			case CMD_HEAD_TYPE3:
				
			break;
			case CMD_HEAD_SURE1:
				sure1Cmd(receive_buff+1,receive_buff[8],NULL);
			break;
			case CMD_HEAD_SURE2:
				
			break;
			case CMD_HEAD_SURE3:
				
			break;
			case CMD_TEST:
				excuteCmd(TYPE_CMD|TYPE_CMD_NOCHECK,receive_buff+1,receive_buff[8],NULL);
			break;
		}
	}else{
	}
}

/***********************************************************
* 将命令存入待执行队列
* @paramers 
* 
*                
* @return 
*/
int excuteCmd(uint8_t type,uint8_t* cmd,uint8_t len,void* persist){
	uint8_t tmp[4]={3,0,0,0};
	if(cmd==NULL) len=0;
	tmp[1]=type;
	tmp[2]=len;
	if(pushElement(cmd,len,tmp)==0){
		if(++receiveErrorTimes>=RECEIVEERRORMAXTIMES){
			resetReceiveBuff(NULL);
			//重置后一般不会失败
			pushElement(cmd,len,tmp);
		}
		else return 0;
	}
	return 1;
}

/***********************************************************
* 对于82的头返回81的确认信息
* @paramers 
* 
*                
* @return 如果是0表示该命令与上一条命令相同，是接收方没有收到回复发送的重复
*/
int addToReceiveBuff(uint8_t* cmd,uint8_t len,void* persist){
	uint8_t i;
	uint8_t head[1]={CMD_HEAD_SURE1},data[8];
	//组装命令确认数据，即复制id，交换from和to
	data[0]=cmd[0];
	data[1]=cmd[1];
	data[2]=cmd[4];
	data[3]=cmd[5];
	data[4]=cmd[2];
	data[5]=cmd[3];
	data[6]=cmd[6];
	data[7]=cmd[7];
	addToSendBuff(data,8,head);
	//判断命令是否为接收方没有收到回复发送的重复命令，id不为0且id不与上次接收到的命令id相同
	if((cmd[0]!=0||cmd[1]!=0)&&cmd[0]==cmd_buff[0]&&cmd[1]==cmd_buff[1]){
		return 0;
	}
	//更新命令id
	for(i=0;i<2;i++){
		cmd_buff[i]=cmd[i];
	}
	
	return 1;
}

void sure2Cmd(uint8_t* cmd,uint8_t len,void* persist){
	
}

void resetReceiveBuff(void* persist){
	removeAllElement(NULL);
}

/*
*********************************************************
* 发送模块 
*/

/*
*********************************************************************************************************
* Description: 使用BSP_USART(USART2)串口发送一个字节的数据，使用标志位判断发送是否成功
*
* Arguments  : value 要发送的字节数组
*              len数组长度
*
* Returns    : 发送成功返回1  失败返回0
*
* Tip        : 该函数线程安全
*********************************************************************************************************
*/
uint8_t sendBytes(USART_TypeDef* usart,uint8_t* value,int len){
	int i=0;
	uint8_t err;
	//**************************************************
	//该部分代码必须作为一个整体发出,即一条条指令发出
	OSSemPend(SEND_SYN_SEM,0,&err);
	/*
	*				OS_NO_ERR   调用成功，任务拥有资源或者目标事件发生
	*       OS_TIMEOUT   规定时间内信号量没有发生——第二个参数是0，会一直等下去。所以不会出现该异常
	*       OS_ERR_EVENT_TYPE 如果没有传递指针到信号量——确认已创建，程序级
	*       OS_ERR_PEND_ISR  如果ISR调用此函数，结果将异常——可能出现，该方法可能被中断服务调用。运行级
	*       OS_ERR_PEVENT_NULL 如果 'pevent' 是一个空指针——确认非空，程序级
	*/
	
	if(err!=OS_ERR_NONE){
		return 0;
	}
	
	if(len==0){
		OSSemPost(SEND_SYN_SEM);
		return 1;
	}
	else if(len<0){
		len=-len;
		len-=3;
		if(value[len]!=CMD_TAIL){
			OSSemPost(SEND_SYN_SEM);
			return 0;
		}
		for(i=0;i<=len;i++){
			if(sendByte(usart,*(value+i))==0){
				OSSemPost(SEND_SYN_SEM);
				return 0;
			}
		}
		OSSemPost(SEND_SYN_SEM);
	}else{
		for(i=0;i<len;i++){
			if(sendByte(usart,*(value+i))==0){
				OSSemPost(SEND_SYN_SEM);
				return 0;
			}
		}
		OSSemPost(SEND_SYN_SEM);
	}
	
	return 1;
}

/*
*********************************************************************************************************
* Description: 使用BSP_USART(USART2)串口发送一个字节的数据，使用标志位判断发送是否成功
*
* Arguments  : value 要发送的字节
*
* Returns    : 发送成功返回1  失败返回0
*
* Tip        : 该函数非线程安全，不支持外部直接调用
*********************************************************************************************************
*/
uint8_t sendByte(USART_TypeDef* usart,uint8_t value){
//	uint8_t ack=0;
	int times=900;   																//最大循环次数，避免死循环,次数太小可能造成数据无法正常发送，比如：值为100时发现近乎100%失败
	USART_ClearFlag(usart, USART_FLAG_TC);
//	ack=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);
	USART_SendData(usart, value);
//	ack=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);
	//采用标志位判断发送是否完成
	while(USART_GetFlagStatus(usart,USART_FLAG_TC)==RESET&&times--);
//	ack=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12);
	if(USART_GetFlagStatus(usart,USART_FLAG_TC)==RESET) return 0;
	return 1;
}

uint8_t addToSendBuff(uint8_t* cmd,uint8_t len,void* persist){
	int tmp;
	uint8_t err;
	uint8_t sendType=CMD_HEAD_TYPE1;
	uint8_t size=len;
	
	if(len<=0) return 0;
	
	if(persist!=NULL) sendType=((uint8_t*)persist)[0];
	
	len=((len+2)/3)*4+4;																							//1字节的状态值,1字节的头和1字节的尾
	
	//进入临界区：临界区目的：headOfSendPoint全局变量的保护
	OSSemPend(SEND_POINT_SYN_SEM,0,&err);
	/*
	*				OS_NO_ERR   调用成功，任务拥有资源或者目标事件发生
	*       OS_TIMEOUT   规定时间内信号量没有发生——第二个参数是0，会一直等下去。所以不会出现该异常
	*       OS_ERR_EVENT_TYPE 如果没有传递指针到信号量——确认已创建，程序级
	*       OS_ERR_PEND_ISR  如果ISR调用此函数，结果将异常——可能出现，该方法可能被中断服务调用。运行级
	*       OS_ERR_PEVENT_NULL 如果 'pevent' 是一个空指针——确认非空，程序级
	*/
	
	
	if(err!=OS_ERR_NONE&&err!=OS_ERR_PEND_ISR){
		return 0;
	}
	
	//头和尾重合时只有两种情况，1完全没有数据，2数据满了。如果头的第一个字节为0则为情况1，否则2
	if(headOfSendPoint==tailOfSendPoint&&send_buff[headOfSendPoint]){
		if(err!=OS_ERR_PEND_ISR) OSSemPost(SEND_POINT_SYN_SEM);													
		return 0;
	}
	
	//判断当前数据长度是否会超过剩余可用数据长度
	if(headOfSendPoint<tailOfSendPoint&&(headOfSendPoint+len)>=tailOfSendPoint){					 //如果头指针在尾指针前，即可能内存不够分配，需要判断
		if(err!=OS_ERR_PEND_ISR) OSSemPost(SEND_POINT_SYN_SEM);											
		return 0;
	}																																						 
//	if(tailOfSendPoint==0&&(headOfSendPoint+len)>SENDCMDBUFFSIZE){														 //即使头指针在尾指针后，也存在一种特殊情况，选择边缘情况，避免数据被覆盖，即point_end=BUF_LEN=》point_end=0
//		OSSemPost(SEND_POINT_SYN_SEM);
//		return 0;  							 
//	}
	
	//改变标志位，表示发送缓存中有数据
	hasDataInSendBuff=1;
	
	//空间足够开始分配
	tmp=headOfSendPoint;
	headOfSendPoint+=len;
	if(headOfSendPoint>=SENDCMDBUFFSIZE) headOfSendPoint=0;
	*(send_buff+tmp)=len;
	
	//头部移动到下一个空闲区，该区首字节置0，如果该区与尾重合说明内存用完无需置0
	if(send_buff[headOfSendPoint]!=0&&headOfSendPoint!=tailOfSendPoint) send_buff[headOfSendPoint]=0;
	
	//离开临界区，临界区目的：headOfSendPoint全局变量的保护
	if(err!=OS_ERR_PEND_ISR) OSSemPost(SEND_POINT_SYN_SEM);
	
	//开始复制
	//状态，8字节，低四位表示发送次数，高两位表示确认状态（00：未确认，01：收到确认，10：3次确认结束）,5,6位目前状态状态，
	switch(sendType){
		case CMD_HEAD_TYPE1:
			*(send_buff+tmp+1)=0x00;
		break;
		case CMD_HEAD_TYPE2:
			*(send_buff+tmp+1)=0x40;
		break;
		case CMD_HEAD_TYPE3:
			*(send_buff+tmp+1)=0x80;
		break;
		default:
			*(send_buff+tmp+1)=0x00;
		break;
	}
	
	*(send_buff+tmp+2)=sendType;
	
	base64Encode(cmd,size,send_buff+tmp+3);
	
	*(send_buff+tmp+len-1)=CMD_TAIL;
	
	return 1;
	
}

uint8_t removeCmdInSendBuff(uint8_t head,void* persist){
	int i;
	uint8_t tmp=0;
	
//	//寻找是否存在该头
//	for(i=tailOfSendPoint;send_buff[i];){
//		if(i==head){
//			tmp=1;
//			break;
//		}
//		i+=send_buff[i];
//		if(i>SENDCMDBUFFSIZE) i=0;
//		if(i==tailOfSendPoint) break;
//		if(i==headOfSendPoint) break;
//	}
//	
//	//不存在，返回失败
//	if(tmp){
//		return 0;
//	}
	
	//如果该头是尾则将尾移到下个位置，并置0
	//理论上这种情况应该是最多的（在只有80头数据时，应该只会出现这种情况，出现发送82头数据时情况会比较复杂）
	if(head==tailOfSendPoint){
		tailOfSendPoint+=send_buff[tailOfSendPoint];
		send_buff[head]=0;
		if(tailOfSendPoint>=SENDCMDBUFFSIZE) tailOfSendPoint=0;
		return 1;
	}
	
	//如果该数据未最后一个，移动头，并置0
	if(head+send_buff[head]==headOfSendPoint){
		headOfSendPoint=head;
		send_buff[head]=0;
		return 1;
	}
	
	//任何处于中间位置的数据，优先考虑移动尾，如果尾部的移动需要处理越界问题，则移动头
	tmp=send_buff[head];
	
	if((head-tailOfSendPoint)>0){               								//当前要删除位置在尾部之后，说明移到数据时不会出现数组越界问题——因为有128个字节的扩张
		for(i=head-1;i>=tailOfSendPoint;i--){     								//移动尾部
			send_buff[i+tmp]=send_buff[i];
		}
		send_buff[tailOfSendPoint]=0;															//将原先的数据尾删掉
		tailOfSendPoint+=tmp;
	}else{																											//当前要删除位置在尾部之前，说明移到数据时会出现数组越界问题
		for(i=head+tmp;i<headOfSendPoint;i++){										//移动头部
			send_buff[i-tmp]=send_buff[i];
		}
		headOfSendPoint-=tmp;
		send_buff[headOfSendPoint]=0;															//将原先的数据头删掉
	}
	
	return 1;
}

uint8_t sendCmdInSendBuff(void* persist){
	int i;
	uint8_t err;
	
	//进入临界区，临界区目的：headOfSendPoint全局变量的保护
	OSSemPend(SEND_POINT_SYN_SEM,0,&err);
	/*
	*				OS_NO_ERR   调用成功，任务拥有资源或者目标事件发生
	*       OS_TIMEOUT   规定时间内信号量没有发生——第二个参数是0，会一直等下去。所以不会出现该异常
	*       OS_ERR_EVENT_TYPE 如果没有传递指针到信号量——确认已创建，
	*       OS_ERR_PEND_ISR  如果ISR调用此函数，结果将异常——可能出现，该方法可能被中断服务调用。
	*       OS_ERR_PEVENT_NULL 如果 'pevent' 是一个空指针——确认非空，
	*/
	
	if(err!=OS_ERR_NONE){
		return 0;
	}
	
	//检测是否有需要发送的数据，即检测队列是否为空
	if(headOfSendPoint==tailOfSendPoint&&!send_buff[headOfSendPoint]){
		//改变标志位，表示发送缓存中没有数据
		hasDataInSendBuff=0;
		OSSemPost(SEND_POINT_SYN_SEM);
		return 1;
	}
	
	for(i=tailOfSendPoint;send_buff[i];){
		sendBytes(MyIOUsart,send_buff+i+2,-send_buff[i]);
		send_buff[i+1]++;
		i+=send_buff[i];
		if(i>SENDCMDBUFFSIZE) i=0;
		if(i==tailOfSendPoint) break;
		if(i==headOfSendPoint) break;
		//没有延时时，缓存中有多条命令发送出去后接收方只能收到一条
		OSTimeDlyHMSM(0, 0, 0, 10);
	}
	
	//确认类型由确认函数进行删除，该方法不判断已被确认的命令
	do{
		err=0;
		for(i=tailOfSendPoint;send_buff[i];){
			//仅发送和发送次数有3次的命令，从发送队列移出
			if((send_buff[i+1]&0xf0)==0||(send_buff[i+1]&0x0f)>=RESENDTIMES){
				removeCmdInSendBuff(i,NULL);
				err=1;
				break;
			}
			i+=send_buff[i];
			if(i>SENDCMDBUFFSIZE) i=0;
			if(i==tailOfSendPoint) break;
			if(i==headOfSendPoint) break;
		}
	}while(err);
	
	//离开临界区，临界区目的：headOfSendPoint全局变量的保护
	OSSemPost(SEND_POINT_SYN_SEM);
	
	return 1;
	
}

void sure1Cmd(uint8_t* cmd,uint8_t len,void* persist){
	int i;
	uint8_t err;
	uint8_t cmdId[3];
	
	//base64编码，由于发送缓存中保存的数据是base64编码后的
	cmdId[0]=cmd[0]>>2;
	cmdId[1]=cmd[1]>>4;
	cmdId[1]+=(cmd[0]&0x03)<<4;
	cmdId[2]=(cmd[1]&0x0f)<<2;
	
	//进入临界区，临界区目的：headOfSendPoint全局变量的保护
	OSSemPend(SEND_POINT_SYN_SEM,0,&err);
	/*
	*				OS_NO_ERR   调用成功，任务拥有资源或者目标事件发生
	*       OS_TIMEOUT   规定时间内信号量没有发生——第二个参数是0，会一直等下去。所以不会出现该异常
	*       OS_ERR_EVENT_TYPE 如果没有传递指针到信号量——确认已创建，程序级
	*       OS_ERR_PEND_ISR  如果ISR调用此函数，结果将异常——可能出现，该方法可能被中断服务调用。运行级
	*       OS_ERR_PEVENT_NULL 如果 'pevent' 是一个空指针——确认非空，程序级
	*/
	
	if(err!=OS_ERR_NONE&&err!=OS_ERR_PEND_ISR){
		return;
	}
	
	//在发送缓存中查找对应id的命令
	do{
		err=0;
		for(i=tailOfSendPoint;send_buff[i];){
			if(send_buff[i+3]==cmdId[0]&&send_buff[i+4]==cmdId[1]&&(send_buff[i+5]&0xfc)==cmdId[2]){
				if((send_buff[i+1]&0xc0)==0x40){
					err=1;
					removeCmdInSendBuff(i,NULL);
					break;
				}
				else{
					send_buff[i+1]&=0xcf;
					send_buff[i+1]|=0x10;
				}
			}
			i+=send_buff[i];
			if(i>SENDCMDBUFFSIZE) i=0;
			if(i==tailOfSendPoint) break;
			if(i==headOfSendPoint) break;
		}
	}while(err);
	
	//离开临界区，临界区目的：headOfSendPoint全局变量的保护
	if(err!=OS_ERR_PEND_ISR) OSSemPost(SEND_POINT_SYN_SEM);
	
}

void sure3Cmd(uint8_t* cmd,uint8_t len,void* persist){
}

void resetSendBuff(void* persist){
	uint8_t i=0x80;
	sendByte(MyIOUsart,i);
	sendByte(MyIOUsart,headOfSendPoint);
	sendByte(MyIOUsart,tailOfSendPoint);
	sendByte(MyIOUsart,sendPoint);
	i=0x40;
	sendByte(MyIOUsart,i);
	
	headOfSendPoint=0;
	tailOfSendPoint=0;
	sendPoint=0;
}

/*
***********************************************************
* base64编解码
*/

/*
*********************************************************************************************************
* Description: base64解码，把解码之后的字节存入value中，
*							 多余部分舍弃，比如给出len为15则后三个字节舍弃不参与解码
*
* Arguments  : value 被base64编码的字节
*							 len value的长度
*							 persist 保留
*
* Returns    : 把解码之后的字节存入value中，返回value中有效长度
*
* Tip        : 
*********************************************************************************************************
*/
unsigned char  base64Decode(unsigned char *value,int len,void* persist){
	int i=0,j=0;
	unsigned int data;
	len&=0xfffffffc;//等效len=(len/4)*4;
	for(i=0,j=0;i<len;i+=4,j++){
		data=value[i]<<26;
		data+=value[i+1]<<20;
		data+=value[i+2]<<14;
		data+=value[i+3]<<8;
		value[i-j]=data>>24;
		value[i+1-j]=data>>16;
		value[i+2-j]=data>>8;
	}
	return (len>>2)*3;
}

/*
*********************************************************************************************************
* Description: base64编码
*
* Arguments  : value 需要编码的字节
*							 len value的长度
*							 persist 保留
*
* Returns    : 返回编码后的数据，长度为((len+2)/3)*4
*
* Tip        : 
*********************************************************************************************************
*/
unsigned char* base64Encode(unsigned char *value,int len,void* persist){
	int i,j;
	unsigned char rmd[3];
	unsigned int data;
	unsigned char* base64EncodeBuff;
	
	base64EncodeBuff=(uint8_t*)persist;
	
	for(i=0;i<len;i++){
		base64EncodeBuff[i]=value[i];
	}
	
	for(i=3,j=0;i<=len&&(i+j+1)<128;i+=3,j++){
		base64EncodeBuff[i-3+j]=value[i-3]>>2;
		base64EncodeBuff[i-2+j]=value[i-2]>>4;
		base64EncodeBuff[i-1+j]=value[i-1]>>6;
		base64EncodeBuff[i+j]=value[i-1]&0x3f;
		base64EncodeBuff[i-1+j]+=(value[i-2]&0x0f)<<2;
		base64EncodeBuff[i-2+j]+=(value[i-3]&0x03)<<4;
	}
	
	if(i>len&&(i+j+1)<128){
		for(i-=3,data=0;i<len;i++,data++) rmd[data]=value[i];
		for(;data<3;data++,i++) rmd[data]=0;
		base64EncodeBuff[i-3+j]=rmd[0]>>2;
		base64EncodeBuff[i-2+j]=rmd[1]>>4;
		base64EncodeBuff[i-1+j]=rmd[2]>>6;
		base64EncodeBuff[i+j]=rmd[2]&0x3f;
		base64EncodeBuff[i-1+j]+=(rmd[1]&0x0f)<<2;
		base64EncodeBuff[i-2+j]+=(rmd[0]&0x03)<<4;
	}
	
	return base64EncodeBuff;
	
}

