#ifndef __CONCENTRATOR_H
#define __CONCENTRATOR_H

#include "sys.h"
#include "server_protocol.h"
#include "event_alarm.h"

/********************************�̼�����***************************************/
#define FIRMWARE_FREE					0
#define FIRMWARE_DOWNLOADING			1
#define FIRMWARE_DOWNLOAD_WAIT			2
#define FIRMWARE_DOWNLOADED				3
#define FIRMWARE_DOWNLOAD_FAILED		4
#define FIRMWARE_UPDATING				5
#define FIRMWARE_UPDATE_SUCCESS			6
#define FIRMWARE_UPDATE_FAILED			7
#define FIRMWARE_ERASE_SUCCESS			8
#define FIRMWARE_ERASE_FAIL				9
#define FIRMWARE_ERASEING				10
#define FIRMWARE_BAG_SIZE				258
#define FIRMWARE_RUN_FLASH_BASE_ADD		0x08040000
#define FIRMWARE_BUCKUP_FLASH_BASE_ADD	0x080A0000
#define FIRMWARE_MAX_FLASH_ADD			0x08100000
#define FIRMWARE_SIZE					FIRMWARE_BUCKUP_FLASH_BASE_ADD - FIRMWARE_RUN_FLASH_BASE_ADD

/********************************�¼�����***************************************/
#define CONCENTRATOR_RESET				1		//��λ�¼�
#define CONCENTRATOR_REBOOT				2		//�����¼�

/********************************�澯����***************************************/
#define CONCENTRATOR_POWER_OFF			1		//�ϵ�澯
#define CONCENTRATOR_LEAKAGE			2		//©��澯
#define CONCENTRATOR_LOW_BATTERY		3		//��ص����͸澯
#define CONCENTRATOR_ABNORMAL_CHARGE	4		//�쳣���澯
#define CONCENTRATOR_ABNORMAL_COMM		5		//�쳣ͨѶ�澯

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
	u16 heartbeat_response_timeout;			//ָ����Ӧ�ȴ�ʱ�� sec
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

typedef struct	SwitchTime					//ÿ�տ��ص�ʱ��
{
	u8 on_hour;								//����ʱ
	u8 on_minute;							//���Ʒ�
	u8 off_hour;							//�ص�ʱ
	u8 off_minute;							//�صƷ�
}__attribute__((packed))SwitchTime_S;

typedef struct	SwitchTimeMonthTable		//ÿ�¿��ص�ʱ��
{
	SwitchTime_S switch_time[31];
}__attribute__((packed))SwitchTimeMonthTable_S;

typedef struct	ConcentratorLocationConfig	//��������γ���������
{
	double longitude;						//����
	double latitude;						//γ��
	
	SwitchTimeMonthTable_S switch_time_month_table[12];
	
	u16 crc16;								//У���� �洢��

}__attribute__((packed))ConcentratorLocationConfig_S;

typedef struct FrameWareState
{
	u8 state;								//����״̬
	u16 total_bags;							//�ܰ���
	u16 current_bag_cnt;					//��ǰ����
	u16 bag_size;							//����С
	u16 last_bag_size;						//ĩ����С
	u32 total_size;							//�̼��ܴ�С
	u8 file_name[31];						//�ļ�����
	u8 md5[33];								//�ļ�Ψһ����
	u16 crc16;								//У����
	
}__attribute__((packed))FrameWareState_S;




extern RUN_MODE_E RunMode;										//����ģʽ
extern ConcentratorBasicConfig_S ConcentratorBasicConfig;		//����������Ϣ
extern ConcentratorAlarmConfig_S ConcentratorAlarmConfig;		//�澯���ò���
extern ConcentratorLocationConfig_S ConcentratorLocationConfig;	//��γ���������
extern FrameWareState_S FrameWareState;							//�̼�����״̬



extern u8 FlagSystemReBoot;										//ϵͳ������ʶ
extern u8 FlagReConnectToServer;								//����������־
extern u8 LoginResponse;										//�������Ե�¼������Ӧ
extern u8 HeartBeatResponse;									//������������������Ӧ




void ReadRunMode(void);
void WriteRunMode(u8 reset,u8 write_enable);
void ReadConcentratorBasicConfig(void);
void WriteConcentratorBasicConfig(u8 reset,u8 write_enable);
void ReadConcentratorAlarmConfig(void);
void WriteConcentratorAlarmConfig(u8 reset,u8 write_enable);
void ReadConcentratorLocationConfig(void);
void WriteConcentratorLocationConfig(u8 reset,u8 write_enable);
u8 ReadFrameWareState(void);
void WriteFrameWareState(u8 reset,u8 write_enable);

void SendLoginFrameToServer(void);
void SendHeartBeatFrameToServer(void);
void SendOtaRequestFrameToServer(FrameWareState_S frame_ware_state);
void SendOtaCompleteFrameToServer(void);
void SendAlarmReportFrameToServer(AlarmReport_S *alarm_report);
void AutoSendFrameToServer(void);
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
u8 SetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 GetBasicConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 SetLocationConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 GetLocationConfiguration(ServerFrameStruct_S *server_frame_struct);
u8 RequestFrameWareUpDate(ServerFrameStruct_S *server_frame_struct);
u8 RecvFrameWareBag(ServerFrameStruct_S *server_frame_struct);
u8 FrameWareUpDateStop(ServerFrameStruct_S *server_frame_struct);

































#endif
