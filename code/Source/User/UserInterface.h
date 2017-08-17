#ifndef  __USERINTERFACE_H
#define  __USERINTERFACE_H

#include "includes.h"
#include "Zigbee.h"
#define	CMD_NO_ERR	 0x00
#define	CMD_ERR			 0x01

#define FLASHADDRESSOFKEYSTATUS                   FLASHADDRESSHIGHBASE+FLASHADDRESSLOWBASE

#define FLASHADDRESSOFSCENE                       FLASHADDRESSHIGHBASE+FLASHADDRESSLOWBASE+FLASHPAGESIZE

#define FLASHFORCOPING                            FLASHADDRESSHIGHBASE+FLASHMAXINDEX


void UserInterface(zigbee_struct* zigbee,void* persist);

/*
*********************************************************
* 自定义命令
*/

typedef struct{
	uint16_t	cmdID;
	uint16_t	from;
	uint16_t	to;
	uint8_t		type;
	uint8_t		length;
	uint8_t	subtyp;
	uint8_t	route;
	uint8_t	value[80];
} cmd_line;

uint8_t* getCmdStream(cmd_line source,uint8_t* len);

cmd_line getDefaultCmd(void);

cmd_line getCmd(cmd_line copy);

/*
*********************************************************
* 输入
*/

int postData(uint8_t type,uint8_t* data,uint8_t dataLen);

cmd_line getNextCmd(uint8_t* err,uint8_t** data,INT32U timeout);

/*
*********************************************************
* 输出
*/             
                                                       
uint8_t sendCmd(uint8_t headType,cmd_line cmd,void *persist);  

/*
*********************************************************
* flash
*/  

uint8_t checkWritable(uint32_t addr,void* persist);

uint8_t flashCopyToOtherPage(uint32_t fromAddr,uint32_t toAddr,uint16_t len,void* persist);


uint8_t writekeyStatus(uint8_t status[6][12],void* persist);

uint8_t readKeyStatus(uint8_t status[6][12],void* persist);

uint8_t findNextAvaliablePage(uint32_t* addr,void* persist);

uint8_t writeSceneHead(uint8_t *sceneId,uint32_t* addr,void* persist);

uint8_t readSceneHead(uint8_t *sceneId,uint32_t addr,void* persist);

uint8_t addSceneItem(uint8_t *item,uint32_t addrOfHead,void* persist);

uint8_t deleteSceneItem(uint8_t *item,uint32_t addrOfHead,void* persist);

uint8_t alertSceneItem(uint8_t *item,uint32_t addrOfHead,void* persist);

uint32_t readSceneItem(uint8_t *item,uint32_t addr,void* persist);

uint8_t  deleteScene(uint8_t *sceneId,uint32_t addrOfHead,void* persist);

#endif
