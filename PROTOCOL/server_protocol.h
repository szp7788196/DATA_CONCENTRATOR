#ifndef __SERVER_PROTOCOL_H
#define __SERVER_PROTOCOL_H

#include "sys.h"
#include "rtos_task.h"


#define NET_RX_FRAME_MAX_LEN					4096
#define NET_TX_FRAME_MAX_LEN					1650

#define MAX_PARA_NUM							50


typedef enum
{
	MODE_INSIDE 	= 0,
	MODE_4G 		= 1,
	MODE_ETH 		= 2,
	MODE_NB_IOT 	= 3,
	MODE_WIFI 		= 4,

} CONNECTION_MODE_E;

typedef enum
{
	SERVER_REQUEST_DOWN 	= 0x01,		//服务器主动下行到设备的数据请求
	DEVICE_RESPONSE_UP 		= 0x02,		//设备上行到服务器的相应
	DEVICE_REQUEST_UP 		= 0x03,		//设备主动上行到服务器的数据请求
	SERVER_RESPONSE_DOWN 	= 0x04,		//服务器下行到设备的相应
	SYNC_RESPONSE 			= 0x05,		//命令异步处理结果以及返回数据

} MESSAGE_TYPE_E;

typedef enum
{
	UNKNOW_DEVICE			= 0xFFFF,	//未知设备
	CONCENTRATOR 			= 0x0000,	//集控设备
	LAMP_CONTROLLER 		= 0x0100,	//路灯控制器
	RELAY 					= 0x0200,	//继电器
	INPUT_COLLECTOR 		= 0x0300,	//输入量采集器
	ELECTRIC_METER 			= 0x0400,	//电表
	LUMETER					= 0x0500,	//照度计
	
} DEVICE_TYPE_E;

typedef enum
{
	NO_ERR 				= 0x00,			//成功
	FAIL 				= 0x01,			//失败
	CRC_ERR 			= 0x02,			//校验失败
	UNDEFINED_MSG_ID 	= 0x03,			//未定义的消息ID
	CAN_NOT_EXECUTE 	= 0x04,			//命令暂时不能执行
	PARA_NUMBER_ERR		= 0x05,			//参数个数错误
	PARA_FORMAT_ERR		= 0x06,			//参数格式错误
	PARA_RANGE_ERR		= 0x07,			//参数范围错误
	UNDEFINED_PARA_TYPE	= 0x08,			//未定义的参数类型
	UNKNOW_ERR			= 0xFF,			//位置错误
	
} ERROR_TYPE_E;








typedef struct	ServerFrame
{
	CONNECTION_MODE_E connection_mode;
	u16 len;
	
	u8 *buf;
}__attribute__((packed))ServerFrame_S;


typedef struct	Parameter					//参数
{
	u16 type;								//参数类型
	u16 len;								//参数长度
	u8 *value;								//参数值
}Parameter_S;								//切记不可强制地址对齐，不然接收消息队列时会死机


typedef struct	ServerFrameStruct			//帧结构
{
	CONNECTION_MODE_E connection_mode;
	
	u8 start;
	u8 msg_type;							//消息类型
	u32 serial_num;							//消息流水号
	u16 msg_len;							//消息长度
	u8 err_code;							//错误码
	u8 reseve[4];							//备用	 
	u32 crc32;								//检验和

	u16 msg_id;								//消息ID
	u32 gateway_id;							//网关ID
	u32 device_id;							//设备ID
	
	u8 para_num;							//参数个数
	
	Parameter_S *para;						//具体参数
	
	u8 stop;
}ServerFrameStruct_S;











void ServerFrameHandle(ServerFrame_S *rx_frame);
u8 TransServerFrameStructToOtherTask(ServerFrameStruct_S *server_frame_struct,DEVICE_TYPE_E device_type);
u8 GetParameterNum(ServerFrame_S *rx_frame);
u8 GetParameters(ServerFrameStruct_S *server_frame_struct,ServerFrame_S *rx_frame);
s8 GetServerFrameStruct(ServerFrameStruct_S *server_frame_struct,ServerFrame_S *rx_frame);
u8 ConvertFrameStructToFrame(ServerFrameStruct_S *server_frame_struct);
u8 CopyServerFrameStruct(ServerFrameStruct_S *s_server_frame_struct,ServerFrameStruct_S *d_server_frame_struct,u8 mode);
u16 GetFinalFrameLen(u8 *buf,u16 len);
u16 EscapeSymbolDelete(u8* inbuf,u16 inbuf_len,u8* outbuf);
u16 EscapeSymbolAdd(u8* inbuf,u16 inbuf_len,u8* outbuf);




















#endif
