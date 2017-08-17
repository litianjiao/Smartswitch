/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	main.c
	*
	*	作者： 		troy
	*
	*	日期： 		2017.04.15
	*
	*	版本： 		V1.0
	*
	*	说明： 		完成单片机初始化、外接IC初始化和任务的创建及运行
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

#include "includes.h"

//网络设备
#include "net_device.h"

//硬件驱动


//定义栈
static		OS_STK	APP_STARTUP_TASK_Stk[APP_STARTUP_TASK_STK_SIZE];


void RCC_Configuration(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
	RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC  , ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
}
/***************************************************************************//**
  * @brief  主函数，硬件初始化
  * @note   无
  * @param  无
  * @retval 无
*******************************************************************************/
int main(void)
{
	RCC_Configuration();

	IOInit(ZIGBEEUSART,zigbeeInit,NULL,NULL);

	otherInit(NULL);
  
	OSInit();

	OSTaskCreate( APP_STARTUP_TASK,
			(void*) 0,
			(OS_STK*) &APP_STARTUP_TASK_Stk[APP_STARTUP_TASK_STK_SIZE - 1],
			(INT8U ) APP_STARTUP_TASK_PRIO);


  OSStart ();

	return 0;
}


