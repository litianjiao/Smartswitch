#ifndef  __MyOthers_H
#define  __MyOthers_H

// 8s 1250
#define	IWDG_TIME		1250

//拨码开关
#define DIP_SWITCH_1				GPIO_Pin_9
#define DIP_SWITCH_1_PORT		GPIOA

#define DIP_SWITCH_2				GPIO_Pin_8
#define DIP_SWITCH_2_PORT		GPIOA

#define DIP_SWITCH_3				GPIO_Pin_7
#define DIP_SWITCH_3_PORT		GPIOA

#define DIP_SWITCH_4				GPIO_Pin_6
#define DIP_SWITCH_4_PORT		GPIOA

#define DIP_SWITCH_5				GPIO_Pin_5
#define DIP_SWITCH_5_PORT		GPIOA

#define DIP_SWITCH_6				GPIO_Pin_4
#define DIP_SWITCH_6_PORT		GPIOA

#define DIP_SWITCH_7				GPIO_Pin_3
#define DIP_SWITCH_7_PORT		GPIOA

#define DIP_SWITCH_8				GPIO_Pin_15
#define DIP_SWITCH_8_PORT		GPIOB

//继电器引脚
#define	RELAY0							  GPIO_Pin_8
#define	RELAY1							  GPIO_Pin_15
#define	RELAY2							  GPIO_Pin_14
#define	RELAY3							  GPIO_Pin_13
#define	RELAY4							  GPIO_Pin_12
#define	RELAY5							  GPIO_Pin_11


#define	RELAY_PORT0				  GPIOA
#define	RELAY_PORT1				  GPIOB


void otherInit(void* persist);

void MyOthers(void* persist);

void otherHead(void* persist);

void otherMain(int err,uint8_t* data,void* persist);


void IWDGInit(void* persist);

void IWDGHeader(void* persist);

void IWDGMain(int err,uint8_t* data,void* persist);


void testMain(int err,uint8_t* data,void* persist);


void netIdInit(void* persist);

uint8_t getNetId(void);


void relayInit(void* reserve);

uint8_t relayOpen(uint8_t id);
uint8_t relayClose(uint8_t id);
uint8_t relayIsOpen(uint8_t id);
uint8_t relayIsOpen_ack(uint8_t id);
uint8_t getrelaydata(uint8_t* datarelay);
uint8_t getrelaydatanow(void);



#endif
