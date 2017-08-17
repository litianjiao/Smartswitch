#ifndef  __MyQueue_H
#define  __MyQueue_H

#define MYCMDQUEUEBUFFSIZE 128

void     MyQueue(void* persist);

uint8_t* getAvailableElement(uint8_t len,void* persist);

void     unlockMyMalloc(void* persist);

int      pushElement(uint8_t* cmd,uint8_t len,void* persist);

uint8_t* pullElement(uint8_t* err,void* persist);

void     removeAllElement(void* persist);

#endif
