/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	hwtimer.c
	*
	*	作者： 		troy
	*
	*	日期： 		2016-11-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		单片机定时器初始化
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//OS头文件
#include "includes.h"

//硬件驱动
#include "hwtimer.h"



TIM_INFO timInfo = {0, 0};





/*
************************************************************
*	函数名称：	Timer3_4_Init
*
*	函数功能：	Timer3或4的定时配置
*
*	入口参数：	TIMx：TIM3 或者 TIM4
*				arr：重载值
*				psc分频值
*
*	返回参数：	无
*
*	说明：		timer3和timer4只具有更新中断功能
************************************************************
*/
void Timer3_4_Init(TIM_TypeDef * TIMx, unsigned short arr, unsigned short psc)
{

	TIM_TimeBaseInitTypeDef timerInitStruct;
	NVIC_InitTypeDef nvicInitStruct;
	
	if(TIMx == TIM3)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
		
		nvicInitStruct.NVIC_IRQChannel = TIM3_IRQn;
	}
	else
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
		
		nvicInitStruct.NVIC_IRQChannel = TIM4_IRQn;
	}
	
	timerInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	timerInitStruct.TIM_Period = arr;
	timerInitStruct.TIM_Prescaler = psc;
	
	TIM_TimeBaseInit(TIMx, &timerInitStruct);
	
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE); //使能更新中断
	
	nvicInitStruct.NVIC_IRQChannelCmd = ENABLE;
	nvicInitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	nvicInitStruct.NVIC_IRQChannelSubPriority = 1;
	
	NVIC_Init(&nvicInitStruct);
	
	TIM_Cmd(TIMx, ENABLE); //使能定时器

}

/*
************************************************************
*	函数名称：	RTOS_TimerInit
*
*	函数功能：	RTOS的心跳定时初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		APB1--36MHz，APB1总线分频不为1，则定时器时钟要乘以2
*				定时5ms
************************************************************
*/
void RTOS_TimerInit(void)
{

	Timer3_4_Init(TIM3, 10000 / OS_TICKS_PER_SEC, 7199);	//72MHz，7200分频-100us，50重载值。则中断周期为100us * 50 = 5ms

}

/*
************************************************************
*	函数名称：	TIM3_IRQHandler
*
*	函数功能：	RTOS的心跳定时中断
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void TIM3_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
	{
		//do something...
		OSIntEnter();
		OSTimeTick();
		OSIntExit();
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}

}

/*
************************************************************
*	函数名称：	TIM4_IRQHandler
*
*	函数功能：	Timer7更新中断服务函数
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void TIM4_IRQHandler(void)
{

	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}

}
