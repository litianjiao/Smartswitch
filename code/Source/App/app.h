#ifndef _APP_H_
#define _APP_H_

//终端类型，0x11开关
#define TERMINAL_TYPE             0x11

//定义终端状态值
#define UNINIT_STATUS             0x00
#define	WORKING_STATUS					  0x01
#define	REGISTER_NET_STATUS			  0x02
#define REGISTER_KEY_STATUS   	  0x03
#define REGISTER_RELATION_STATUS  0x04
#define STATUS_CHANGING           0x10

#define READ_KEY_STATUS           0x11

//定义协议值
#define	TYPE_G_BRO_INFO			0x03
#define	TYPE_G_SEND_CONF		0x04
#define	TYPE_T_ASK_LOCMAC		0x05
#define	TYPE_T_ACT_LOCMAC		0x06
#define	TYPE_T_ACT_RESUCC		0x07

#define	TYPE_G_RUN					0x11
#define	TYPE_G_RUN_ACT			0x14

#define	TYPE_G_ASK_INFO			0x12
#define	TYPE_G_ASK_INFO_ACT	0x10

#define TYPE_G_CONFIGURE    0x30
#define TYPE_G_CONFIGURE_ACT    0x31

#define	TYPE_T_ASK_RUN			0x13

#define	TYPE_T_RET_INFO			0x15
#define TYPE_T_ACT_INFO     0x99

#define TYPE_T_TEST         0xff

//功能点
//TYPE_G_RUN
#define FUNC_OPEN           0x20
#define FUNC_CLOSE          0x21


//TYPE_G_ASK_INFO
#define FUNC_RETURN         0x05
#define FUNC_DIP            0x10
#define FUNC_KEYINFO        0x30
// #define FUNC_POWER          0x1c
#define LIGHT_INTENSITY		0x2b

//TYPE_G_CONFIGURE
#define FUNC_MAPPING        0x03
#define FUNC_RELAY          0x04
#define FUNC_SCENE_KEY      0x40
#define FUNC_SCENE_ITEM     0x41
#define FUNC_DELAY          0x42
#define FUNC_OCENABLE       0x43

//TYPE_T_TEST
#define FUNC_CLEARFLASH_KEYSTATUS   0x01
#define FUNC_CLEARFLASH_SCENEALL    0x02


void  APP_TASK_HEAD(void* pdata);

void  APP_TASK_MAIN(void* pdata);

//用户任务声明
void	APP_STARTUP_TASK(void* pdata);
void APP_TASK_ADC(void* pdata);


void delayKey(uint8_t times,void* pdata);

#endif
