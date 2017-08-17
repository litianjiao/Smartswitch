/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	MyOthers.c
	*
	*	作者： 		TROY
	*
	*	日期： 		2016-11-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		蜂鸣器、继电器初始化
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/




#include "includes.h"

uint8_t times=0;

GPIO_TypeDef*	NETID_PORT[8] = {GPIOB,GPIOB,GPIOB,GPIOB,GPIOB,GPIOB,GPIOB,GPIOA};
const uint16_t 	NETID[8] = {DIP_SWITCH_1,DIP_SWITCH_2,DIP_SWITCH_3,DIP_SWITCH_4,DIP_SWITCH_5,DIP_SWITCH_6,DIP_SWITCH_7,DIP_SWITCH_8};

uint8_t netid;

extern uint8_t terminal_status;

void otherInit(void* persist){
	//IWDGInit(NULL);
	netIdInit(NULL); //设备入网，需要修改成现有模式
	relayInit(NULL); //继电器控制
}



void otherHead(void* persist){
	//IWDGHeader(persist);
}

void otherMain(int err,uint8_t* data,void* persist){
	//IWDGMain(err,data,persist);
	testMain(err,data,persist);
}

void IWDGInit(void* persist){
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_256);
	IWDG_SetReload(IWDG_TIME);
	IWDG_ReloadCounter();
	IWDG_Enable();
}

void IWDGHeader(void* persist){
	postData(TYPE_IWDG,NULL,0);
}

void IWDGMain(int err,uint8_t* data,void* persist){
	switch(err){
		case BSP_ERR_IWDG:
		{
			IWDG_ReloadCounter();
			break;
		}
	}
}

void testMain(int err,uint8_t* data,void* persist){
	uint32_t addr;
	cmd_line* cmd=(cmd_line*)persist;
	if(terminal_status!=WORKING_STATUS||err!=OS_ERR_NONE||cmd->subtyp!=TERMINAL_TYPE||cmd->type!=TYPE_T_TEST) return;
	switch(cmd->route){
		case FUNC_CLEARFLASH_KEYSTATUS:
			erasePage(FLASHADDRESSOFKEYSTATUS);
		break;
		case FUNC_CLEARFLASH_SCENEALL:
			for(addr=FLASHADDRESSOFSCENE;addr<FLASHFORCOPING;addr+=FLASHPAGESIZE){
				erasePage(addr);
			}
		break;
		default :
		break;
	}
}

void netIdInit(void* persist){



}

uint8_t getNetId(void){
	return netid;
}
/***************************************************************************//**
  * @brief  继电器硬件初始化
  * @note   无
  * @param  无
  * @retval 无
*******************************************************************************/

void relayInit(void* reserve){
  int i;	
	GPIO_InitTypeDef GPIO_InitStructure;
	// A8-SW0  A11-SW5
	GPIO_InitStructure.GPIO_Pin=RELAY0 | RELAY5 ;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(RELAY_PORT0, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin=RELAY1 |RELAY2 |RELAY3 |RELAY4;
	GPIO_Init(RELAY_PORT0, &GPIO_InitStructure);
	
	   for(i=1;i<5;i++)
    {
	   relayOpen(i);
    }
}

uint8_t relayOpen(uint8_t id)
{
		if(id>=1 && id<=4)
		{
							if(1==id)
								{
                	if(relayIsOpen(1)) return 0;
                	GPIO_SetBits(RELAY_PORT0,RELAY1);
                	return 1;
                }          
        
            	if(2==id)
								{
                	if(relayIsOpen(2)) return 0;
                	GPIO_SetBits(RELAY_PORT0,RELAY2);
                	return 1;
                }
            
            if(3 ==id)
                {
                	if(relayIsOpen(3)) return 0;
                	GPIO_SetBits(RELAY_PORT0,RELAY3);
                	return 1;
                }           
						if(4==id)
											{
												if(relayIsOpen(4)) return 0;
												GPIO_SetBits(RELAY_PORT0,RELAY4);
												return 1;
											}								
	}
}

uint8_t relayClose(uint8_t id)
{
		if(id>=1 && id<=4)
		{
	
      if(1==id)  {
                	if(!relayIsOpen(1)) return 0;
                	GPIO_ResetBits(RELAY_PORT0,RELAY1);
                	return 1;
                  }
           

      if(2==id) 
                 {
                	if(!relayIsOpen(2)) return 0;
                	GPIO_ResetBits(RELAY_PORT0,RELAY2);
                	return 1;
                  }
           

      if(3==id) 
                {
                	if(!relayIsOpen(3)) return 0;
                	GPIO_ResetBits(RELAY_PORT0,RELAY3);
                	return 1;
                  }
            

      if(4==id) 
                {
                	if(!relayIsOpen(4)) return 0;
                	GPIO_ResetBits(RELAY_PORT0,RELAY4);
                	return 1;
                  }         
				}
		
        
}

uint8_t relayIsOpen(uint8_t id)
{
			if(id>=1 && id<=5)
			{
         if(1==id)  return GPIO_ReadInputDataBit(RELAY_PORT0,RELAY1); 
         if(2==id)  return GPIO_ReadInputDataBit(RELAY_PORT0,RELAY2); 
         if(3==id)   return GPIO_ReadInputDataBit(RELAY_PORT0,RELAY3);  
         if(4==id)   return GPIO_ReadInputDataBit(RELAY_PORT0,RELAY4); 
			}				
			
}

uint8_t relayIsOpen_ack(uint8_t id)
{
			if(id>=0 && id<=4)
			{
         if(0==id)   return GPIO_ReadInputDataBit(RELAY_PORT0,RELAY1); 
         if(1==id)   return GPIO_ReadInputDataBit(RELAY_PORT0,RELAY2); 
         if(2==id)   return GPIO_ReadInputDataBit(RELAY_PORT0,RELAY3);  
         if(3==id)   return GPIO_ReadInputDataBit(RELAY_PORT0,RELAY4);  
			}
					
}

uint8_t getrelaydata(uint8_t* datarelay)
{
	uint8_t cnt = 0;
	uint8_t data = 0;
	
	for(cnt=1;cnt<=8;cnt++)
	{
			data |= (datarelay[cnt] <<(cnt-1));
		 
	}
	return data;

}

uint8_t getrelaydatanow(void)
{
	uint8_t cnt = 0;
	uint8_t data = 0;
	
	for(cnt=0;cnt<=7;cnt++)
	{
			data |= relayIsOpen_ack(cnt)<<cnt;
		 
	}
	return data;

}


