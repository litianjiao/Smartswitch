/*
************************************************************************************************
主要的包含文件

文 件: INCLUDES.C ucos包含文件
作 者: Jean J. Labrosse
************************************************************************************************
*/



#ifndef __INCLUDES_H__
#define __INCLUDES_H__

////定义MyIO接收数据时判断数据目的地址的依据为拨码开关，默认为zigebee的网络号
//#define USINNETID

/*------C_Lib---*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
/*-------OS---------*/
#include "ucos_ii.h"
#include "os_cpu.h"
#include "os_cfg.h"

/*-----MCU------*/
#include <stm32f10x.h>
#include "stm32f10x_conf.h"

/*------App------*/
#include "app.h"

/*------Interface------*/
#include "MyIO.h"
#include "UserInterface.h"
#include "MyOthers.h"
#include "MyQueue.h"
/*------HardWare-------*/
#include "Zigbee.h"
#include "Flash.h"
#include "Led.h"

#endif































