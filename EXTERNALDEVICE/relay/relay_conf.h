#ifndef __RELAY_CONF_H
#define __RELAY_CONG_H

#include "sys.h"
#include "common.h"


#define MAX_RELAY_MODULE_CONF_NUM					5	//�������5���̵���ģ��
#define MAX_RELAY_MODULE_STRATEGY_GROUP_NUM			10	//���10������ģʽ(��)
#define MAX_RELAY_MODULE_APPOINTMENT_NUM			10	//���10��ԤԼ����
#define MAX_RELAY_MODULE_STRATEGY_NUM				300	//���300����
#define MAX_RELAY_MODULE_GROUP_STRATEGY_NUM			30	//ÿ��ģʽ(��)���30������
#define MAX_RELAY_MODULE_APPOINTMENT_TIME_RANGE_NUM	10	//ÿ��ԤԼ�������10��ʱ���

#define MAX_RELAY_MODULE_MODULE_NAME_LEN			16
#define MAX_RELAY_MODULE_LOOP_CH_NUM				12
#define MAX_RELAY_MODULE_LOOP_GROUP_NUM				16
#define MAX_RELAY_MODULE_ALARM_TYPE_NUM				8

#define MAX_RELAY_MODULES_LOOP_CH_NUM				MAX_RELAY_MODULE_LOOP_CH_NUM * MAX_RELAY_MODULE_CONF_NUM	//ϵͳ�����õĻ�·�������ֵ



typedef enum
{
	TYPE_FIXED_TIME			= 1,			//��ʱ
	TYPE_FIXED_TIME_LIGHT 	= 2,			//��ʱ���
	TYPE_LOCATION 			= 3,			//��γ��
	TYPE_LOCATION_LIGHT 	= 4,			//��γ�ȹ��
	TYPE_CHRONOLOGY			= 5,			//���
	TYPE_CHRONOLOGY_LIGHT	= 6,			//�����

} STRATEGY_TYPE_E;

typedef struct	RelayModuleConfig													//�̵���ģ������
{
	u8 address;																		//ͨѶ��ַ
	u8 channel;																		//ͨѶͨ��
	u8 loop_num;																	//��·����
	u16 interval_time;																//���������ʱ
	u8 module[MAX_RELAY_MODULE_MODULE_NAME_LEN];									//ģ���ͺ�
	u16 loop_enable;																//��·����״̬
	u8 loop_group[MAX_RELAY_MODULE_LOOP_CH_NUM][MAX_RELAY_MODULE_LOOP_GROUP_NUM];	//��·���
	u8 loop_alarm_thre[MAX_RELAY_MODULE_LOOP_CH_NUM][5];							//��·�����쳣��ֵ

	u16 crc16;																		//У���� �洢��

}__attribute__((packed))RelayModuleConfig_S;

typedef struct	RelayModuleState													//�̵���ģ��״̬
{
	u8 address;																		//ͨѶ��ַ
	u8 channel;																		//ͨѶͨ��
	
	u16 interval_time;																//���������ʱ
	
	u8 execute_immediately;															//����ִ��
	
	u16 loop_channel_bit;															//ģ��ӵ�л�·״̬
	u16 loop_last_channel;															//��·�ϸ�����ͨ��
	u16 loop_current_channel;														//��·��ǰ����ͨ��
	u16 loop_last_task_channel;														//��·�ϸ��������ͨ��
	u16 loop_task_channel;															//��·��ǰ�������ͨ��
	
	u16 loop_mirror_state;															//�ԱȻ�·״̬
	u16 loop_last_state;															//��·�ϸ�״̬
	u16 loop_current_state;															//��·��ǰ״̬
	u16 loop_collect_state;															//��·�ɼ�״̬
	u16 loop_last_task_state;														//��·�ϸ�����״̬
	u16 loop_task_state;															//��·��ǰ����״̬
	
	u8 controller;																	//������Դ
	u8 control_time[15];															//����ʱ��
	
	u16 abnormal_loop;																//�쳣��·
	u8 loop_alarm[MAX_RELAY_MODULE_LOOP_CH_NUM][MAX_RELAY_MODULE_ALARM_TYPE_NUM];	//��·�澯

}__attribute__((packed))RelayModuleState_S;

typedef struct	RelayModuleCollectState												//�̵���ģ��ɼ�״̬
{
	u8 address;																		//ͨѶ��ַ
	u8 channel;																		//ͨѶͨ��
	u8 update;																		//���±�־
	u16 loop_collect_state;															//��·�ɼ�״̬

}__attribute__((packed))RelayModuleCollectState_S;

typedef struct	RelayModuleBasicConfig		//�̵���ģ���������
{
	u8 on_hour;								//����ʱ
	u8 on_minute;							//���Ʒ�
	u8 off_hour;							//�ص�ʱ
	u8 off_minute;							//�صƷ�
	
	u16 state_monitoring_cycle;				//��·״̬������� min
	u8 state_recording_time;				//��·״̬��¼ʱ�� day
	
	u16 crc16;								//У���� �洢��
	
}__attribute__((packed))RelayModuleBasicConfig_S;

typedef struct	RelayAlarmConfig			//�̵���ģ��澯���ò���
{
	u8 relay_abnormal_alarm_enable;			//�̵����쳣ʹ��
	u8 contact_abnormal_alarm_enable;		//�����쳣ʹ��
	u8 task_abnormal_alarm_enable;			//������״̬�쳣ʹ��

	u16 crc16;								//У���� �洢��

}__attribute__((packed))RelayAlarmConfig_S;

typedef struct	RelayTaskAction				//�̵���������
{
	u8 module_address;						//ģ���ַ
	u8 module_channel;						//ģ���·
	u16 loop_channel;						//��·ͨ��
	u16 loop_action;						//��·����

}__attribute__((packed))RelayTaskAction_S;

typedef struct	RelayTask									//�̵�����������
{
	u8 group_id;											//ģʽ(��)���
	u8 type;												//��������
	s16 time;												//����ʱ��
	u8 time_option;											//ʱ��ѡ��
	u8 action_num;											//��������
	
	RelayTaskAction_S action[MAX_RELAY_MODULE_CONF_NUM];	//������
	
	u16 crc16;												//У���� �洢��
	
}__attribute__((packed))RelayTask_S;

typedef struct	RelaySenceConfig									//�̵���ģ�鳡��ģʽ����
{
	u8 group_id;													//ģʽ(��)���
	u8 priority;													//���ȼ�
	u8 time_range_num;												//��Чʱ�䷶Χ����
	TimeRange_S range[MAX_RELAY_MODULE_APPOINTMENT_TIME_RANGE_NUM];	//������
	
	u16 crc16;														//У���� �洢��

}__attribute__((packed))RelaySenceConfig_S;

typedef struct	RelayStrategyGroupSwitch							//ģʽ�л�����
{
	u8 group_num;													//����������
	u8 group_id[MAX_RELAY_MODULE_STRATEGY_GROUP_NUM];				//�������
	u8 type;														//�л���ʽ
	u8 time[15];													//�л�ʱ��
	
	u16 crc16;														//У���� �洢��

}__attribute__((packed))RelayStrategyGroupSwitch_S;


#define RelayStrategyGroupID_Priority_S struct RelayStrategyGroupID_Priority
typedef struct RelayStrategyGroupID_Priority *RelayStrategyGroupID_Priority;
struct RelayStrategyGroupID_Priority								//������ź����ȼ�
{
	u8 group_id;													//ģʽ(��)���
	u8 priority;													//���ȼ�
	
}__attribute__((packed));

#define RelayAppointment_S struct RelayAppointment
typedef struct RelayAppointment *pRelayAppointment;
struct RelayAppointment												//��������(ԤԼ����)
{
	u8 group_id;													//ģʽ(��)���
	u8 priority;													//���ȼ�
	
	u8 time_range_num;												//��Чʱ�䷶Χ����
	TimeRange_S range[MAX_RELAY_MODULE_APPOINTMENT_TIME_RANGE_NUM];	//������

	pRelayAppointment prev;
	pRelayAppointment next;
}__attribute__((packed));

#define RelayStrategy_S struct RelayStrategy
typedef struct RelayStrategy *pRelayStrategy;
struct RelayStrategy										//ģʽ����(��������)
{
	u8 group_id;											//ģʽ(��)���
	u8 type;												//��������

	u16 action_time;										//����ʱ��
	
	s16 offset_min;											//ƫ��ʱ��
	u8 time_option;											//ʱ��ѡ��

	u8 action_num;											//��������
	RelayTaskAction_S action[MAX_RELAY_MODULE_CONF_NUM];	//������

	pRelayStrategy prev;
	pRelayStrategy next;
}__attribute__((packed));





extern u8 RelayForceSwitchOffAllRelays;											//ǿ�ƶϿ����м̵���
extern u8 RelayRefreshStrategyGroup;											//�̵�������ˢ�±�־

extern Uint32TypeNumber_S RelayModuleConfigNum;									//�̵���ģ����������
extern RelayModuleConfig_S RelayModuleConfig[MAX_RELAY_MODULE_CONF_NUM];		//�̵���ģ������
extern RelayModuleBasicConfig_S RelayModuleBasicConfig;							//Ĭ�Ͽ��ص�ʱ��
extern RelayAlarmConfig_S RelayAlarmConfig;										//�̵���ģ��澯��������
extern Uint32TypeNumber_S RelayAppointmentNum;									//�̵���ģ��ԤԼ��������
extern Uint32TypeNumber_S RelayStrategyNum;										//�̵���ģ�������������
extern RelayModuleState_S RelayModuleState[MAX_RELAY_MODULE_CONF_NUM];			//�̵���ģ�鵱ǰ״̬

extern pRelayAppointment RelayAppointmentGroup;									//ԤԼ������
extern pRelayStrategy RelayStrategyGroup[MAX_RELAY_MODULE_STRATEGY_GROUP_NUM];	//������
extern RelayStrategyGroupSwitch_S RelayStrategyGroupSwitch;						//������(ģʽ)�л�����
extern RelayStrategyGroupSwitch_S RelayStrategyGroupSwitchTemp;					//������(ģʽ)�л�����(��ʱ)
extern pRelayStrategy CurrentRelayStrategyGroup;								//��ǰ������(ģʽ)
extern pRelayStrategy CurrentRelayStrategyGroupTemp;							//��ǰ������(ģʽ)(��ʱ)






void ReadRelayModuleConfigNum(void);
void WriteRelayModuleConfigNum(u8 reset,u8 write_enable);
void ReadRelayModuleConfig(void);
void WriteRelayModuleConfig(u8 i,u8 reset,u8 write_enable);
void ReadRelayModuleBasicConfig(void);
void WriteRelayModuleBasicConfig(u8 reset,u8 write_enable);
void ReadRelayAlarmConfig(void);
void WriteRelayAlarmConfig(u8 reset,u8 write_enable);
void ReadRelayAppointmentNum(void);
void WriteRelayAppointmentNum(u8 reset,u8 write_enable);
u8 ReadRelayAppointment(u8 i,RelaySenceConfig_S *appointment);
void WriteRelayAppointment(u8 i,RelaySenceConfig_S *appointment,u8 reset,u8 write_enable);
void ReadRelayStrategyNum(void);
void WriteRelayStrategyNum(u8 reset,u8 write_enable);
u8 ReadRelayStrategy(u8 i,RelayTask_S *strategy);
void WriteRelayStrategy(u8 i,RelayTask_S *strategy,u8 reset,u8 write_enable);
void ReadRelayAppointmentGroup(void);
void ReadRelayStrategyGroups(void);
void ReadRelayStrategyGroupSwitch(void);
void WriteRelayStrategyGroupSwitch(u8 reset,u8 write_enable);

void RefreshRelayStrategyActionTime(pRelayStrategy strategy,u32 illuminance_value);
void RefreshRelayStrategyGroupActionTime(u8 group_id,u32 illuminance_value);
void RelayStrategyAdd(pRelayStrategy strategy);
void RelayStrategyGroupDelete(u8 group_id);
void RelayAllStrategyGroupDelete(void);
void RelayAppointmentGroupAdd(pRelayAppointment appointment);
void RelayAppointmentGroupDelete(void);
pRelayStrategy GetRelayStrategyGroupByGroupID(u8 group_id);
pRelayStrategy RefreshCurrentRelayStrategyGroup(void);




















#endif
