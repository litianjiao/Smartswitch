#include "includes.h"

/*
*****************************************************************************
* Ŀ�ģ����ڴ洢��������
* �൱��һ������
*	���ݽṹ��1�ֽ�ƫ����+n�ֽ����ݡ�ƫ����=
* 
*****************************************************************************
*/

OS_EVENT	*QUEUE_SEM;      //�����ź�������ֵӦΪ0

int locking=0;

//ʹ�������ڶ��еķ�ʽ�洢���ݣ�������Ŀ�ģ��ڴ���������ĵ�ַ���û�������ݣ���������
//��ʹ�ö�ջԭ���Ƴ�����ʱ��Ҫ�����ĸ��Ʋ���
//uint8_t MEMORY[BUF_LEN+128];          //���ڴ������Ļ����������������ڶ��У�128��ʾ������������ֽ���������Խ�磬cmd�������94��base64ת��Ҳ������128,128�ܱ�4������BUF_LEN�������ܱ�4������

int     cmdQueueHead=0,								 //ͷ
				cmdQueueTail=0;								 //β
																										 //ͷ��β��

uint8_t myQueueBuff[MYCMDQUEUEBUFFSIZE+128]={0};

uint8_t nowUsing=0;																	 //��ǰ��ʹ���ֽ���,���⵱ǰʹ�õ�ʱ�䱻����


void MyQueue(void* persist){
	QUEUE_SEM=OSSemCreate(0);
}

/**
* ��ȡ��һ�������ڴ�ռ䣬��СΪlen���ڴ�������������ڷ���NULL��
* ���ϣ���÷���ͬ����persist={1}�����Ը÷�������ͬ������
* ���persist={1}���ܻ������������������������ס�÷��������ʹ�������������������н������������������򲢲��Ը������ȡ�κδ�ʩ��ֹ
*/
uint8_t* getAvailableElement(uint8_t len,void* persist){
	int tmp;
#if OS_CRITICAL_METHOD == 3u                          /* Allocate storage for CPU status register      */
	OS_CPU_SR  cpu_sr = 0u;
#endif
	//���ܻ������������������������ס�÷��������ʹ�������������������н������������������򲢲��Ը������ȡ�κδ�ʩ��ֹ
	while(locking) OSTimeDlyHMSM(0, 0, 0, 100);
	if(persist!=NULL) locking=((int*)persist)[0];
	
	if(len<=0) return 0;
	//len+=1;                 																									 //1�ֽ����ڴ�����볤�ȣ�����͸��
	len=4*(1+len/4);																														 //���Ǳ���Ҫ���볤�ȶ�1-4���ֽڵĳ��Ƚ�������
	
	OS_ENTER_CRITICAL();
																																							 //���ڱ�֤��һ���ֽڴ��ƫ������4�ı�������ΪĳЩԭ��������ת����uint8_t->uint32_t��ʱ������ʼ��ַ������4�ı�������Ȼ��ָ���������⣬����ԭ�ü����㷨����
	if(cmdQueueHead<cmdQueueTail&&(cmdQueueHead+len+nowUsing)>=cmdQueueTail){					 //���ͷָ����βָ��ǰ���������ڴ治�����䣬��Ҫ�ж�
		OS_EXIT_CRITICAL();																												 //����Ƿ����㹻���ڴ�ռ�������룬now_using��ʾ��һ�������ߵ��ֽڳ��ȣ����ڱ������ڱ�ʹ�õ��ֽڱ�����
		return NULL;
	}																																						 
	if(cmdQueueTail==0&&(cmdQueueHead+len+nowUsing)>MYCMDQUEUEBUFFSIZE){														 //��ʹͷָ����βָ���Ҳ����һ�����������ѡ���Ե������������ݱ����ǣ���point_end=BUF_LEN=��point_end=0
		OS_EXIT_CRITICAL();
		return NULL;  							 
	}
	
	//�ռ��㹻��ʼ����
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
* Description: �ڴ�����,��Ҫ���������ڽ��ջ��棬��4Ϊ��λ���䣨����ĳЩ��������Ҫ������crc���ࣩ
*
* Arguments  : len�� Ҫ������ֽ���
*
* Returns    : �ɹ����ؿ����ֽ������׵�ַ,ʧ�ܷ���NULL
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
* Description: �ڴ��ͷ�
*
* Arguments  :
*
* Returns    : ���ͷŵ��ֽ������׵�ַ
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
	
	if(cmdQueueTail==cmdQueueHead){															//ͷβ��ͬ����ʾ�����ݣ��ɷ��䷽ʽ��֪�����䲻������ص�
		OS_EXIT_CRITICAL();
		return NULL;									
	}
	
	tmp=cmdQueueTail+*(myQueueBuff+cmdQueueTail);										//��ȡ�ͷ�֮��βָ��λ�ã�Ԥ�У�
	
	if(cmdQueueTail<cmdQueueHead&&tmp>cmdQueueHead){              //�������˵���ڴ�����ѻ��ң���ȡ�������Ѳ���ȷ������ָ��
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
* Description: ʹ�Զ����ڴ淵�س�ʼ״̬
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
