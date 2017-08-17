/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	info.c
	*
	*	作者： 		troy
	*
	*	日期： 		2016-11-23
	*
	*	版本： 		V1.0
	*
	*	说明： 		SSID、PSWD、DEVID、APIKEY保存及读取
	*				重要：只有当外部存储器存在时，才从中读取信息
	*					  若不存在，会读取固化在代码里的信息
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//硬件驱动
#include "info.h"
#include "at24c02.h"
#include "delay.h"
#include "selfcheck.h"
#include "usart.h"

//协议
#include "onenet.h"

//网络设备
#include "net_device.h"

//C库
#include <string.h>
#include <stdlib.h>




/*
************************************************************
*	函数名称：	Info_Check
*
*	函数功能：	检查信息是否存在
*
*	入口参数：	无
*
*	返回参数：	检查结果
*
*	说明：		判断wifi的ssid和pswd是否存在
*				0-ok	1-无ssid	2-无pswd
*				3-无devid	4-无apikey
************************************************************
*/
unsigned char Info_Check(void)
{
	
	unsigned char rData = 0;
	
	AT24C02_ReadByte(SSID_ADDRESS, &rData);		//读取长度值
	if(rData == 0 || rData >= 30)				//如果为0或超出
		return 1;
        
	AT24C02_ReadByte(PSWD_ADDRESS, &rData);		//读取长度值
	if(rData == 0 || rData >= 30)				//如果为0或超出
		return 2;
	
	AT24C02_ReadByte(DEVID_ADDRESS, &rData);	//读取长度值
	if(rData == 0 || rData >= 10)				//如果为0或超出
		return 3;
	
	AT24C02_ReadByte(AKEY_ADDRESS, &rData);		//读取长度值
	if(rData == 0 || rData >= 30)				//如果为0或超出
		return 4;
        
	return 0;

}

/*
************************************************************
*	函数名称：	Info_WifiLen
*
*	函数功能：	获取信息长度
*
*	入口参数：	sp：需要检查的信息-见说明
*
*	返回参数：	检查结果
*
*	说明：		获取0-ssid长度	1-pswd长度	
*				2-devid长度		3-apikey长度
************************************************************
*/
unsigned char Info_WifiLen(unsigned char sp)
{
	
	unsigned char len = 0;
    
    switch(sp)
    {
        case 0:
            AT24C02_ReadByte(SSID_ADDRESS, &len);		//读取长度值
			if(len == 0 || len >= 30)					//如果为0或超出
				return 1;
        break;
        
        case 1:
            AT24C02_ReadByte(PSWD_ADDRESS, &len);		//读取长度值
			if(len == 0 || len >= 30)					//如果为0或超出
				return 1;
        break;
        
        case 2:
            AT24C02_ReadByte(DEVID_ADDRESS, &len);		//读取长度值
			if(len == 0 || len >= 10)					//如果为0或超出
				return 1;
        break;
        
        case 3:
            AT24C02_ReadByte(AKEY_ADDRESS, &len);		//读取长度值
			if(len == 0 || len >= 30)					//如果为0或超出
				return 1;
        break;
    }
	
	return len;

}

/*
************************************************************
*	函数名称：	Info_CountLen
*
*	函数功能：	计算字段长度
*
*	入口参数：	info：需要检查的字段
*
*	返回参数：	字段长度
*
*	说明：		计算串1发过来的字段长度   以"\r\n"结尾
************************************************************
*/
unsigned char Info_CountLen(char *info)
{

	unsigned char len = 0;
	char *buf = strstr(info, ":");		//找到':'
	
	buf++;								//偏移到下一个字节，代表字段信息开始
	while(1)
	{
		if(*buf == '\r')				//直到'\r'为止
			return len;
		
		buf++;
		len++;
	}

}

/*
************************************************************
*	函数名称：	Info_Read
*
*	函数功能：	读取ssid、pswd、devid、apikey
*
*	入口参数：	无
*
*	返回参数：	读取结果
*
*	说明：		0-成功		1-失败
************************************************************
*/
_Bool Info_Read(void)
{
    
	memset(netDeviceInfo.staName, 0, sizeof(netDeviceInfo.staName)); 								//清除之前的内容
	AT24C02_ReadBytes(SSID_ADDRESS + 1, (unsigned char *)netDeviceInfo.staName, Info_WifiLen(0));	//获取ssid长度  读出ssid
	DelayXms(10);																					//延时
			
	memset(netDeviceInfo.staPass, 0, sizeof(netDeviceInfo.staPass));								//清除之前的内容
	AT24C02_ReadBytes(PSWD_ADDRESS + 1, (unsigned char *)netDeviceInfo.staPass, Info_WifiLen(1));	//获取pswd长度  读pswd
	DelayXms(10);																					//延时
        
    memset(oneNetInfo.devID, 0, sizeof(oneNetInfo.devID));											//清除之前的内容
	AT24C02_ReadBytes(DEVID_ADDRESS + 1, (unsigned char *)oneNetInfo.devID, Info_WifiLen(2));		//获取devid长度  读devid
    DelayXms(10);																					//延时
                
    memset(oneNetInfo.apiKey, 0, sizeof(oneNetInfo.apiKey));										//清除之前的内容
	AT24C02_ReadBytes(AKEY_ADDRESS + 1, (unsigned char *)oneNetInfo.apiKey, Info_WifiLen(3));		//获取apikey长度  读apikey

    return 0;

}

/*
************************************************************
*	函数名称：	Info_Alter
*
*	函数功能：	更改wifi信息和项目信息
*
*	入口参数：	需要保存的字段
*
*	返回参数：	保存结果
*
*	说明：		0-不需要重新连接		1-需要重新连接
************************************************************
*/
_Bool Info_Alter(char *info)
{
    
    char *usart1Tmp;
    unsigned char usart1Count = 0;
	_Bool flag = 0;
            
	if((usart1Tmp = strstr(info, "SSID:")) != (void *)0)								//提取ssid
	{
		usart1Count = Info_CountLen(usart1Tmp);											//计算长度
        if(usart1Count > 0)
        {
            memset(netDeviceInfo.staName, 0, sizeof(netDeviceInfo.staName));			//清除之前的内容
            strncpy(netDeviceInfo.staName, usart1Tmp + 5, usart1Count);					//拷贝内容
            UsartPrintf(USART_DEBUG, "Tips:	Save SSID: %s\r\n", netDeviceInfo.staName);	//打印

			AT24C02_WriteByte(SSID_ADDRESS, strlen(netDeviceInfo.staName));				//保存ssid长度
			RTOS_TimeDly(2);
			AT24C02_WriteBytes(SSID_ADDRESS + 1,										//保存ssid
								(unsigned char *)netDeviceInfo.staName,
								strlen(netDeviceInfo.staName));
            
            flag = 1;
        }
	}
            
	if((usart1Tmp = strstr(info, "PSWD:")) != (void *)0)								//提取pswd
	{
		usart1Count = Info_CountLen(usart1Tmp);											//计算长度
        if(usart1Count > 0)
        {
            memset(netDeviceInfo.staPass, 0, sizeof(netDeviceInfo.staPass));			//清除之前的内容
            strncpy(netDeviceInfo.staPass, usart1Tmp + 5, usart1Count);
            UsartPrintf(USART_DEBUG, "Tips:	Save PSWD: %s\r\n", netDeviceInfo.staPass);

			AT24C02_WriteByte(PSWD_ADDRESS, strlen(netDeviceInfo.staPass));				//保存pswd长度
			RTOS_TimeDly(2);
			AT24C02_WriteBytes(PSWD_ADDRESS + 1,										//保存ssid
								(unsigned char *)netDeviceInfo.staPass,
								strlen(netDeviceInfo.staPass));
            
            flag = 1;
        }
	}
        
	if((usart1Tmp = strstr(info, "DEVID:")) != (void *)0)								//提取devid
	{
		usart1Count = Info_CountLen(usart1Tmp);											//计算长度
        if(usart1Count > 0)
        {
            memset(oneNetInfo.devID, 0, sizeof(oneNetInfo.devID));						//清除之前的内容
            strncpy(oneNetInfo.devID, usart1Tmp + 6, usart1Count);
            UsartPrintf(USART_DEBUG, "Tips:	Save DEVID: %s\r\n", oneNetInfo.devID);

			AT24C02_WriteByte(DEVID_ADDRESS, strlen(oneNetInfo.devID));					//保存devid长度
			RTOS_TimeDly(2);
			AT24C02_WriteBytes(DEVID_ADDRESS + 1,										//保存ssid
								(unsigned char *)oneNetInfo.devID,
								strlen(oneNetInfo.devID));
            
            flag = 1;
        }
	}
        
	if((usart1Tmp = strstr(info, "APIKEY:")) != (void *)0)								//提取apikey
	{
		usart1Count = Info_CountLen(usart1Tmp);											//计算长度
        if(usart1Count > 0)
        {
            memset(oneNetInfo.apiKey, 0, sizeof(oneNetInfo.apiKey));					//清除之前的内容
            strncpy(oneNetInfo.apiKey, usart1Tmp + 7, usart1Count);
            UsartPrintf(USART_DEBUG, "Tips:	Save APIKEY: %s\r\n", oneNetInfo.apiKey);

			AT24C02_WriteByte(AKEY_ADDRESS, strlen(oneNetInfo.apiKey));					//保存apikey长度
			RTOS_TimeDly(2);
			AT24C02_WriteBytes(AKEY_ADDRESS + 1,										//保存ssid
								(unsigned char *)oneNetInfo.apiKey,
								strlen(oneNetInfo.apiKey));
            
            flag = 1;
        }
	}
	
	return flag;

}
	
    return receive;
	
}

/*
************************************************************
*	函数名称：	I2C_WriteByte
*
*	函数功能：	软件IIC写一个数据
*
*	入口参数：	slaveAddr：从机地址
*				regAddr：寄存器地址
*				byte：需要写入的数据
*
*	返回参数：	0-写入成功	1-写入失败
*
*	说明：		*byte是缓存写入数据的变量的地址，因为有些寄存器只需要控制下寄存器，并不需要写入值
************************************************************
*/
_Bool I2C_WriteByte(unsigned char slaveAddr, unsigned char regAddr, unsigned char *byte)
{

	unsigned char addr = 0;

	addr = slaveAddr << 1;		//IIC地址是7bit，这里需要左移1位，bit0：1-读	0-写
	
	IIC_Start();				//起始信号
	
	IIC_SendByte(addr);			//发送设备地址(写)
	if(IIC_WaitAck(5000))		//等待应答
		return IIC_Err;
	
	IIC_SendByte(regAddr);		//发送寄存器地址
	if(IIC_WaitAck(5000))		//等待应答
		return IIC_Err;
	
	if(byte)
	{
		IIC_SendByte(*byte);	//发送数据
		if(IIC_WaitAck(5000))	//等待应答
			return IIC_Err;
	}
	
	IIC_Stop();					//停止信号
	
	return IIC_OK;

}

/*
************************************************************
*	函数名称：	I2C_ReadByte
*
*	函数功能：	软件IIC读取一个字节
*
*	入口参数：	slaveAddr：从机地址
*				regAddr：寄存器地址
*				val：需要读取的数据缓存
*
*	返回参数：	0-成功		1-失败
*
*	说明：		val是一个缓存变量的地址
************************************************************
*/
_Bool I2C_ReadByte(unsigned char slaveAddr, unsigned char regAddr, unsigned char *val)
{

	unsigned char addr = 0;

    addr = slaveAddr << 1;		//IIC地址是7bit，这里需要左移1位，bit0：1-读	0-写
	
	IIC_Start();				//起始信号
	
	IIC_SendByte(addr);			//发送设备地址(写)
	if(IIC_WaitAck(5000))		//等待应答
		return IIC_Err;
	
	IIC_SendByte(regAddr);		//发送寄存器地址
	if(IIC_WaitAck(5000))		//等待应答
		return IIC_Err;
	
	IIC_Start();				//重启信号
	
	IIC_SendByte(addr + 1);		//发送设备地址(读)
	if(IIC_WaitAck(5000))		//等待应答
		return IIC_Err;
	
	*val = IIC_RecvByte();		//接收
	IIC_NAck();					//产生一个非应答信号，代表读取接收
	
	IIC_Stop();					//停止信号
	
	return IIC_OK;

}

/*
************************************************************
*	函数名称：	I2C_WriteBytes
*
*	函数功能：	软件IIC写多个数据
*
*	入口参数：	slaveAddr：从机地址
*				regAddr：寄存器地址
*				buf：需要写入的数据缓存
*				num：数据长度
*
*	返回参数：	0-写入成功	1-写入失败
*
*	说明：		*buf是一个数组或指向一个缓存区的指针
************************************************************
*/
_Bool I2C_WriteBytes(unsigned char slaveAddr, unsigned char regAddr, unsigned char *buf, unsigned char num)
{

	unsigned char addr = 0;

	addr = slaveAddr << 1;		//IIC地址是7bit，这里需要左移1位，bit0：1-读	0-写
	
	IIC_Start();				//起始信号
	
	IIC_SendByte(addr);			//发送设备地址(写)
	if(IIC_WaitAck(5000))		//等待应答
		return IIC_Err;
	
	IIC_SendByte(regAddr);		//发送寄存器地址
	if(IIC_WaitAck(5000))		//等待应答
		return IIC_Err;
	
	while(num--)				//循环写入数据
	{
		IIC_SendByte(*buf);		//发送数据
		if(IIC_WaitAck(5000))	//等待应答
			return IIC_Err;
		
		buf++;					//数据指针偏移到下一个
		
		DelayUs(10);
	}
	
	IIC_Stop();					//停止信号
	
	return IIC_OK;

}

/*
************************************************************
*	函数名称：	I2C_ReadBytes
*
*	函数功能：	软件IIC读多个数据
*
*	入口参数：	slaveAddr：从机地址
*				regAddr：寄存器地址
*				buf：需要读取的数据缓存
*				num：数据长度
*
*	返回参数：	0-写入成功	1-写入失败
*
*	说明：		*buf是一个数组或指向一个缓存区的指针
************************************************************
*/
_Bool I2C_ReadBytes(unsigned char slaveAddr, unsigned char regAddr, unsigned char *buf, unsigned char num)
{

	unsigned short addr = 0;

    addr = slaveAddr << 1;		//IIC地址是7bit，这里需要左移1位，bit0：1-读	0-写
	
	IIC_Start();				//起始信号
	
	IIC_SendByte(addr);			//发送设备地址(写)
	if(IIC_WaitAck(5000))		//等待应答
		return IIC_Err;
	
	IIC_SendByte(regAddr);		//发送寄存器地址
	if(IIC_WaitAck(5000))		//等待应答
		return IIC_Err;
	
	IIC_Start();				//重启信号
	
	IIC_SendByte(addr + 1);		//发送设备地址(读)
	if(IIC_WaitAck(5000))		//等待应答
		return IIC_Err;
	
	while(num--)
	{
		*buf = IIC_RecvByte();
		buf++;					//偏移到下一个数据存储地址
		
		if(num == 0)
        {
           IIC_NAck();			//最后一个数据需要回NOACK
        }
        else
        {
          IIC_Ack();			//回应ACK
		}
	}
	
	IIC_Stop();
	
	return IIC_OK;

}
