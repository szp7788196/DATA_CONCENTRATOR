#ifndef __SERVER_PROTOCOL_H
#define __SERVER_PROTOCOL_H

#include "sys.h"
#include "rtos_task.h"
#include "concentrator.h"


#define NET_RX_FRAME_MAX_LEN					4096
#define NET_TX_FRAME_MAX_LEN					1650

#define MAX_PARA_NUM							50




typedef enum
{
	SERVER_REQUEST_DOWN 	= 0x01,		//�������������е��豸����������
	DEVICE_RESPONSE_UP 		= 0x02,		//�豸���е�����������Ӧ
	DEVICE_REQUEST_UP 		= 0x03,		//�豸�������е�����������������
	SERVER_RESPONSE_DOWN 	= 0x04,		//���������е��豸����Ӧ
	SYNC_RESPONSE 			= 0x05,		//�����첽�������Լ���������

} MESSAGE_TYPE_E;

typedef enum
{
	CONCENTRATOR 			= 0x0000,	//�����豸
	LAMP_CONTROLLER 		= 0x0100,	//·�ƿ�����
	RELAY 					= 0x0200,	//�̵���
	INPUT_COLLECTOR 		= 0x0300,	//�������ɼ���
	ELECTRIC_METER 			= 0x0400,	//���
	LUMETER					= 0x0500,	//�նȼ�
	
} DEVICE_TYPE_E;

typedef enum
{
	NO_ERR 				= 0x00,			//�ɹ�
	FAIL 				= 0x01,			//ʧ��
	CRC_ERR 			= 0x02,			//У��ʧ��
	UNDEFINED_MSG_ID 	= 0x03,			//δ�������ϢID
	CAN_NOT_EXECUTE 	= 0x04,			//������ʱ����ִ��
	PARA_NUMBER_ERR		= 0x05,			//������������
	PARA_FORMAT_ERR		= 0x06,			//������ʽ����
	PARA_RANGE_ERR		= 0x07,			//������Χ����
	UNDEFINED_PARA_TYPE	= 0x08,			//δ����Ĳ�������
	UNKNOW_ERR			= 0xFF,			//λ�ô���
	
} ERROR_TYPE_E;








typedef struct	ServerFrame
{
	CONNECTION_MODE_E connection_mode;
	u16 len;
	
	u8 *buf;
}__attribute__((packed))ServerFrame_S;


typedef struct	Parameter					//����
{
	u16 type;								//��������
	u8 *value;								//����ֵ
}__attribute__((packed))Parameter_S;


typedef struct	ServerFrameStruct			//֡�ṹ
{
	CONNECTION_MODE_E connection_mode;
	
	u8 start;
	u8 msg_type;							//��Ϣ����
	u32 serial_num;							//��Ϣ��ˮ��
	u16 msg_len;							//��Ϣ����
	u8 err_code;							//������
	u8 reseve[4];							//����	 
	u32 crc32;								//�����

	u16 msg_id;								//��ϢID
	u32 gateway_id;							//����ID
	u32 device_id;							//�豸ID
	
	u8 para_num;							//��������
	
	Parameter_S para[MAX_PARA_NUM];			//�������
	
	u8 stop;
}__attribute__((packed))ServerFrameStruct_S;











void ServerFrameHandle(ServerFrame_S *rx_frame);
u8 GetParameters(ServerFrameStruct_S *server_frame_struct,ServerFrame_S *rx_frame);
s8 GetServerFrameStruct(ServerFrameStruct_S *server_frame_struct,ServerFrame_S *rx_frame);
u8 CopyServerFrameStruct(ServerFrameStruct_S *s_server_frame_struct,ServerFrameStruct_S *d_server_frame_struct,u8 mode);
u16 EscapeSymbolDelete(u8* inbuf,u16 inbuf_len,u8* outbuf);
u16 EscapeSymbolAdd(u8* inbuf,u16 inbuf_len,u8* outbuf);




















#endif
