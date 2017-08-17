#ifndef _ONENET_H_
#define _ONENET_H_







//strcut onenet function
typedef struct
{
	
	void (*IO_Init)(void);

	_Bool (*Init)(void); //返回0代表成功
	
	void (*Reset)(void);
	
	_Bool (*ReLink)(void); //返回0成功
	
	_Bool (*Send_Cmd)(char *cmd, char *res); //返回0成功
	
	void (*Send_Data)(unsigned char *data, unsigned short len); //数据发送的函数指针
	
	unsigned char *(*Get_Data)(unsigned short timeOut); //不同网络设备返回格式可能不同，这里自己实现
	
	void (*Clr_Data)(void);
	
	unsigned char (*Device_Check)(void); //设备检查，返回状态
	
	void (*ReConfig)(unsigned char step); //重置状态
	
	void (*Set_DataMode)(unsigned char mode); //设置网络模块数据接收的类型
	
	unsigned char (*Get_DataMode)(void);

} DEVICE_FUN;

typedef struct
{

    char devID[15];
    
    char apiKey[35];
	
	unsigned char netWork : 1; //0-局网模式(AP)		1-互联网模式(STA)
	unsigned char sendData : 1;
	unsigned char errCount : 3; //错误计数
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


#define CHECK_CONNECTED			0	//已连接
#define CHECK_CLOSED			1	//已断开
#define CHECK_GOT_IP			2	//已获取到IP
#define CHECK_NO_DEVICE			3	//无设备
#define CHECK_INITIAL			4	//初始化状态
#define CHECK_NO_CARD			5	//没有sim卡
#define CHECK_NO_ERR			255 //

#define DEVICE_CMD_MODE			0 //AT命令模式
#define DEVICE_DATA_MODE		1 //平台命令下发模式
#define DEVICE_HEART_MODE		2 //心跳连接模式





_Bool OneNet_SendData(HTTP_TYPE  type, unsigned char len);

void OneNet_Status(void);

void OneNet_Event(unsigned char *dataPtr);


#endif
