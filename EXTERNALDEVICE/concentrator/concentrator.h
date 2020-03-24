#ifndef __CONCENTRATOR_H
#define __CONCENTRATOR_H

#include "sys.h"
#include "server_protocol.h"




typedef enum
{
	MODE_AUTO 	= 1,
	MODE_MANUAL = 2,
	MODE_TASK 	= 3,
	MODE_STOP 	= 4,

} RUN_MODE_E;


typedef struct	ConcentratorBasicConfig		//�������������ò���
{
	u8 connection_mode;						//���ӷ�ʽ
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


typedef struct	ConcentratorAlarmConfig		//�������澯���ò���
{
	u8 power_off_alarm_enable;				//�ϵ�澯ʹ��
	u16 power_off_alarm_thre;				//�ϵ�澯����ֵ
	u8 electric_leakage_alarm_enable;		//©��澯ʹ��
	u16 electric_leakage_alarm_v_thre;		//©���ѹ��ֵ
	u16 electric_leakage_alarm_c_thre;		//©�������ֵ
	u8 low_battery_alarm_enable;			//��ص����͸澯ʹ��
	u8 low_battery_alarm_thre;				//��ص����͸澯��ֵ
	u8 abnormal_charge_alarm_enable;		//��س���쳣�澯
	u16 abnormal_charge_alarm_v_thre;		//��س���쳣��ѹ��ֵ
	u16 abnormal_charge_alarm_c_thre;		//��س���쳣������ֵ
	u16 crc16;								//У���� �洢��

}__attribute__((packed))ConcentratorAlarmConfig_S;






extern RUN_MODE_E RunMode;
extern ConcentratorBasicConfig_S ConcentratorBasicConfig;




extern u8 FlagSystemReBoot;										//ϵͳ������ʶ
extern u8 FlagReConnectToServer;								//����������־











void ReadRunMode(void);
void WriteRunMode(u8 reset,u8 write_enable);
void ReadConcentratorBasicConfig(void);
void WriteConcentratorBasicConfig(u8 reset,u8 write_enable);
void ReadConcentratorAlarmConfig(void);
void WriteConcentratorAlarmConfig(u8 reset,u8 write_enable);


void RecvAndHandleFrameStruct(void);


u8 TransparentTransmission(ServerFrameStruct_S *server_frame_struct);
u8 SynchronizeTime(ServerFrameStruct_S *server_frame_struct);
u8 ResetConfigParameters(ServerFrameStruct_S *server_frame_struct);
u8 RebootTheSystem(ServerFrameStruct_S *server_frame_struct);
u8 ReConnectToServer(ServerFrameStruct_S *server_frame_struct);
u8 SetRunMode(ServerFrameStruct_S *server_frame_struct);
u8 QueryState(ServerFrameStruct_S *server_frame_struct);
u8 SetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 GetAlarmConfiguration(ServerFrameStruct_S *server_frame_struct);

































#endif
