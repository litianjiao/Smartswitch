#ifndef  __Zigbee_H
#define  __Zigbee_H

#include <stdint.h>

#define ZIGBEEUSART              USART1
#define ZIGBEERXD                GPIO_Pin_10
#define ZIGBEERXDPORT            GPIOA
#define ZIGBEETXD                GPIO_Pin_9
#define ZIGBEETXDPORT            GPIOA

#define    READ_LOCAL_ZIGBEE_CONFIG_MESSAGE_SIZE      74  //读取本地zigbee配置信息的长度  

typedef struct{
	uint8_t  DevName[16];
	uint8_t  DevPwd[16];
	uint8_t		DevMode;
	uint8_t		Chan;
	uint8_t		PanID[2];
	uint8_t		MyAddr[2];
	uint8_t		MyAddr_new[2];
	uint8_t    MyIEEE[8];
	uint8_t		DstAddr[2];
	uint8_t    DstIEEE[8];
	uint8_t    Reserve;
	uint8_t		PowerLevel;
	uint8_t   ReTryNum;
	uint8_t   TranTimeout;
	uint8_t		Serial_Rate;
	uint8_t   Serial_DataB;
	uint8_t   Serial_StopB;
	uint8_t   Serial_ParityB;
	uint8_t		SendMode;
} zigbee_struct;

void zigbeeInit(void* persist);

void Zigbee(void* persist);

void disableZigbee(void* persist);

void setByteCheck(uint8_t* cmd,int len);

zigbee_struct* getZigBee(void* persist);

int setZigBee(zigbee_struct* value);

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#endif
