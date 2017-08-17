#include "includes.h"

/*
*****************************************************************************
* 目的：用于存储多条命令
* 相当于一个队列
*	数据结构：1字节偏移量+n字节数据。偏移量=
* 
*****************************************************************************
*/

OS_EVENT	*QUEUE_SEM;      //队列信号量，初值应为0

int locking=0;

//使用类似于队列的方式存储数据，缓存区目的：内存申请给出的地址由用户填充数据，必须连续
//不使用堆栈原因：移出数据时需要大量的复制操作
//uint8_t MEMORY[BUF_LEN+128];          //用于存放命令的缓存区，作用类似于队列，128表示单次申请最大字节数，避免越界，cmd长度最大94，base64转码也不超过128,128能被4整除，BUF_LEN必须是能被4整除数

int     cmdQueueHead=0,								 //头
				cmdQueueTail=0;								 //尾
																										 //头进尾出

uint8_t myQueueBuff[MYCMDQUEUEBUFFSIZE+128]={0};

uint8_t nowUsing=0;																	 //当前已使用字节数,避免当前使用的时间被覆盖


void MyQueue(void* persist){
	QUEUE_SEM=OSSemCreate(0);
}

/**
* 获取下一个可用内存空间，大小为len的内存区。如果不存在返回NULL。
* 如果希望该方法同步，persist={1}，即对该方法进行同步操作
* 如果persist={1}可能会造成死锁——方法不主动锁住该方法，如果使用者主动加锁而不进行解锁则会造成死锁，程序并不对该情况采取任何措施制止
*/
uint8_t* getAvailableElement(uint8_t len,void* persist){
	int tmp;
#if OS_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
	OS_CPU_SR  cpu_sr = 0u;
#endif
	//可能会造成死锁——方法不主动锁住该方法，如果使用者主动加锁而不进行解锁则会造成死锁，程序并不对该情况采取任何措施制止
	while(locking) OSTimeDlyHMSM(0, 0, 0, 100);
	if(persist!=NULL) locking=((int*)persist)[0];
	
	if(len<=0) return 0;
	//len+=1;                 																									 //1字节用于存放申请长度，对外透明
	len=4*(1+len/4);																														 //总是比需要申请长度多1-4个字节的长度进行申请
	
	OS_ENTER_CRITICAL();
																																							 //用于保证有一个字节存放偏移量，4的倍数是因为某些原因在类型转换（uint8_t->uint32_t）时给的起始地址必须是4的倍数，不然传指针会出现问题，比如原用加密算法传参
	if(cmdQueueHead<cmdQueueTail&&(cmdQueueHead+len+nowUsing)>=cmdQueueTail){					 //如果头指针在尾指针前，即可能内存不够分配，需要判断
		OS_EXIT_CRITICAL();																												 //检查是否有足够的内存空间可以申请，now_using表示上一个被拿走的字节长度，用于避免正在被使用的字节被覆盖
		return NULL;
	}																																						 
	if(cmdQueueTail==0&&(cmdQueueHead+len+nowUsing)>MYCMDQUEUEBUFFSIZE){														 //即使头指针在尾指针后，也存在一种特殊情况，选择边缘情况，避免数据被覆盖，即point_end=BUF_LEN=》point_end=0
		OS_EXIT_CRITICAL();
		return NULL;  							 
	}
	
	//空间足够开始分配
	tmp=cmdQueueHead;
	*(myQueueBuff+cmdQueueHead)=len;
	cmdQueueHead+=len;
	if(cmdQueueHead>=MYCMDQUEUEBUFFSIZE) cmdQueueHead=0;
	
	OS_EXIT_CRITICAL();
	
	OSSemPost(QUEUE_SEM);
	
	return myQueueBuff+tmp+1;
}

void     unlockMyMalloc(void* persist){
	locking=0;
}

/*
*********************************************************************************************************
* Description: 内存申请,主要作用是用于接收缓存，以4为单位分配（由于某些方法的需要，比如crc冗余）
*
* Arguments  : len： 要申请的字节数
*
* Returns    : 成功返回可用字节数组首地址,失败返回NULL
*********************************************************************************************************
*/
int pushElement(uint8_t* cmd,uint8_t len,void* persist){
	int i;
	uint8_t* tmp=NULL;
	uint8_t* ext=NULL;
	uint8_t  def[1]={0};

	if(persist!=NULL) ext=(uint8_t*)persist;
	else ext=def;
	tmp=getAvailableElement(len+ext[0],NULL);

	if(tmp==NULL) return 0;
	
	for(i=0;i<ext[0];i++){
		tmp[i]=ext[i+1];
	}
	
	len+=ext[0];
	
	for(;i<len;i++){
		tmp[i]=cmd[i-ext[0]];
	}
	
	return 1;
}

/*
*********************************************************************************************************
* Description: 内存释放
*
* Arguments  :
*
* Returns    : 被释放的字节数组首地址
*********************************************************************************************************
*/
uint8_t* pullElement(uint8_t* err,void* persist){
	int tmp;
#if OS_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
	OS_CPU_SR  cpu_sr = 0u;
#endif
	
	OSSemPend(QUEUE_SEM,0,err);
	
	if((*err)!=OS_ERR_NONE) return NULL;
	
	OS_ENTER_CRITICAL();
	
	if(cmdQueueTail==cmdQueueHead){															//头尾相同，表示无数据，由分配方式可知，分配不会造成重叠
		OS_EXIT_CRITICAL();
		return NULL;									
	}
	
	tmp=cmdQueueTail+*(myQueueBuff+cmdQueueTail);										//获取释放之后尾指针位置（预判）
	
	if(cmdQueueTail<cmdQueueHead&&tmp>cmdQueueHead){              //这种情况说明内存分配已混乱，获取的数据已不正确，重置指针
		OS_EXIT_CRITICAL();
		removeAllElement(NULL);
		return NULL;
	}
	
	tmp=cmdQueueTail;
	nowUsing=*(myQueueBuff+cmdQueueTail);
	cmdQueueTail+=nowUsing;
	if(cmdQueueTail>=MYCMDQUEUEBUFFSIZE) cmdQueueTail=0;
	
	OS_EXIT_CRITICAL();
	
	return myQueueBuff+tmp+1;
}

/*
*********************************************************************************************************
* Description: 使自定义内存返回初始状态
*
* Arguments  :
*
* Returns    : 
*********************************************************************************************************
*/
void     removeAllElement(void* persist){
#if OS_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
	OS_CPU_SR  cpu_sr = 0u;
#endif
	OS_ENTER_CRITICAL();
	cmdQueueHead=cmdQueueTail=nowUsing=0;
	while(OSSemAccept(QUEUE_SEM)>0) continue;
	OS_EXIT_CRITICAL();
}
