#include "includes.h"

void    CRCInit(void* persist){
	CRC_DeInit();
}

void    MyCRC(void* persist){
}

/*
*********************************************************************************************************
* Description: 32λCRCѭ������У��
*
* Arguments  : temp����ҪУ�������
*							 len�� ���ݵ��ܳ��ȣ�����4���ֽڵ�����
*
* Returns    : CRC_CHECK_ERR_NONE  :  У����Ϊ������ȷ
*              CRC_CHECK_ERR_ERROR  :  У����Ϊ���ݴ���
*********************************************************************************************************
*/
int CRC32_Check(uint8_t *temp,uint8_t len){
	//tmpΪ�ֽ����������룬crcΪ�������ɵ��ֽ�����������
	uint32_t tmp,crc;
	
	tmp=temp[len-1];
	tmp+=temp[len-2]<<8;
	tmp+=temp[len-3]<<16;
	tmp+=temp[len-4]<<24;
	
	crc=CRC32_Create(temp,len-4);
	
	if(crc==tmp) return CRC_CHECK_ERR_NONE;
	return CRC_CHECK_ERR_ERROR;
}

/*
*********************************************************************************************************
* Description: 32λCRCѭ�����������ɲ����
*
* Arguments  : temp����ҪУ������ݣ��ò�����ҪΪ�������ͣ�ָ�������޷���ֵ������c���Կ��ԣ�����ʹ��˫��ָ�룩
*							 len�� ���ݵ��ܳ��ȣ�����4���ֽڵ�����
*
* Returns    : CRC_FILL_ERR_NONE  :  ���쳣
*********************************************************************************************************
*/
int CRC32_Fill(uint8_t *temp,uint8_t len){
	
	uint32_t crc_data=CRC32_Create(temp,len-4);
	
	temp[len-4]=crc_data>>24;
	temp[len-3]=(crc_data>>16)&0x000000ff;
	temp[len-2]=(crc_data>>8)&0x000000ff;
	temp[len-1]=crc_data;
	
	return CRC_FILL_ERR_NONE;
}

/*
*********************************************************************************************************
* Description: 32λCRCѭ�����������ɣ�ֻ��4���������ֽ����������룬����5���ֽڣ���5���ֽڲ�����
*
* Arguments  : temp����ҪУ�������
*							 len�� ��Ҫ�������������ɵ����ݳ���
*
* Returns    : 
*********************************************************************************************************
*/
uint32_t CRC32_Create(uint8_t *temp,uint8_t len){
	CRC_ResetDR();
	return CRC_CalcBlockCRC((uint32_t *)temp,len/4);
}
