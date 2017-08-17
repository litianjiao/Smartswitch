/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	led.c
	*
	*	���ߣ� 		TROY
	*
	*	���ڣ� 		2016-11-23
	*
	*	�汾�� 		V1.0
	*
	*	˵���� 		ʹ��TIM��ʱ�����PWM����LED��ʼ��������LED
	*
	*	�޸ļ�¼��	
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//LEDͷ�ļ�
#include "led.h"




LED_STATUS ledStatus;


/*
************************************************************
*	�������ƣ�	Led_Init
*
*	�������ܣ�	LED��ʼ��
*
*	��ڲ�����	��
*
*	���ز�����	��
*
*	˵����		LED4-PB6	LED5-PB7	LED6-PB8	LED7-PB9
				�ߵ�ƽ�ص�		�͵�ƽ����
************************************************************
*/
void Led_Init(void* reserve)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;

	//ʹ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//IO����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = LED0 | LED1 | LED2 |LED3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//IO��ʼ��
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
*	�������ƣ�	Led0_Set
*
*	�������ܣ�	LED0����
*
*	��ڲ�����	status��LED_ON-����	LED_OFF-�ص�
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Led0_Set(LED_ENUM status)
{

	GPIO_WriteBit(LED_PORT_1, LED0, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led0Sta = status;

}/*
************************************************************
*	�������ƣ�	Led1_Set
*
*	�������ܣ�	LED1����
*
*	��ڲ�����	status��LED_ON-����	LED_OFF-�ص�
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Led1_Set(LED_ENUM status)
{

	GPIO_WriteBit(LED_PORT_1, LED1, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led1Sta = status;

}
/*
************************************************************
*	�������ƣ�	Led2_Set
*
*	�������ܣ�	LED2����
*
*	��ڲ�����	status��LED_ON-����	LED_OFF-�ص�
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Led2_Set(LED_ENUM status)
{

	GPIO_WriteBit(LED_PORT_2, LED2, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led2Sta = status;

}

/*
************************************************************
*	�������ƣ�	Led3_Set
*
*	�������ܣ�	LED3����
*
*	��ڲ�����	status��LED_ON-����	LED_OFF-�ص�
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Led3_Set(LED_ENUM status)
{

	GPIO_WriteBit(LED_PORT_2, LED3, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led3Sta = status;

}

/*
************************************************************
*	�������ƣ�	Led4_Set
*
*	�������ܣ�	LED4����
*
*	��ڲ�����	status��LED_ON-����	LED_OFF-�ص�
*
*	���ز�����	��
*
*	˵����		
************************************************************
*/
void Led4_Set(LED_ENUM status)
{

	GPIO_WriteBit(LED_PORT_2, LED4, status != LED_ON ? Bit_SET : Bit_RESET);
	ledStatus.Led4Sta = status;

}

/*
************************************************************
*	�������ƣ�	Led5_Set
*
*	�������ܣ�	LED5����
*
*	��ڲ�����	status��LED_ON-����	LED_OFF-�ص�
*
*	���ز�����	��
*
*	˵����		
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
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//ʹ�ܶ�ʱ��3ʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��

//	GPIO_InitStructure.GPIO_Pin = LED0 | LED1 ; //TIM_CH12
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(LED_PORT_1, &GPIO_InitStructure);//��ʼ��GPIO   
//	GPIO_InitStructure.GPIO_Pin = LED2 | LED3 ; //TIM_CH34
//	GPIO_Init(LED_PORT_2, &GPIO_InitStructure);//��ʼ��GPIO     
//	
//  TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4; //����ʱ�ӷָ� 1/4
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
//	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ	
//	
//	//��ʼ��TIM3 Channel234 PWMģʽ	 
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
// 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը� 
//	TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC1
//	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���

//	TIM_OC2Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC1	
//	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
//	
//  TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC1	
//	TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
//	
//  TIM_OC4Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC1	
//	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
//	
//	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM4
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
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//ʹ�ܶ�ʱ��4ʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��

//	GPIO_InitStructure.GPIO_Pin = LED4 | LED5 ; //TIM_CH2
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(LED_PORT_2, &GPIO_InitStructure);//��ʼ��GPIO     
//	
//  TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
//	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
//	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV4; //����ʱ�ӷָ�:TDTS = Tck_tim
//	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
//	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ	
//	
//	//��ʼ��TIM4 Channel2 PWMģʽ	 
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
// 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը� 
//	TIM_OC1Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC1
//	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���

//	TIM_OC2Init(TIM4, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM4 OC1	
//	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR2�ϵ�Ԥװ�ؼĴ���
//	

//	TIM_Cmd(TIM4, ENABLE);  //ʹ��TIM4
//	
//	TIM_SetCompare1(TIM4,1150);
//	TIM_SetCompare2(TIM4,1150);
//	

//}



