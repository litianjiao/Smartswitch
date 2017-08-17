#ifndef  __MyCRC_H
#define  __MyCRC_H

#define CRC_CHECK_ERR_NONE     0;

#define CRC_CHECK_ERR_ERROR    1;

#define CRC_FILL_ERR_NONE      0;

#define CRC_FILL_ERR_ERROR     1;

void    CRCInit(void* persist);

void    MyCRC(void* persist);

int 		CRC32_Check(uint8_t *temp,uint8_t len);

int 		CRC32_Fill(uint8_t *temp,uint8_t len);

uint32_t CRC32_Create(uint8_t *temp,uint8_t len);

#endif
