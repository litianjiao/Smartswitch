#ifndef  __Flash_H
#define  __Flash_H

#define FLASHADDRESSHIGHBASE                         (0x08000000)  

#define FLASHADDRESSLOWBASE                          (0x0000c000) 

#define FLASHPAGESIZE                                 0x00000400

#define FLASHMAXINDEX                                 0x0000fc00

#define FLASHTOPADDRESS                              (0x08010000)

void flashInit(void *persist);

void Flash(void* persist);

uint8_t erasePage(const uint32_t page_addr);

uint8_t write_itn_flash_data(uint32_t *data, uint16_t length, const uint32_t page_addr);

void read_itn_flash_data(uint32_t *Write_buf, uint16_t length, const uint32_t Write_Addr);

#endif
