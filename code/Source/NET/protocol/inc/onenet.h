#ifndef _ONENET_H_
#define _ONENET_H_







//strcut onenet function
typedef struct
{
	
	void (*IO_Init)(void);

	_Bool (*Init)(void); //����0����ɹ�
	
	void (*Reset)(void);
	
	_Bool (*ReLink)(void); //����0�ɹ�
	
	_Bool (*Send_Cmd)(char *cmd, char *res); //����0�ɹ�
	
	void (*Send_Data)(unsigned char *data, unsigned short len); //���ݷ��͵ĺ���ָ��
	
	unsigned char *(*Get_Data)(unsigned short timeOut); //��ͬ�����豸���ظ�ʽ���ܲ�ͬ�������Լ�ʵ��
	
	void (*Clr_Data)(void);
	
	unsigned char (*Device_Check)(void); //�豸��飬����״̬
	
	void (*ReConfig)(unsigned char step); //����״̬
	
	void (*Set_DataMode)(unsigned char mode); //��������ģ�����ݽ��յ�����
	
	unsigned char (*Get_DataMode)(void);

} DEVICE_FUN;

typedef struct
{

    char devID[15];
    
    char apiKey[35];
	
	unsigned char netWork : 1; //0-����ģʽ(AP)		1-������ģʽ(STA)
	unsigned char sendData : 1;
	unsigned char errCount : 3; //�������
	unsigned char reverse : 3;

} ONETNET_INFO;

extern ONETNET_INFO oneNetInfo;


typedef enum
{

	TYPE_BOOL = 0,
	
	TYPE_CHAR,
	TYPE_UCHAR,
	
	TYPE_SHORT,
	TYPE_USHORT,
	
	TYPE_INT,
	TYPE_UINT,
	
	TYPE_LONG,
	TYPE_ULONG,
	
	TYPE_FLOAT,
	TYPE_DOUBLE,
	
	TYPE_GPS,

} DATA_TYPE;

typedef struct
{

	char *name;
	void *data;
	DATA_TYPE dataType;
	_Bool flag;

} DATA_STREAM;

#define SEND_BUF_SIZE  1024


typedef enum
{

    HTTP_TYPE1 = 1,
	HTTP_TYPE3 = 3,
	HTTP_TYPE4,
	HTTP_TYPE5

} HTTP_TYPE;


#define CHECK_CONNECTED			0	//������
#define CHECK_CLOSED			1	//�ѶϿ�
#define CHECK_GOT_IP			2	//�ѻ�ȡ��IP
#define CHECK_NO_DEVICE			3	//���豸
#define CHECK_INITIAL			4	//��ʼ��״̬
#define CHECK_NO_CARD			5	//û��sim��
#define CHECK_NO_ERR			255 //

#define DEVICE_CMD_MODE			0 //AT����ģʽ
#define DEVICE_DATA_MODE		1 //ƽ̨�����·�ģʽ
#define DEVICE_HEART_MODE		2 //��������ģʽ





_Bool OneNet_SendData(HTTP_TYPE  type, unsigned char len);

void OneNet_Status(void);

void OneNet_Event(unsigned char *dataPtr);


#endif
