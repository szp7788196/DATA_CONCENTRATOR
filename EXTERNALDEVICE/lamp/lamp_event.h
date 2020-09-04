#ifndef __LAMP_EVENT_H
#define __LAMP_EVENT_H

#include "sys.h"
#include "lamp_conf.h"
#include "concentrator_conf.h"
#include "task_plc.h"
#include "history_record.h"


#define LAMP_NODE_LOSS_ALARM					1		//�ڵ㶪ʧ�澯
#define LAMP_LAMP_FAULT_ALARM					2		//�ƾ߹��ϸ澯
#define LAMP_POWER_MODULE_FAULT_ALARM			3		//��Դģ����ϸ澯
#define LAMP_CAPACITOR_FAULT_ALARM				4		//�������ݹ��ϸ澯
#define LMAP_RELAY_FAULT_ALARM					5		//�̵������ϸ澯
#define LAMP_OVRE_LOW_TEMPERATURE_ALARM			6		//�ߵ��¸澯
#define LAMP_LEAKAGE_ALARM						7		//©��澯
#define LAMP_GATE_MAGNETISM_ALARM				8		//�ŴŸ澯
#define LAMP_POLE_TILT_ALARM					9		//�Ƹ���б�澯
#define LAMP_ELECTRICAL_PARA_OVER_THRE_ALARM	10		//�����Խ�޸澯
#define LAMP_ABNORMAL_LIGHT_ON_ALARM			11		//�쳣���Ƹ澯
#define LAMP_ABNORMAL_LIGHT_OFF_ALARM			12		//�쳣�صƸ澯
#define LAMP_LIGHT_ON_ABNORMAL_ALARM			13		//�����쳣�澯
#define LAMP_TASK_STATE_ABNORMAL_ALARM			14		//����������״̬�쳣�澯

typedef struct	LampParas
{
	u8 brightness;
	u16 light_up_day;
	u32 light_up_total;
	u32 active_energy_day;				//��λ��Wh
	u32 active_energy_total;
	u32 reactive_energy_day;
	u32 reactive_energy_total;
	u16 voltage;						//��λ��V
	u16 current;						//��λ��mA
	u16 active_power;					//��λ��W
	u16 power_factor;					//��λ��0.01
	u16 frequency;						//��λ��0.01Hz

}__attribute__((packed))LampParas_S;


typedef struct	LampState
{
	u8 report_type;								//�ϱ����� 0���� 1����
	u32 address;								//�豸��ַ
	u8 channel;									//��ͷͨ��
	
	LampParas_S lamp_paras[MAX_LAMP_CH_NUM];	//��ͷ����
	
	RUN_MODE_E run_mode;						//���Կ�ģʽ
	u8 controller;								//������Դ
	u8 control_time[3];							//����ʱ��
	s8 temperature;								//�¶�
	u16 tilt_angle;								//��б�Ƕ�
	u8 dev_time[6];								//�豸��ǰʱ��
	
}__attribute__((packed))LampState_S;

















void LampEventCheckPolling(void);


void LampLamp_Power_CapacitorFaultAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampRelayFaultAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampOverLowTemperatureAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampLeakageAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampGateMagnetismAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampPoleTiltAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampElectricalParaOverThreAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampAbnormalLightOnOffAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampLightOnAbnormalAlarm(PlcFrame_S *user_frame,u8 record_type);
void LampTaskStateAbnormalAlarm(PlcFrame_S *user_frame,u8 record_type);











































#endif
