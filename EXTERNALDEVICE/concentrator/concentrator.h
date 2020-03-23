#ifndef __CONCENTRATOR_H
#define __CONCENTRATOR_H

#include "sys.h"
#include "server_protocol.h"






typedef struct	ConcentratorBasicConfig		//�������������ò���
{
	u8 conncetion_mode;						//���ӷ�ʽ
	u8 server_ip[31];						//��������ַ
	u8 server_port[6];						//�������˿�
	u16 heartbeat_cycle;					//�������� s
	u16 electric_energy_collection_cycle;	//���ܲɼ����� min
	u8 electric_energy_recording_time;		//���ܼ�¼ʱ�� day
	u16 loop_state_monitoring_cycle;		//��·״̬������� min
	u8 loop_state_recording_time;			//��·״̬��¼ʱ�� day
	u16 cupboard_alarm_collection_cycle;	//��ظ澯�ɼ����� min
	u8 cupboard_alarm_recording_time;		//��ظ澯��¼ʱ�� day
	u16 lamp_state_collection_cycle;		//�ƾ�״̬�ɼ����� min
	s16 lamp_state_collection_offset;		//�ƾ�״̬�ɼ�ƫ���� min
	u8 lamp_state_recording_time;			//�ƾ�״̬��¼ʱ�� day
	u16 command_response_timeout;			//ָ����Ӧ�ȴ�ʱ�� sec
	u8 command_retransmission_times;		//ָ�ʱ�ط�����
	u8 heartbeat_retransmission_times;		//������ʱ�ط�����
	u16 lamp_response_timeout;				//�ƾ���Ӧ�ȴ�ʱ�� sec
	u8 lamp_retransmission_times;			//�ƾ߳�ʱ�ط�����
	u8 lamp_broadcast_times;				//�ƾ߹㲥���ʹ���
	u16 lamp_broadcast_interval_time;		//�ƾ߹㲥���ʱ�� ms
	u8 operation_password[7];				//�豸����ά������
	u8 manufacturer_name[33];				//��������
	u8 manufacturer_website[33];			//������ַ
	u16 crc16;								//У���� �洢��

}__attribute__((packed))ConcentratorBasicConfig_S;







extern ConcentratorBasicConfig_S ConcentratorBasicConfig;
extern u8 SystemReBoot;										//ϵͳ������ʶ












void ReadConcentratorBasicConfig(void);
void WriteConcentratorBasicConfig(u8 reset,u8 write_enable);
void RecvAndHandleFrameStruct(void);


u8 TransparentTransmission(ServerFrameStruct_S *server_frame_struct);
u8 SynchronizeTime(ServerFrameStruct_S *server_frame_struct);
u8 ResetConfigParameters(ServerFrameStruct_S *server_frame_struct);
u8 RebootTheSystem(ServerFrameStruct_S *server_frame_struct);








#endif
