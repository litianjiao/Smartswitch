#ifndef  __MyIO_H
#define  __MyIO_H

#include "stm32f10x_usart.h"

//RECEIVEBUFFSIZE+2要能被4整除
#define RECEIVEBUFFSIZE 126

//SENDCMDBUFFSIZE要能被4整除
#define SENDCMDBUFFSIZE 128

//发送失败重发次数最大值，超过后返回发送失败错误
#define RESENDTIMES 3

//接收命令后获取队列缓存区获取失败的最大次数，超过或重置队列
#define RECEIVEERRORMAXTIMES 3

void MyIO(void* persist);

void IOInit(USART_TypeDef* usart,void (*initFuc)(void* persist),void (*disableFuc)(void* persist),void* persist);

/*
*********************************************************
* 输入模块        ok
*/
#define CMD_HEAD_TYPE1    0x80

#define CMD_HEAD_TYPE2    0x82

#define CMD_HEAD_TYPE3    0x84

#define CMD_HEAD_SURE1    0x81

#define CMD_HEAD_SURE2    0x83

#define CMD_HEAD_SURE3    0x85

#define CMD_TAIL          0x40

#define CMD_TEST          0xa0

#define   READ_ARRAY      1    //readStatu的状态信息，读取方式为读取数组

#define   READ_CMD        2    //readStatu的状态信息，读取方式为读取命令

#define   CHANGING        0    //readStatu的状态信息，读取状态改变过程中，不处理任何接收数据

#define   TYPE_ARRAY      1    //每个放入接收队列中的数据，数据头包含的字符，表示该数据为数组方式读取

#define   TYPE_CMD        2    //每个放入接收队列中的数据，数据头包含的字符，表示该数据为命令方式读取

#define   TYPE_IWDG       4    //每个放入接收队列中的数据，数据头包含的字符，表示该数据为喂狗方式读取

#define   TYPE_EXIT       8    //中断类型命令

#define   TYPE_ALIVE      16    //心跳包命令

#define   TYPE_REGISTER   32   //注册命令

#define   TYPE_CMD_NOCHECK  64   //命令不进行解密和冗余码的校验

#define   BSP_ERR_ARRAY   201u //OS_ERR_?的定义最大到164u，这边从201u开始，由于只有8位所以不能超过255
															 //在读取非数组类型的数据时读取到数组类型的数据时报的错
															 
#define   BSP_ERR_CMD     202u //在读取非命令类型的数据时读取到数组类型的数据时报的错

#define   BSP_ERR_IWDG    202u //在读取非喂狗类型的数据时读取到喂狗类型的数据时报的错

#define   BSP_ERR_UNDEFINE  202u //类型未定义

#define   BSP_ERR_READSTATU 203u //读取状态错误

#define   BSP_ERR_CRC 204u //crc校验出错

#define   BSP_ERR_DECRYPT 205u //解密出错

#define   BSP_ERR_EXIT 206u //中断

#define   BSP_ERR_ALIVE 207u //心跳包

#define   BSP_ERR_REGISTER 208u //注册

void changeReadStatu(int statu,int len);

void receiceByte(void);

void readArray(uint8_t data);

void readCmd(uint8_t data);

int excuteCmd(uint8_t type,uint8_t* cmd,uint8_t len,void* persist);

int addToReceiveBuff(uint8_t* cmd,uint8_t len,void* persist);

void sure2Cmd(uint8_t* cmd,uint8_t len,void* persist);

void resetReceiveBuff(void* persist);

/*
**********************************************************
* 输出模块       ok                                        
*/
#define MAXSENDTIMES 3

uint8_t sendBytes(USART_TypeDef* usart,uint8_t* value,int len);

uint8_t sendByte(USART_TypeDef* usart,uint8_t value); 

uint8_t addToSendBuff(uint8_t* cmd,uint8_t len,void* persist);

uint8_t removeCmdInSendBuff(uint8_t head,void* persist);

uint8_t sendCmdInSendBuff(void* persist);

void sure1Cmd(uint8_t* cmd,uint8_t len,void* persist);

void sure3Cmd(uint8_t* cmd,uint8_t len,void* persist);

void resetSendBuff(void* persist);

/*
***********************************************************
* base64编解码
*/
unsigned char  base64Decode(unsigned char *value,int len,void* persist);

unsigned char* base64Encode(unsigned char *value,int len,void* persist);

#endif
