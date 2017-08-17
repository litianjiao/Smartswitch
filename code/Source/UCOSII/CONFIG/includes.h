/*
************************************************************************************************
��Ҫ�İ����ļ�

�� ��: INCLUDES.C ucos�����ļ�
�� ��: Jean J. Labrosse
************************************************************************************************
*/



#ifndef __INCLUDES_H__
#define __INCLUDES_H__

////����MyIO��������ʱ�ж�����Ŀ�ĵ�ַ������Ϊ���뿪�أ�Ĭ��Ϊzigebee�������
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































