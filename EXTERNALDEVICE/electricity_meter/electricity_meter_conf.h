#ifndef __ELECTRICITY_METER_CONF_H
#define __ELECTRICITY_METER_CONF_H

#include "sys.h"
#include "common.h"


#define MAX_ELECTRICITY_METER_CONF_NUM					6	//�������6�����
#define MAX_ELECTRICITY_METER_MODULE_NAME_LEN			16	//ģ���ͺų���
#define MAX_ELECTRICITY_METER_CH_NUM					7	//ͨ����
#define MAX_ELECTRICITY_METER_ALARM_TYPE_NUM			1	//�澯������
#define MAX_ELECTRICITY_METER_PER_CH_PARA_NUM			8	//ÿ��ͨ����������
#define MAX_ELECTRICITY_METER_ALARM_PARA_NUM			8	//�����ø澯�Ĳ�������


typedef struct	ElectricityMeter_Para										//����ͨ���澯����
{
	float voltage;										//���ѹ					
	float current;										//�����
	float active_power;									//�й�����
	double active_energy;								//�й�����
	double reactive_energy;								//�޹�����
	float power_factor;									//��������
	float frequency;									//Ƶ��
	float line_voltage;									//���ѹ

}__attribute__((packed))ElectricityMeter_Para_S;

typedef struct	ElectricityMeter_Alarm		//����ͨ���澯����
{
	u8 channel;								//������
	u8 para_id;								//�澯��ƽ
	float min_value;						//��Сֵ
	u8 min_range;							//��Сֵ���Ʋ�����Χ�ٷֱ�
	float max_value;						//���ֵ
	u8 max_range;							//���ֵ���Ʋ�����Χ�ٷֱ�
	u8 confirm_time;						//ȷ�ϴ���
	u8 last_run_mode;						//�ϸ�����״̬
	u8 switch_run_mode;						//���Կ��л�
	u8 resume_run_mode;						//���Կػָ�
	u8 relay_action;						//��·����

}__attribute__((packed))ElectricityMeter_Alarm_S;

typedef struct	ElectricityMeterBasicConfig			//����������
{
	u16 detect_interval;							//�����
	u8 auto_report;									//�Զ��ϱ�

	u16 crc16;										//У���� �洢��

}__attribute__((packed))ElectricityMeterBasicConfig_S;

typedef struct	ElectricityMeterConfig					//�������
{
	u8 address;											//ͨѶ��ַ
	u8 channel;											//ͨѶͨ��
	u8 ch_num;											//����ͨ����
	float voltage_ratio;								//��ѹ����
	float current_ratio;								//��������
	float voltage_range_of_change;						//��ѹ�仯��Χ
	float current_range_of_change;						//�����仯��Χ
	float pf_range_of_change;							//���������仯��Χ
	u8 confirm_time;									//ȷ�ϴ���
	u8 module[MAX_ELECTRICITY_METER_MODULE_NAME_LEN];	//ģ���ͺ�
	ElectricityMeter_Alarm_S alarm_thre[MAX_ELECTRICITY_METER_CH_NUM][MAX_ELECTRICITY_METER_ALARM_PARA_NUM];//�������澯��ֵ

	u16 crc16;											//У���� �洢��

}__attribute__((packed))ElectricityMeterConfig_S;

typedef struct	ElectricityMeterAlarmConfig				//���澯��������
{
	u8 electrical_parameters_thre_over_alarm_enable;	//�����Խ�߸澯

	u16 crc16;											//У���� �洢��

}__attribute__((packed))ElectricityMeterAlarmConfig_S;

typedef struct	ElectricityMeterState					//���ʵʱ�ɼ�����
{
	u8 address;											//ͨѶ��ַ
	u8 channel;											//ͨѶͨ��
	
	u8 ch_num;											//����ͨ����
	
	u8 abnormal_ch;										//�����쳣�Ļ�·
	
	ElectricityMeter_Para_S mirror_para[MAX_ELECTRICITY_METER_CH_NUM];
	ElectricityMeter_Para_S current_para[MAX_ELECTRICITY_METER_CH_NUM];

}__attribute__((packed))ElectricityMeterState_S;

typedef struct	ElectricityMeterCollectState			//���ʵʱ�ɼ�����
{
	u8 address;											//ͨѶ��ַ
	u8 channel;											//ͨѶͨ��
	
	u8 update;

	ElectricityMeter_Para_S collect_para[MAX_ELECTRICITY_METER_CH_NUM];

}__attribute__((packed))ElectricityMeterCollectState_S;




extern ElectricityMeterBasicConfig_S ElectricityMeterBasicConfig;
extern Uint32TypeNumber_S ElectricityMeterConfigNum;
extern ElectricityMeterConfig_S ElectricityMeterConfig[MAX_ELECTRICITY_METER_CONF_NUM];
extern ElectricityMeterAlarmConfig_S ElectricityMeterAlarmConfig;

extern ElectricityMeterState_S ElectricityMeterState[MAX_ELECTRICITY_METER_CONF_NUM];








void ReadElectricityMeterBasicConfig(void);
void WriteElectricityMeterBasicConfig(u8 reset,u8 write_enable);
void ReadElectricityMeterConfigNum(void);
void WriteElectricityMeterConfigNum(u8 reset,u8 write_enable);
void ReadElectricityMeterConfig(void);
void WriteElectricityMeterConfig(u8 i,u8 reset,u8 write_enable);
void ReadElectricityMeterAlarmConfig(void);
void WriteElectricityMeterAlarmConfig(u8 reset,u8 write_enable);































#endif
