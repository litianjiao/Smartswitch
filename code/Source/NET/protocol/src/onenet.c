/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		troy
	*
	*	日期： 		2016-12-2
	*
	*	版本： 		V1.0
	*
	*	说明： 		与onenet平台的数据交互，协议层
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "net_device.h"

//协议文件
#include "onenet.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "sht20.h"
#include "fault.h"
#include "iwdg.h"
#include "hwtimer.h"
#include "selfcheck.h"

//图片数据文件
#include "image_2k.h"

//C库
#include <string.h>
#include <stdlib.h>
#include <stdio.h>




ONETNET_INFO oneNetInfo = {"4093253", "f=LzrTF6B77mlSm=vo=kmNp43oI=", 0, 0, 0, 0};
extern DATA_STREAM dataStream[];


/*
************************************************************
*	函数名称：	OneNet_Load_DataStream
*
*	函数功能：	数据流封装
*
*	入口参数：	type：发送数据的格式
*				send_buf：发送缓存指针
*				len：发送数据流的个数
*
*	返回参数：	无
*
*	说明：		封装数据流格式
************************************************************
*/
void OneNet_Load_DataStream(unsigned char type, char *send_buf, unsigned char len)
{
	
	unsigned char count = 0;
	char stream_buf[50];
	char *ptr = send_buf;

	switch(type)
	{
		case HTTP_TYPE1:
			
			UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE1\r\n");
		
			strcpy(send_buf, "{\"datastreams\":[");
			for(; count < len; count++)
			{
				if(dataStream[count].flag) //如果使能发送标志位
				{
					memset(stream_buf, 0, sizeof(stream_buf));
					switch((unsigned char)dataStream[count].dataType)
					{
						case TYPE_BOOL:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(_Bool *)dataStream[count].data);
						break;
						
						case TYPE_CHAR:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(char *)dataStream[count].data);
						break;
						
						case TYPE_UCHAR:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(unsigned char *)dataStream[count].data);
						break;
						
						case TYPE_SHORT:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(short *)dataStream[count].data);
						break;
						
						case TYPE_USHORT:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(unsigned short *)dataStream[count].data);
						break;
						
						case TYPE_INT:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(int *)dataStream[count].data);
						break;
						
						case TYPE_UINT:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%d}]},", dataStream[count].name, *(unsigned int *)dataStream[count].data);
						break;
						
						case TYPE_LONG:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%ld}]},", dataStream[count].name, *(long *)dataStream[count].data);
						break;
						
						case TYPE_ULONG:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%ld}]},", dataStream[count].name, *(unsigned long *)dataStream[count].data);
						break;
							
						case TYPE_FLOAT:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%f}]},", dataStream[count].name, *(float *)dataStream[count].data);
						break;
						
						case TYPE_DOUBLE:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":%f}]},", dataStream[count].name, *(float *)dataStream[count].data);
						break;
						
						case TYPE_GPS:
							sprintf(stream_buf, "{\"id\":\"%s\",\"datapoints\":[{\"value\":{\"lon\":1.1,\"lat\":1.1}}]},", dataStream[count].name);
						break;
					}
					
					strcat(send_buf, stream_buf);
				}
			}
			
			while(*ptr != '\0')					//找到结束符
				ptr++;
			*(--ptr) = '\0';					//将最后的','替换为结束符
			
			strcat(send_buf, "]}");
		
		break;
			
		case HTTP_TYPE3:
			
			UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE3\r\n");
		
			strcpy(send_buf, "{");
			for(; count < len; count++)
			{
				if(dataStream[count].flag) //如果使能发送标志位
				{
					memset(stream_buf, 0, sizeof(stream_buf));
					switch((unsigned char)dataStream[count].dataType)
					{
						case TYPE_BOOL:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(_Bool *)dataStream[count].data);
						break;
							
						case TYPE_CHAR:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(char *)dataStream[count].data);
						break;
							
						case TYPE_UCHAR:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(unsigned char *)dataStream[count].data);
						break;
							
						case TYPE_SHORT:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(short *)dataStream[count].data);
						break;
							
						case TYPE_USHORT:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(unsigned short *)dataStream[count].data);
						break;
							
						case TYPE_INT:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(int *)dataStream[count].data);
						break;
							
						case TYPE_UINT:
							sprintf(stream_buf, "\"%s\":%d,", dataStream[count].name, *(unsigned int *)dataStream[count].data);
						break;
							
						case TYPE_LONG:
							sprintf(stream_buf, "\"%s\":%ld,", dataStream[count].name, *(long *)dataStream[count].data);
						break;
							
						case TYPE_ULONG:
							sprintf(stream_buf, "\"%s\":%ld,", dataStream[count].name, *(unsigned long *)dataStream[count].data);
						break;
							
						case TYPE_FLOAT:
							sprintf(stream_buf, "\"%s\":%f,", dataStream[count].name, *(float *)dataStream[count].data);
						break;
							
						case TYPE_DOUBLE:
							sprintf(stream_buf, "\"%s\":%f,", dataStream[count].name, *(float *)dataStream[count].data);
						break;
					}
					
					strcat(send_buf, stream_buf);
				}
			}
			
			while(*ptr != '\0')					//找到结束符
				ptr++;
			*(--ptr) = '\0';					//将最后的','替换为结束符
			
			strcat(send_buf, "}");
		
		break;
		
		case HTTP_TYPE4:
			
			UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE4\r\n");
		
			strcpy(send_buf, "{");
			for(; count < len; count++)
			{
				if(dataStream[count].flag) //如果使能发送标志位
				{
					memset(stream_buf, 0, sizeof(stream_buf));
					switch((unsigned char)dataStream[count].dataType)
					{
						case TYPE_BOOL:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(_Bool *)dataStream[count].data);
						break;
						
						case TYPE_CHAR:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(char *)dataStream[count].data);
						break;
						
						case TYPE_UCHAR:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(unsigned char *)dataStream[count].data);
						break;
						
						case TYPE_SHORT:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(short *)dataStream[count].data);
						break;
						
						case TYPE_USHORT:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(unsigned short *)dataStream[count].data);
						break;
						
						case TYPE_INT:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(int *)dataStream[count].data);
						break;
						
						case TYPE_UINT:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%d},", dataStream[count].name, *(unsigned int *)dataStream[count].data);
						break;
						
						case TYPE_LONG:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%ld},", dataStream[count].name, *(long *)dataStream[count].data);
						break;
						
						case TYPE_ULONG:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%ld},", dataStream[count].name, *(unsigned long *)dataStream[count].data);
						break;
						
						case TYPE_FLOAT:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%f},", dataStream[count].name, *(float *)dataStream[count].data);
						break;
						
						case TYPE_DOUBLE:
							sprintf(stream_buf, "\"%s\":{\"2016-08-10T12:31:17\":%f}", dataStream[count].name, *(float *)dataStream[count].data);
						break;
					}
					
					strcat(send_buf, stream_buf);
				}
			}
			
			while(*ptr != '\0')					//找到结束符
				ptr++;
			*(--ptr) = '\0';					//将最后的','替换为结束符
			
			strcat(send_buf, "}");
		
		break;
		
		case HTTP_TYPE5:
			
			UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-HTTP_TYPE5\r\n");
		
			strcpy(send_buf, ",;");
			for(; count < len; count++)
			{
				if(dataStream[count].flag) //如果使能发送标志位
				{
					memset(stream_buf, 0, sizeof(stream_buf));
					switch((unsigned char)dataStream[count].dataType)
					{
						case TYPE_BOOL:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(_Bool *)dataStream[count].data);
						break;
						
						case TYPE_CHAR:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(char *)dataStream[count].data);
						break;
						
						case TYPE_UCHAR:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(unsigned char *)dataStream[count].data);
						break;
						
						case TYPE_SHORT:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(short *)dataStream[count].data);
						break;
						
						case TYPE_USHORT:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(unsigned short *)dataStream[count].data);
						break;
						
						case TYPE_INT:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(int *)dataStream[count].data);
						break;
						
						case TYPE_UINT:
							sprintf(stream_buf, "%s,%d;", dataStream[count].name, *(unsigned int *)dataStream[count].data);
						break;
						
						case TYPE_LONG:
							sprintf(stream_buf, "%s,%ld;", dataStream[count].name, *(long *)dataStream[count].data);
						break;
						
						case TYPE_ULONG:
							sprintf(stream_buf, "%s,%ld;", dataStream[count].name, *(unsigned long *)dataStream[count].data);
						break;
						
						case TYPE_FLOAT:
							sprintf(stream_buf, "%s,%f;", dataStream[count].name, *(float *)dataStream[count].data);
						break;
						
						case TYPE_DOUBLE:
							sprintf(stream_buf, "%s,%f;", dataStream[count].name, *(float *)dataStream[count].data);
						break;
					}
				}
				
				strcat(send_buf, stream_buf);
			}
		
		break;
	}

}

/*
************************************************************
*	函数名称：	OneNet_SendData
*
*	函数功能：	上传数据到平台
*
*	入口参数：	type：发送数据的格式
*
*	返回参数：	无
*
*	说明：		这里需要更改为自己想要上传的数据，依照格式填写
************************************************************
*/
_Bool OneNet_SendData(HTTP_TYPE  type, unsigned char len)
{
	
	char sendBuf[512];
	char streams[416];
	unsigned short strLen = 0;
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)
		return 1;
	
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);
	
	memset(sendBuf, 0, sizeof(sendBuf));
	memset(streams, 0, sizeof(streams));
	
	OneNet_Load_DataStream(type, streams, len);
	
	strLen = strlen(streams);
	
	//这个不改
	sprintf(sendBuf, "POST /devices/%s/datapoints?type=%d HTTP/1.1\r\napi-key:%s\r\nHost:api.heclouds.com\r\n"
					"Content-Length:%d\r\n\r\n",
	
					oneNetInfo.devID, type, oneNetInfo.apiKey, strLen);
	
	strcat(sendBuf, streams);
	
	NET_DEVICE_SendData((unsigned char *)sendBuf, strlen(sendBuf));
	
	faultTypeReport = FAULT_NONE; //发送之后清除标记
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);
	
	return 0;
	
}

/*
************************************************************
*	函数名称：	OneNet_Status
*
*	函数功能：	连接状态检查
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void OneNet_Status(void)
{
	
	unsigned char errType = 0;
	
	if(!oneNetInfo.netWork || NET_DEVICE_Get_DataMode() != DEVICE_DATA_MODE)	//如果网络为连接 或 不为数据收发模式
		return;
	
	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);									//设置为心跳收发模式
	
	errType = NET_DEVICE_Check();
	
	if(errType == CHECK_CLOSED || errType == CHECK_GOT_IP)
	{
		faultTypeReport = faultType = FAULT_EDP;								//标记为协议错误
		
		oneNetInfo.errCount++;
	}
	else if(errType == CHECK_NO_DEVICE)
	{
		faultTypeReport = faultType = FAULT_NODEVICE;							//标记为设备错误
		
		oneNetInfo.errCount++;
	}
	else
	{
		faultTypeReport = faultType = FAULT_NONE;								//无错误
	}
	
	NET_DEVICE_ClrData();														//情况缓存
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);									//设置为数据收发模式
	
	

}

/*
************************************************************
*	函数名称：	OneNet_Event
*
*	函数功能：	平台返回数据检测
*
*	入口参数：	dataPtr：平台返回的数据
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void OneNet_Event(unsigned char *dataPtr)
{

	//这里用来检测是否发送成功
	if(strstr((char *)dataPtr, "succ"))
	{
		UsartPrintf(USART_DEBUG, "Tips:		Send OK\r\n");
		oneNetInfo.errCount = 0;
	}
	else
	{
		UsartPrintf(USART_DEBUG, "Tips:		Send Err\r\n");
		oneNetInfo.errCount++;
	}

}
