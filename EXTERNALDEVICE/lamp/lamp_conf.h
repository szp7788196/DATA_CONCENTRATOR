#ifndef __LAMP_CONF_H
#define __LAMP_CONF_H

#include "sys.h"
#include "common.h"
#include "concentrator_conf.h"

/********************************�̼�����***************************************/
//#define LAMP_FIRMWARE_FREE					0
//#define LAMP_FIRMWARE_DOWNLOADING			1
//#define LAMP_FIRMWARE_DOWNLOAD_WAIT			2
//#define LAMP_FIRMWARE_DOWNLOADED			3
//#define LAMP_FIRMWARE_DOWNLOAD_FAILED		4
//#define LAMP_FIRMWARE_UPDATING				5
//#define LAMP_FIRMWARE_UPDATE_SUCCESS		6
//#define LAMP_FIRMWARE_UPDATE_FAILED			7
//#define LAMP_FIRMWARE_ERASE_SUCCESS			8
//#define LAMP_FIRMWARE_ERASE_FAIL			9
//#define LAMP_FIRMWARE_ERASEING				10
#define LAMP_FIRMWARE_BAG_SIZE				258
#define LAMP_FIRMWARE_BUCKUP_FLASH_BASE_ADD	0x08020000	//����5
#define LAMP_FIRMWARE_SIZE					131072		//128K
#define LAMP_FIRMWARE_BAG_SEND_SIZE			128


#define MAX_LAMP_AUTO_SYNC_TIME_CYCLE		604800	//�����Զ�ͬ��ʱ������ ���һ��

#define MAX_LAMP_CONF_NUM					512		//�������õĵ�������
#define MAX_LAMP_CH_NUM						2		//���ͨ����
#define MAX_LAMP_GROUP_NUM					10		//�������
#define MAX_LAMP_ALARM_E_PARA_NUM			3		//�����ø澯�ĵ��������

#define MAX_LAMP_STRATEGY_NUM				512
#define MAX_LAMP_APPOINTMENT_NUM			10
#define MAX_LAMP_APPOINTMENT_TIME_RANGE_NUM	10		//ÿ��ԤԼ�������10��ʱ���



typedef struct	LampListNum							//����ͳ�Ƶ������ø���
{
	u16 number;										//����
	
	u8 list[64];									//���ձ� ÿһ��bit��ʾһ������
	
	u16 crc16;										//У���� �洢��
	
}__attribute__((packed))LampListNum_S;

typedef struct	LampGroupListNum					//����ͳ��ÿ�鵥�����ø���
{
	u16 list[MAX_LAMP_GROUP_NUM];					//���ÿ�鵥������
	
	u16 crc16;										//У���� �洢��
	
}__attribute__((packed))LampGroupListNum_S;

typedef struct	ElectriccalParaAlarm
{
	u8 channel;									//��ͷͨ��
	u8 para_id;									//����ID
	u16 min_value;								//��Сֵ
	u8 min_range;								//��Сֵ�澯���Ʋ�����Χ�ٷֱ�
	u16 max_value;								//���ֵ
	u8 max_range;								//���ֵ�澯���Ʋ�����Χ�ٷֱ�
	u16 duration_time;							//����ȷ��ʱ��
}__attribute__((packed))ElectriccalParaAlarm_S;

typedef struct	LampAlarmConf
{	
	u8 lamp_fault_alarm_enable;						//�ƾ߹��ϸ澯ʹ��
	u8 power_module_fault_alarm_enable;				//��Դ���ϸ澯ʹ��
	u8 capacitor_fault_alarm_enable;				//�������ݹ��ϸ澯ʹ��
	u8 relay_fault_alarm_enable;					//�̵������ϸ澯ʹ��
	
	u8 temperature_alarm_enable;					//�ߵ��¸澯ʹ��
	s8 temperature_alarm_low_thre;					//�ߵ��¸澯������ֵ
	u8 temperature_alarm_high_thre;					//�ߵ��¸澯������ֵ
	u16 temperature_alarm_duration;					//�ߵ��¸澯���ʱ��
	
	u8 leakage_alarm_enable;						//©��澯ʹ��
	u16 leakage_alarm_c_thre;						//©��澯������ֵ
	u8 leakage_alarm_v_thre;						//©��澯��ѹ��ֵ
	u16 leakage_alarm_duration;						//©��澯���ʱ��
	
	u8 gate_magnetism_alarm_enable;					//�ŴŸ澯ʹ��
	u8 gate_magnetism_alarm_type;					//�ŴŸ澯�������
	
	u8 post_tilt_alarm_enable;						//�Ƹ���б�澯ʹ��
	s8 post_tilt_alarm_thre;						//�Ƹ���б�澯��ֵ
	u16 post_tilt_alarm_duration;					//�Ƹ���б�澯���ʱ��
	
	u8 electrical_para_alarm_enable;				//�����Խ�޸澯ʹ��
	ElectriccalParaAlarm_S electrical_para_alarm_thre[MAX_LAMP_CH_NUM][MAX_LAMP_ALARM_E_PARA_NUM];
	
	u8 abnormal_light_on_alarm_enable;				//�쳣���Ƹ澯ʹ��
	u16 abnormal_light_on_alarm_c_thre;				//�쳣���Ƹ澯������ֵ
	u16 abnormal_light_on_alarm_p_thre;				//�쳣���Ƹ澯������ֵ
	u16 abnormal_light_on_alarm_duration;			//�쳣���Ƹ澯���ʱ��
	
	u8 abnormal_light_off_alarm_enable;				//�쳣�صƸ澯ʹ��
	u16 abnormal_light_off_alarm_c_thre;			//�쳣�صƸ澯������ֵ
	u16 abnormal_light_off_alarm_p_thre;			//�쳣�صƸ澯������ֵ
	u16 abnormal_light_off_alarm_duration;			//�쳣�صƸ澯���ʱ��
	
	u8 light_on_fault_alarm_enable;							//�����쳣�澯ʹ��
	u8 light_on_fault_alarm_rated_power[MAX_LAMP_CH_NUM];	//�ƾ�1�����
	u8 light_on_fault_alarm_low_thre[MAX_LAMP_CH_NUM];		//�ƾ�1��С���ʳ��ްٷֱ�
	u8 light_on_fault_alarm_high_thre[MAX_LAMP_CH_NUM];		//�ƾ�1��С���ʳ��ްٷֱ�
	u16 light_on_fault_alarm_duration[MAX_LAMP_CH_NUM];		//�����쳣�澯���ʱ��
	
	u8 task_light_state_fault_alarm_enhable;		//��������״̬�쳣�澯ʹ��
	
	u16 crc16;												//У���� �洢��

}__attribute__((packed))LampAlarmConf_S;

typedef struct	LampBasicConfig
{
	u32 auto_sync_time_cycle;								//�Զ���ʱ���� ��
	u8 auto_report_plc_state;								//�Զ��ϱ�PLC״̬
	u16 state_collection_cycle;								//�ƾ�״̬�ɼ����� min
	s16 state_collection_offset;							//�ƾ�״̬�ɼ�ƫ���� min
	u8 state_recording_time;								//�ƾ�״̬��¼ʱ�� day
	u16 response_timeout;									//�ƾ���Ӧ�ȴ�ʱ�� sec
	u8 retransmission_times;								//�ƾ߳�ʱ�ط�����
	u8 broadcast_times;										//�ƾ߹㲥���ʹ���
	u16 broadcast_interval_time;							//�ƾ߹㲥���ʱ�� ms

	u16 crc16;												//У���� �洢��

}__attribute__((packed))LampBasicConfig_S;

typedef struct	LampConfig
{
	u32 address;											//�豸��ַ
	s8 advance_time;										//�����ǰ���Ʒ�����
	s8 delay_time;											//����Ӻ����Ʒ�����
	float longitude;										//����
	float latitude;											//γ��
	u8 light_wane;											//��˥
	u8 auto_report;											//�����Զ��ϱ�ʹ��
	u8 adjust_type;											//���ⷽʽ
	u8 node_loss_check_times;								//�ڵ㶪ʧ������
	u8 default_brightness;									//��ͨ��ʼ������ ��4�ֽڴ���1��ͷ ��4�ֽڱ�ʾ��ͷ2
	u8 group[MAX_LAMP_GROUP_NUM];							//�豸�������

	u16 crc16;												//У���� �洢��

}__attribute__((packed))LampConfig_S;

typedef struct	LampTask									//������������
{
	u8 group_id;											//ģʽ(��)���
	u8 type;												//��������
	u8 executor;											//ִ����
	s16 time;												//����ʱ��
	u8 time_option;											//ʱ��ѡ��
	u8 brightness[MAX_LAMP_CH_NUM];							//��ͷ����
	u8 ctrl_mode;											//����ģʽ 0�������� 1����ָ���� 2����ָ����ַ
	u32 group_add[MAX_LAMP_GROUP_NUM];						//��Ż��ַ
	
	u16 crc16;												//У���� �洢��
	
}__attribute__((packed))LampTask_S;

typedef struct	LampSenceConfig									//���Ƴ���ģʽ����
{
	u8 group_id;												//ģʽ(��)���
	u8 priority;												//���ȼ�
	u8 time_range_num;											//��Чʱ�䷶Χ����
	TimeRange_S range[MAX_LAMP_APPOINTMENT_TIME_RANGE_NUM];		//����ʱ���
	
	u16 crc16;													//У���� �洢��

}__attribute__((packed))LampSenceConfig_S;

typedef struct	LampStrategyGroupSwitch							//ģʽ�л�����
{
	u8 group_num;												//����������
	u8 group_id[MAX_LAMP_GROUP_NUM];							//�������
	u8 type;													//�л���ʽ
	u8 time[15];												//�л�ʱ��
	
	u16 crc16;													//У���� �洢��

}__attribute__((packed))LampStrategyGroupSwitch_S;




extern LampBasicConfig_S LampBasicConfig;				//�����Զ���ʱ����

extern LampGroupListNum_S LampGroupListNum;						//ÿ�鵥������
extern LampListNum_S LampStrategyNumList;						//�����õ���������
extern LampListNum_S LampNumList;								//�����õĵ�������
extern Uint32TypeNumber_S LampAppointmentNum;					//����ԤԼ��������
extern Uint32TypeNumber_S LampStrategyNum;						//���Ʋ�����������
extern FrameWareState_S LampFrameWareState;						//�̼�����״̬




void ReadLampBasicConfig(void);
void WriteLampBasicConfig(u8 reset,u8 write_enable);
void ReadLampNumList(void);
void WriteLampNumList(u8 reset,u8 write_enable);
u8 ReadSpecifyLampNumList(u16 i);
void WriteSpecifyLampNumList(u16 i,u8 mode);
void ReadLampStrategyNumList(void);
void WriteLampStrategyNumList(u8 reset,u8 write_enable);
u8 ReadSpecifyLampStrategyNumList(u16 i);
void WriteSpecifyLampStrategyNumList(u16 i,u8 mode);
void ReadLampAppointmentNum(void);
void WriteLampAppointmentNum(u8 reset,u8 write_enable);
u8 ReadLampAppointment(u8 i,LampSenceConfig_S *appointment);
void WriteLampAppointment(u8 i,LampSenceConfig_S *appointment,u8 reset,u8 write_enable);
void WriteSpecifyLampNumList(u16 i,u8 mode);
void ReadLampGroupListNum(void);
void WriteLampGroupListNum(u8 reset,u8 write_enable);
u8 ReadLampConfig(u16 i,LampConfig_S *config);
void WriteLampConfig(u8 i,u8 reset,LampConfig_S config);
u8 ReadLampTaskConfig(u16 i,LampTask_S *task);
void WriteLampTaskConfig(u8 i,u8 reset,LampTask_S task);
u8 ReadLampFrameWareState(void);
void WriteLampFrameWareState(u8 reset,u8 write_enable);












#endif
