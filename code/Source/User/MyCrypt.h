#ifndef  __MyCrypt_H
#define  __MyCrypt_H

#include "openssl/aes.h"

#include "stdint.h"

void bsp_crypt_init(uint8_t* KEY);

uint8_t bsp_encrypt(uint8_t* in,uint8_t len,uint8_t mode);

void bsp_decrypt(uint8_t* in,uint8_t len);

#endif
