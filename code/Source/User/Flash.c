#include "includes.h"

void flashInit(void *persist){
}

void Flash(void* persist){
}

uint8_t erasePage(const uint32_t page_addr){
	
	FLASH_Unlock();
	
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	while(1) {
		
		if (FLASH_ErasePage(page_addr) == FLASH_COMPLETE) break;
		else{
				FLASH_Lock();
				return 0;
		}
		
	}	
	
	FLASH_Lock();
	
	return 1;
}

/*
*********************************************************************************************************
* Description: flash 读写
*
* Arguments  : 
*							 
*
* Returns    : 
*********************************************************************************************************
*/
uint8_t write_itn_flash_data(uint32_t *data, uint16_t length, const uint32_t page_addr) {
	uint16_t i;
	uint32_t addr = page_addr;
	
	FLASH_Unlock();
	
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	for (i=0; i<length; i++) {
		while(1) {
			if (FLASH_ProgramWord(addr, data[i]) == FLASH_COMPLETE) {
				if (*(uint32_t*)addr == data[i])
					break;	
			}else{
				FLASH_Lock();
				return 0;
			}
		}

		addr += 4; //write word formated of 32bit.
	} 

	FLASH_Lock();
	
	return 1;
	
}


void read_itn_flash_data(uint32_t *Write_buf, uint16_t length, const uint32_t Write_Addr)
{
	uint16_t i;
	uint32_t addr = Write_Addr;
	
	for (i=0; i<length; i++) {
		
		Write_buf[i] = (*(uint32_t *)addr);

		addr += 4; 
	}

}
