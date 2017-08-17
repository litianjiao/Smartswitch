#include "includes.h"

void    CRCInit(void* persist){
	CRC_DeInit();
}

void    MyCRC(void* persist){
}

/*
*********************************************************************************************************
* Description: 32位CRC循环冗余校验
*
* Arguments  : temp：需要校验的数据
*							 len： 数据的总长度，包括4个字节的冗余
*
* Returns    : CRC_CHECK_ERR_NONE  :  校验结果为数据正确
*              CRC_CHECK_ERR_ERROR  :  校验结果为数据错误
*********************************************************************************************************
*/
int CRC32_Check(uint8_t *temp,uint8_t len){
	//tmp为字节数组冗余码，crc为重新生成的字节数组冗余码
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
* Description: 32位CRC循环冗余码生成并填充
*
* Arguments  : temp：需要校验的数据，该参数需要为数组类型，指针类型无法赋值（正常c语言可以，可以使用双重指针）
*							 len： 数据的总长度，包括4个字节的冗余
*
* Returns    : CRC_FILL_ERR_NONE  :  无异常
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
* Description: 32位CRC循环冗余码生成，只对4的整数倍字节生成冗余码，比如5个字节，第5个字节不参与
*
* Arguments  : temp：需要校验的数据
*							 len： 需要进行冗余码生成的数据长度
*
* Returns    : 
*********************************************************************************************************
*/
uint32_t CRC32_Create(uint8_t *temp,uint8_t len){
	CRC_ResetDR();
	return CRC_CalcBlockCRC((uint32_t *)temp,len/4);
}
