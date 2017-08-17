#include "fault.h"

#include "usart.h"
#include "onenet.h"
#include "delay.h"
#include "selfcheck.h"

#include "net_device.h"







unsigned char faultType = FAULT_NONE; //初始为无错误类型
unsigned char faultTypeReport = FAULT_NONE; //报告错误类型用的




void Fault_Process(void)
{

	NET_DEVICE_Set_DataMode(DEVICE_CMD_MODE);
	
	switch(faultType)
	{
		case FAULT_NONE:
			
			UsartPrintf(USART_DEBUG, "WARN:	FAULT_NONE\r\n");
		
		break;
		
		case FAULT_REBOOT: //死机重启的处理
			
			UsartPrintf(USART_DEBUG, "WARN:	FAULT_REBOOT\r\n");
		
			faultType = FAULT_NONE;
			
			//死机重启后，设备会被复位，所以是重新初始化
		
		break;
		
		case FAULT_EDP: //协议出错的处理
			
			UsartPrintf(USART_DEBUG, "WARN:	FAULT_EDP\r\n");
		
			faultType = FAULT_NONE;
			
			NET_DEVICE_ReLink();
			oneNetInfo.netWork = 0; //这里设置重连平台
		
		break;
		
		case FAULT_NODEVICE: //设备出错的处理
			
			UsartPrintf(USART_DEBUG, "WARN:	FAULT_NODEVICE\r\n");
		
			faultType = FAULT_NONE;

			checkInfo.NET_DEVICE_OK = DEV_ERR;
			NET_DEVICE_Reset();
			NET_DEVICE_ReConfig(0);
			oneNetInfo.netWork = 0;
		
		break;
		
		default:
			break;
	}
	
	NET_DEVICE_Set_DataMode(DEVICE_DATA_MODE);

}
