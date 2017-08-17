/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2006; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              with the
*                                   STM3210B-EVAL Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.10
* Programmer(s) : BAN
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

//#define  APP_OS_PROBE_EN                         DEF_ENABLED
//#define  APP_PROBE_COM_EN                        DEF_ENABLED

/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/

#define APP_TASK_HEAD_PRIO                4u

#define APP_TASK_MAIN_PRIO                5u

#define	APP_STARTUP_TASK_STK_SIZE					256u


#define	APP_STARTUP_TASK_PRIO							9u
#define	OS_TASK_TMR_PRIO                  (OS_LOWEST_PRIO - 2u)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define APP_TASK_HEAD_STK_SIZE                256u

#define APP_TASK_MAIN_STK_SIZE               256u



/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/
#if 0

#define  TRACE_LEVEL_OFF                               0
#define  TRACE_LEVEL_INFO                              1
#define  TRACE_LEVEL_DBG                               2


#define  APP_TRACE_LEVEL                        TRACE_LEVEL_OFF
#define  APP_TRACE                               printf

#define  APP_TRACE_INFO(x)               ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                ((APP_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_TRACE x) : (void)0)

#endif
/*
*********************************************************************************************************
*                                                 PROBE
*********************************************************************************************************
*/

//#define  OS_PROBE_TASK                                     0    /* Task will be created for uC/Probe OS Plug-In.        */
//#define  OS_PROBE_TMR_32_BITS                              0    /* uC/Probe OS Plugin timer is a 16-bit timer.          */
//#define  OS_PROBE_TIMER_SEL                                2    /* Select timer 2.                                      */
//#define  OS_PROBE_HOOKS_EN                                 1    /* Hooks to update OS_TCB profiling members included.   */
//#define  OS_PROBE_USE_FP                                   1




#endif
