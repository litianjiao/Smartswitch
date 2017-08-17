#ifndef  __MyIO_H
#define  __MyIO_H

#include "stm32f10x_usart.h"

//RECEIVEBUFFSIZE+2Ҫ�ܱ�4����
#define RECEIVEBUFFSIZE 126

//SENDCMDBUFFSIZEҪ�ܱ�4����
#define SENDCMDBUFFSIZE 128

//����ʧ���ط��������ֵ�������󷵻ط���ʧ�ܴ���
#define RESENDTIMES 3

//����������ȡ���л�������ȡʧ�ܵ������������������ö���
#define RECEIVEERRORMAXTIMES 3

void MyIO(void* persist);

void IOInit(USART_TypeDef* usart,void (*initFuc)(void* persist),void (*disableFuc)(void* persist),void* persist);

/*
*********************************************************
* ����ģ��        ok
*/
#define CMD_HEAD_TYPE1    0x80

#define CMD_HEAD_TYPE2    0x82

#define CMD_HEAD_TYPE3    0x84

#define CMD_HEAD_SURE1    0x81

#define CMD_HEAD_SURE2    0x83

#define CMD_HEAD_SURE3    0x85

#define CMD_TAIL          0x40

#define CMD_TEST          0xa0

#define   READ_ARRAY      1    //readStatu��״̬��Ϣ����ȡ��ʽΪ��ȡ����

#define   READ_CMD        2    //readStatu��״̬��Ϣ����ȡ��ʽΪ��ȡ����

#define   CHANGING        0    //readStatu��״̬��Ϣ����ȡ״̬�ı�����У��������κν�������

#define   TYPE_ARRAY      1    //ÿ��������ն����е����ݣ�����ͷ�������ַ�����ʾ������Ϊ���鷽ʽ��ȡ

#define   TYPE_CMD        2    //ÿ��������ն����е����ݣ�����ͷ�������ַ�����ʾ������Ϊ���ʽ��ȡ

#define   TYPE_IWDG       4    //ÿ��������ն����е����ݣ�����ͷ�������ַ�����ʾ������Ϊι����ʽ��ȡ

#define   TYPE_EXIT       8    //�ж���������

#define   TYPE_ALIVE      16    //����������

#define   TYPE_REGISTER   32   //ע������

#define   TYPE_CMD_NOCHECK  64   //������н��ܺ��������У��

#define   BSP_ERR_ARRAY   201u //OS_ERR_?�Ķ������164u����ߴ�201u��ʼ������ֻ��8λ���Բ��ܳ���255
															 //�ڶ�ȡ���������͵�����ʱ��ȡ���������͵�����ʱ���Ĵ�
															 
#define   BSP_ERR_CMD     202u //�ڶ�ȡ���������͵�����ʱ��ȡ���������͵�����ʱ���Ĵ�

#define   BSP_ERR_IWDG    202u //�ڶ�ȡ��ι�����͵�����ʱ��ȡ��ι�����͵�����ʱ���Ĵ�

#define   BSP_ERR_UNDEFINE  202u //����δ����

#define   BSP_ERR_READSTATU 203u //��ȡ״̬����

#define   BSP_ERR_CRC 204u //crcУ�����

#define   BSP_ERR_DECRYPT 205u //���ܳ���

#define   BSP_ERR_EXIT 206u //�ж�

#define   BSP_ERR_ALIVE 207u //������

#define   BSP_ERR_REGISTER 208u //ע��

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
* ���ģ��       ok                                        
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
* base64�����
*/
unsigned char  base64Decode(unsigned char *value,int len,void* persist);

unsigned char* base64Encode(unsigned char *value,int len,void* persist);

#endif
