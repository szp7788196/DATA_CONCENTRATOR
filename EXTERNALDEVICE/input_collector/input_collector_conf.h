#ifndef __INPUT_COLLECTOR_CONF_H
#define __INPUT_COLLECTOR_CONF_H

#include "sys.h"
#include "common.h"

#define MAX_INPUT_COLLECTOR_CONF_NUM				10	//�������10���������ɼ�ģ��
#define MAX_INPUT_COLLECTOR_MODULE_NAME_LEN			16	//ģ���ͺų���
#define MAX_INPUT_COLLECTOR_D_LOOP_CH_NUM			8	//������ͨ����
#define MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM			8	//ģ����ͨ����
#define MAX_INPUT_COLLECTOR_ALARM_TYPE_NUM			8	//�澯������





typedef struct	InputCollectorD_Alarm										//����ͨ���澯����
{
	u8 channel;																//������
	u8 alarm_level;															//�澯��ƽ
	u8 confirm_time;														//ȷ�ϴ���
	u8 last_run_mode;														//�ϸ�����״̬
	u8 switch_run_mode;														//���Կ��л�
	u8 resume_run_mode;														//���Կػָ�
	u8 relay_action;														//��·����

}__attribute__((packed))InputCollectorD_Alarm_S;

typedef struct	InputCollectorA_Alarm										//ģ��ͨ���澯����
{
	u8 channel;																//������
	double min_value;														//��Сֵ
	double max_value;														//���ֵ
	u8 confirm_time;														//ȷ�ϴ���
	u8 last_run_mode;														//�ϸ�����״̬
	u8 switch_run_mode;														//���Կ��л�
	u8 resume_run_mode;														//���Կػָ�
	u8 relay_action;														//��·����

}__attribute__((packed))InputCollectorA_Alarm_S;

typedef struct	InputCollectorBasicConfig									//�������ɼ�ģ������
{
	u16 detect_interval;													//�����
	u8 auto_report;															//�Զ��ϱ�

	u16 crc16;																//У���� �洢��

}__attribute__((packed))InputCollectorBasicConfig_S;

typedef struct	InputCollectorConfig										//�������ɼ�ģ������
{
	u8 address;																//ͨѶ��ַ
	u8 channel;																//ͨѶͨ��
	u8 d_loop_num;															//��������·����
	u8 a_loop_num;															//ģ������·����
	float a_quantity_range;													//ģ�����仯����
	u8 confirm_time;														//ȷ�ϴ���
	u8 module[MAX_INPUT_COLLECTOR_MODULE_NAME_LEN];							//ģ���ͺ�
	InputCollectorD_Alarm_S d_alarm_thre[MAX_INPUT_COLLECTOR_D_LOOP_CH_NUM];//�������澯��ֵ
	InputCollectorA_Alarm_S a_alarm_thre[MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM];//�������澯��ֵ

	u16 crc16;																//У���� �洢��

}__attribute__((packed))InputCollectorConfig_S;

typedef struct	InputCollectorAlarmConfig	//�������ɼ�ģ��澯��������
{
	u8 d_quantity_abnormal_alarm_enable;	//�������쳣ʹ��
	u8 a_quantity_abnormal_alarm_enable;	//ģ�����쳣ʹ��

	u16 crc16;								//У���� �洢��

}__attribute__((packed))InputCollectorAlarmConfig_S;

typedef struct	InputCollectorState														//���������ģ��״̬
{
	u8 address;																			//ͨѶ��ַ
	u8 channel;																			//ͨѶͨ��
	
	u8 d_channel_bit;																	//ģ��ӵ��������ͨ��״̬
	u8 a_channel_bit;																	//ģ��ӵ��ģ����ͨ��״̬
	u8 d_mirror_state;																	//������ͨ���ϸ�״̬
	u8 d_current_state;																	//������ͨ����ǰ״̬
	double a_mirror_state[MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM];							//ģ����ͨ���ϸ�״̬
	double a_current_state[MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM];							//ģ����ͨ����ǰ״̬
	u16 d_abnormal_loop;																//�����쳣��·
	u16 a_abnormal_loop;																//ģ���쳣��·
	u8 d_alarm[MAX_INPUT_COLLECTOR_D_LOOP_CH_NUM][MAX_INPUT_COLLECTOR_ALARM_TYPE_NUM];	//���ֻ�·�澯
	u8 a_alarm[MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM][MAX_INPUT_COLLECTOR_ALARM_TYPE_NUM];	//ģ���·�澯

}__attribute__((packed))InputCollectorState_S;

typedef struct	InputCollectorCollectState											//�������ɼ�ģ��ɼ�״̬
{
	u8 address;																		//ͨѶ��ַ
	u8 channel;																		//ͨѶͨ��
	u8 update;																		//���±�־
	u16 d_collect_state;															//������״̬
	double a_collect_state[MAX_INPUT_COLLECTOR_A_LOOP_CH_NUM];						//ģ����״̬

}__attribute__((packed))InputCollectorCollectState_S;







extern InputCollectorBasicConfig_S InputCollectorBasicConfig;
extern Uint32TypeNumber_S InputCollectorConfigNum;
extern InputCollectorConfig_S InputCollectorConfig[MAX_INPUT_COLLECTOR_CONF_NUM];
extern InputCollectorAlarmConfig_S InputCollectorAlarmConfig;

extern InputCollectorState_S InputCollectorState[MAX_INPUT_COLLECTOR_CONF_NUM];










void ReadInputCollectorBasicConfig(void);
void WriteInputCollectorBasicConfig(u8 reset,u8 write_enable);
void ReadInputCollectorConfigNum(void);
void WriteInputCollectorConfigNum(u8 reset,u8 write_enable);
void ReadInputCollectorConfig(void);
void WriteInputCollectorConfig(u8 i,u8 reset,u8 write_enable);
void ReadInputCollectorAlarmConfig(void);
void WriteInputCollectorAlarmConfig(u8 reset,u8 write_enable);

























#endif
