#ifndef _DELAY_H_
#define _DELAY_H_


#include "includes.h"




#define RTOS_ENTER_CRITICAL()						OS_ENTER_CRITICAL()
#define RTOS_EXIT_CRITICAL()						OS_EXIT_CRITICAL()

#define RTOS_TimeDly(ticks) 						OSTimeDly(ticks)
#define RTOS_TimeDlyHMSM(hour, min, sec, ms)		OSTimeDlyHMSM(hour, min, sec, ms)




void Delay_Init(void);

void DelayUs(unsigned short us);

void DelayXms(unsigned short ms);

void DelayMs(unsigned short ms);

#endif
