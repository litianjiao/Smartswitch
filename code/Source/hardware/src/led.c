/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	led.c
	*
	*	作者： 		TROY
	*
	*	日期： 		2016-11-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		使用TIM定时器输出PWM控制LED初始化，亮灭LED
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//LED头文件
#include "led.h"




LED_STATUS ledStatus;


/*
************************************************************
*	函数名称：	Led_Init
*
*	函数功能：	LED初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		LED4-PB6	LED5-PB7	LED6-PB8	LED7-PB9
				高电平关灯		低电平开灯
************************************************************
*/
void Led_Init(void* reserve)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

	//使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//IO配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED0 | LED1 | LED2 |LED3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//IO初始化
	GPIO_Init(LED_PORT_1, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin=LED4 |LED5;
	GPIO_Init(LED_PORT_2, &GPIO_InitStructure);	
    
    Led0_Set(LED_OFF);
    Led1_Set(LED_OFF);
    Led2_Set(LED_OFF);
    Led3_Set(LED_OFF);
    Led4_Set(LED_OFF);
    Led5_Set(LED_OFF);

}
/*
************************************************************
*	函数名称：	Led0_Set
*
*	函数功能：	LED0控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led0_Set(LED_ENUM status)
{

	GPIO_WriteBit(LED_PORT_1, LED0, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led0Sta = status;

}/*
************************************************************
*	函数名称：	Led1_Set
*
*	函数功能：	LED1控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led1_Set(LED_ENUM status)
{

	GPIO_WriteBit(LED_PORT_1, LED1, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led1Sta = status;

}
/*
************************************************************
*	函数名称：	Led2_Set
*
*	函数功能：	LED2控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led2_Set(LED_ENUM status)
{

	GPIO_WriteBit(LED_PORT_2, LED2, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led2Sta = status;

}

/*
************************************************************
*	函数名称：	Led3_Set
*
*	函数功能：	LED3控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led3_Set(LED_ENUM status)
{

	GPIO_WriteBit(LED_PORT_2, LED3, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led3Sta = status;

}

/*
************************************************************
*	函数名称：	Led4_Set
*
*	函数功能：	LED4控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led4_Set(LED_ENUM status)
{

	GPIO_WriteBit(LED_PORT_2, LED4, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led4Sta = status;

}

/*
************************************************************
*	函数名称：	Led5_Set
*
*	函数功能：	LED5控制
*
*	入口参数：	status：LED_ON-开灯	LED_OFF-关灯
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led5_Set(LED_ENUM status)
{

	GPIO_WriteBit(LED_PORT_2, LED5, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led5Sta = status;

}


//void TIM3_PWM_Init(u16 arr,u16 psc)
//{
// 	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM_OCInitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟

//	GPIO_InitStructure.GPIO_Pin = LED0 | LED1 ; //TIM_CH12
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(LED_PORT_1, &GPIO_InitStructure);//初始化GPIO   
//	GPIO_InitStructure.GPIO_Pin = LED2 | LED3 ; //TIM_CH34
//	GPIO_Init(LED_PORT_2, &GPIO_InitStructure);//初始化GPIO     
//	
//  TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4; //设置时钟分割 1/4
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位	
//	
//	//初始化TIM3 Channel234 PWM模式	 
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
// 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高 
//	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC1
//	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器

//	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC1	
//	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
//	
//  TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC1	
//	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
//	
//  TIM_OC4Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC1	
//	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
//	
//	TIM_Cmd(TIM3, ENABLE);  //使能TIM4
//	
//	TIM_SetCompare1(TIM3,1150);
//	TIM_SetCompare2(TIM3,1150);	
//	TIM_SetCompare3(TIM3,1150);	
//	TIM_SetCompare4(TIM3,1150);
//}	

//void TIM4_PWM_Init(u16 arr,u16 psc)
//{
// 	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM_OCInitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器4时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟

//	GPIO_InitStructure.GPIO_Pin = LED4 | LED5 ; //TIM_CH2
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(LED_PORT_2, &GPIO_InitStructure);//初始化GPIO     
//	
//  TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4; //设置时钟分割:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
//	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位	
//	
//	//初始化TIM4 Channel2 PWM模式	 
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
// 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高 
//	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC1
//	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器

//	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC1	
//	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM3在CCR2上的预装载寄存器
//	

//	TIM_Cmd(TIM4, ENABLE);  //使能TIM4
//	
//	TIM_SetCompare1(TIM4,1150);
//	TIM_SetCompare2(TIM4,1150);
//	

//}



