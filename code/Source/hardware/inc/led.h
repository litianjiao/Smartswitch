#ifndef _LED_H_
#define _LED_H_


#define LED0              GPIO_Pin_6
#define LED1              GPIO_Pin_7
#define LED2              GPIO_Pin_0
#define LED3              GPIO_Pin_1
#define LED4              GPIO_Pin_10
#define LED5              GPIO_Pin_11

#define LED_PORT_1        GPIOA
#define LED_PORT_2        GPIOB

#define SR_CYC		       //2.5k 

typedef struct
{
	_Bool Led0Sta;
	_Bool Led1Sta;
	_Bool Led2Sta;
	_Bool Led3Sta;
	_Bool Led4Sta;
	_Bool Led5Sta;

} LED_STATUS;

extern LED_STATUS ledStatus;

typedef enum
{

	LED_OFF = 0,
	LED_ON

} LED_ENUM;




void Led_Init(void* reserve);

void Led0_Set(LED_ENUM status);

void Led1_Set(LED_ENUM status);

void Led2_Set(LED_ENUM status);

void Led3_Set(LED_ENUM status);

void Led4_Set(LED_ENUM status);

void Led5_Set(LED_ENUM status);

#endif
