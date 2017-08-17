#include "includes.h"

zigbee_struct zigbee;

uint8_t zigbeeHasInit=0;

extern int readStatu;

extern int READ_ARRAY_LENGTH;

extern uint8_t  receive_buff[RECEIVEBUFFSIZE+2];

void zigbeeInit(void* persist){
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	

	/* USART1的端口配置 */

	//PA9	TXD
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = ZIGBEETXD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ZIGBEETXDPORT, &GPIO_InitStructure);
	
	//PA10	RXD
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = ZIGBEERXD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(ZIGBEERXDPORT, &GPIO_InitStructure);
	

	/* USART1的基本配置 */

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		//无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;						//接收和发送
	USART_InitStructure.USART_Parity = USART_Parity_No;									//无校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;								//1位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;							//8位数据位
	USART_Init(ZIGBEEUSART, &USART_InitStructure);
	
	
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_ITConfig(ZIGBEEUSART,USART_IT_RXNE,ENABLE); //使能接收中断
	
	
}

void Zigbee(void* persist){
	uint8_t i=0;
	getZigBee(NULL);
	//确认本地网络号是否正确
	if(zigbee.MyAddr_new[0]!=zigbee.MyIEEE[2]){ 
		zigbee.MyAddr_new[0]=zigbee.MyIEEE[2];
		i++;
	}
	if(zigbee.MyAddr_new[1]!=zigbee.MyIEEE[3]){
		zigbee.MyAddr_new[1]=zigbee.MyIEEE[3];
		i++;
	}
	//确认发送模式是否广播
	if(zigbee.SendMode!=1){
		zigbee.SendMode=1;
		i++;
	}
	//如果有字段修改，同步修改数据到Zigbee设备
	if(i){
		setZigBee(&zigbee);
	}
	//同步时使用新地址，同步后需要修改旧地址为新地址，或者重新调用下getZigBee(NULL);将本地数据全部更新下
	if(zigbee.MyAddr_new[0]!=zigbee.MyAddr[0]) zigbee.MyAddr_new[0]=zigbee.MyAddr[0];
	if(zigbee.MyAddr_new[1]!=zigbee.MyAddr[1]) zigbee.MyAddr_new[1]=zigbee.MyAddr[1];
	
	zigbeeHasInit=1;
}

void disableZigbee(void* persist){
	USART_Cmd(ZIGBEEUSART, DISABLE); //使能USART2
}

/*
*********************************************************************************************************
* Description: 为命令添加字节校验，即除校验位（len-1）外的数据之和
*
* Arguments  : cmd 命令的字节数组
*							 len 数组的长度
*
* Returns    : 
*********************************************************************************************************
*/
void setByteCheck(uint8_t* cmd,int len){
	uint8_t tmp=0;
	uint8_t i=0;
	for(;i<len-1;i++){
		tmp+=cmd[i];
	}
	cmd[len-1]=tmp;
}


uint8_t readZigBeeCmd[5]={0xAB,0xBC,0xCD,0xD1,0x67};

/*
*********************************************************************************************************
* Description: 获取芯片的配置信息，函数结束后读取方式不会发生变化
*
* Arguments  : 
*
* Returns    : zigbee的结构体
*********************************************************************************************************
*/
zigbee_struct* getZigBee(void* persist){
	uint8_t* tmp;
	uint8_t err;
	uint8_t i,t;
	int statu=readStatu,len=READ_ARRAY_LENGTH;  									//保存原来读取方式，以便恢复
	
	if(zigbeeHasInit&&persist!=NULL&&((uint8_t*)persist)[0]==1) return &zigbee;
	
	if(readStatu!=READ_ARRAY){
		changeReadStatu(READ_ARRAY,READ_LOCAL_ZIGBEE_CONFIG_MESSAGE_SIZE);
	}
	setByteCheck(readZigBeeCmd,5);
	do{
		sendBytes(USART2,readZigBeeCmd,5);
		tmp=pullElement(&err,NULL);
		if(err==OS_ERR_NONE&&tmp[0]==TYPE_ARRAY&&tmp[2]==0xab&&tmp[3]==0xbc&&tmp[4]==0xcd&&tmp[5]==0xd1) break;
		removeAllElement(NULL);
//		readStatu=CHANGING;                                          //挂起接收队列，避免清空时再往队列中添加数据
//		changeReadStatu(READ_ARRAY,READ_LOCAL_ZIGBEE_CONFIG_MESSAGE_SIZE);
	}while(1);
	i=6;
	for(t=0;t<16;i++) zigbee.DevName[t++]=tmp[i];
	for(t=0;t<16;i++) zigbee.DevName[t++]=tmp[i];
	zigbee.DevMode=tmp[i++]; 
	zigbee.Chan=tmp[i++]; 
	zigbee.PanID[0]=tmp[i++]; 
	zigbee.PanID[1]=tmp[i++]; 
	zigbee.MyAddr_new[0]=tmp[i++]; 
	zigbee.MyAddr_new[1]=tmp[i++];  
	zigbee.MyAddr[0]=zigbee.MyAddr_new[0];
	zigbee.MyAddr[1]=zigbee.MyAddr_new[1];
	for(t=0;t<8;i++) zigbee.MyIEEE[t++]=tmp[i]; 
	zigbee.DstAddr[0]=tmp[i++]; 
	zigbee.DstAddr[1]=tmp[i++];  
	for(t=0;t<8;i++) zigbee.DstIEEE[t++]=tmp[i]; 
	zigbee.Reserve=tmp[i++]; 
	zigbee.PowerLevel=tmp[i++];
	zigbee.ReTryNum=tmp[i++];
	zigbee.TranTimeout=tmp[i++];
	zigbee.Serial_Rate=tmp[i++];
	zigbee.Serial_DataB=tmp[i++];
	zigbee.Serial_StopB=tmp[i++];
	zigbee.Serial_ParityB=tmp[i++];
	zigbee.SendMode=tmp[i++];
	changeReadStatu(statu,len);    //恢复原来的读取方式

	return &zigbee;
}

/*
*********************************************************************************************************
* Description: 设置芯片的配置信息，函数结束后读取方式不会发生变化
*
* Arguments  : zigbee结构体
*
* Returns    : 
*********************************************************************************************************
*/
int setZigBee(zigbee_struct* value){
	uint8_t* re;
	uint8_t i,t;
	int statu=readStatu,len=READ_ARRAY_LENGTH;  //保存原来读取方式，以便恢复
	i=0;
	changeReadStatu(CHANGING,7);
	
	//设值
	//利用目前空闲receive_buff缓存，该缓存作为READ_CMD时才会被使用
	receive_buff[i++]=0xab;
	receive_buff[i++]=0xbc;
	receive_buff[i++]=0xcd;
	receive_buff[i++]=0xd6;
	receive_buff[i++]=value->MyAddr[0];
	receive_buff[i++]=value->MyAddr[1];
	for(t=0;t<16;i++) receive_buff[i]=value->DevName[t++];
	for(t=0;t<16;i++) receive_buff[i]=value->DevPwd[t++];
	receive_buff[i++]=value->DevMode; 
	receive_buff[i++]=value->Chan; 
	receive_buff[i++]=value->PanID[0]; 
	receive_buff[i++]=value->PanID[1]; 
	receive_buff[i++]=value->MyAddr_new[0]; 
	receive_buff[i++]=value->MyAddr_new[1];  
	for(t=0;t<8;i++) receive_buff[i]=value->MyIEEE[t++]; 
	receive_buff[i++]=value->DstAddr[0]; 
	receive_buff[i++]=value->DstAddr[1];  
	for(t=0;t<8;i++) receive_buff[i]=value->DstIEEE[t++]; 
	receive_buff[i++]=value->Reserve; 
	receive_buff[i++]=value->PowerLevel;
	receive_buff[i++]=value->ReTryNum;
	receive_buff[i++]=value->TranTimeout;
	receive_buff[i++]=value->Serial_Rate;
	receive_buff[i++]=value->Serial_DataB;
	receive_buff[i++]=value->Serial_StopB;
	receive_buff[i++]=value->Serial_ParityB;
	receive_buff[i++]=value->SendMode;
	setByteCheck(receive_buff,72);
	changeReadStatu(READ_ARRAY,7);
	do{
		sendBytes(USART2,receive_buff,72);
		do{
			re=pullElement(&t,0);
			if(t==OS_ERR_TIMEOUT) break;
			if(t!=OS_ERR_NONE) break;
			if(re[2]==0xab&&re[3]==0xbc&&re[4]==0xcd&&re[5]==0xd6&&re[8]==0x00) break;
		}while(1);
		if(t==OS_ERR_NONE) break;
	}while(1);
	
	//复位
	receive_buff[0]=0xab;
	receive_buff[1]=0xbc;
	receive_buff[2]=0xcd;
	receive_buff[3]=0xd9;
	receive_buff[4]=value->MyAddr[0];
	receive_buff[5]=value->MyAddr[1];
	receive_buff[6]=0x00;
	receive_buff[7]=0x01;
	setByteCheck(receive_buff,9);
	sendBytes(USART2,receive_buff,9);
	
	changeReadStatu(statu,len);    //恢复原来的读取方式
	
	OSTimeDlyHMSM(0, 0, 0, 10);
	
	return 1;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(ZIGBEEUSART, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(ZIGBEEUSART, USART_FLAG_TC) == RESET)
  {}

  return ch;
}
