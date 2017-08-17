#include "includes.h"
	  uint8_t relayda[9]={0};
extern uint8_t terminal_status;
extern uint8_t getrelaydata(uint8_t* datarelay);
extern uint8_t getrelaydatanow(void);

extern uint8_t hasDataInSendBuff;
extern uint8_t relayIsOpen_ack(uint8_t id);
uint8_t interruptTimes = 0;
uint32_t sceneAddrForSend=0;

static		OS_STK	APP_TASK_HEAD_STK[APP_TASK_HEAD_STK_SIZE];

static		OS_STK	APP_TASK_MAIN_STK[APP_TASK_MAIN_STK_SIZE];

void AddInterruptTimes(void){
	if(interruptTimes<6) interruptTimes++;
}

void APP_STARTUP_TASK(void* pdata){
	uint8_t i;

	(void)pdata;

	//OS_CPU_SysTickInit(OS_TICKS_PER_SEC);
	//设置系统时钟

	terminal_status = WORKING_STATUS;

	MyQueue(NULL);

	MyIO(NULL);

	Zigbee(NULL);


	//zigbee数据已更新则不再重新获取，使用NULL则一定会重新获取最新的zigbee配置信息
	i=1;
	UserInterface(getZigBee(&i),NULL);

	OSTaskCreate( APP_TASK_HEAD,
			(void*) 0,
			(OS_STK*) &APP_TASK_HEAD_STK[APP_TASK_HEAD_STK_SIZE - 1],
			(INT8U ) APP_TASK_HEAD_PRIO);

	OSTaskCreate( APP_TASK_MAIN,
			(void*) 0,
			(OS_STK*) &APP_TASK_MAIN_STK[APP_TASK_MAIN_STK_SIZE - 1],
			(INT8U ) APP_TASK_MAIN_PRIO);

	while(1)
	{
        OSTimeDlyHMSM(0, 0, 0, 500);	
	}
}// APP_STARTUP_TASK


void APP_TASK_HEAD(void* pdata){
	uint8_t times=0;

//	uint8_t i;
	while(1){
		OSTimeDlyHMSM(0, 0, 0, 200);
			
		AddInterruptTimes();
		if((times&0x1f)==0x1f) otherHead(NULL);
		sendCmdInSendBuff(NULL);
		times++;
	}
		
}

/**************************************************************
* 发送命令执行情况,如果是广播的命令无需回复
* @paramers cmd:被执行的命令
*           status：该命令的执行情况，如果reserve非空则表示reserve字节数组的大小
*           reserve：字节数组，表示value中的数据，大小由status定义
*/
void commandExecuteStatus(cmd_line cmd,uint8_t status,void* reserve){
	uint8_t i=0;
	uint8_t* value=NULL;

	//广播的命令无需回复
	//if(cmd.to==0) return;

	cmd=getCmd(cmd);
	switch(cmd.type){
		case TYPE_G_RUN:
			cmd.type=TYPE_G_RUN_ACT;
			break;
		case TYPE_G_ASK_INFO:
			cmd.type=TYPE_G_ASK_INFO_ACT;
			break;
		case TYPE_G_CONFIGURE:
			cmd.type=TYPE_G_CONFIGURE_ACT;
			break;
	}
	if(reserve!=NULL){
		value=(uint8_t*)reserve;
		for(i=0;i<status;i++) cmd.value[i]=value[i];
		cmd.length+=status;
	}else{
		cmd.value[0]=status;
		cmd.length=15;
	}
	sendCmd(CMD_HEAD_TYPE1,cmd,NULL);
}// commandExecuteStatus

void APP_TASK_MAIN(void* pdate){
	cmd_line cmd;
	uint8_t err,i,j,relaydata;
	uint8_t* data;


while(1)
{
	cmd=getNextCmd(&err,&data,0);
		
	otherMain(err,data,&cmd);
if(terminal_status!=WORKING_STATUS||err!=OS_ERR_NONE||cmd.subtyp!=TERMINAL_TYPE) continue;
 
	switch(cmd.type)
		{
		case TYPE_G_ASK_INFO:
			switch(cmd.route){
				case FUNC_RETURN:
					i = 1;
					commandExecuteStatus(cmd,2,getZigBee(&i)->MyAddr);
					break;

				case FUNC_KEYINFO:
							//relaydata=getrelaydata(relayda);
							relaydata = getrelaydatanow();
							cmd.value[0] = relaydata;
//					sendByte(ZIGBEEUSART,88);
//				sendByte(ZIGBEEUSART,relaydata);
              commandExecuteStatus(cmd,cmd.value[0],NULL);
			               

					break;

				default :
					break;
			}
			break;//end TYPE_G_ASK_INFO
        case TYPE_G_RUN:
			switch(cmd.route){
				case FUNC_OPEN:
					i = cmd.value[1];
                    if(i>=1 && i<=8)
										{
												relayda[i] = 1;
												relayOpen(i);
										}
					break;

				case FUNC_CLOSE:
					j = cmd.value[1];
                    if(j>=1 && j<=8)
										{
											relayda[j] = 0;
                    relayClose(j);
										}
                  
					break;

				default :
					break;
			}
			break;//end TYPE_G_RUN

		default :
			break;
		}//end switch(cmd.type)
	}// end while(1)
}// end APP_TASK_MAIN

