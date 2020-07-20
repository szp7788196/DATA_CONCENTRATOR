#ifndef __LAMP_EVENT_H
#define __LAMP_EVENT_H

#include "sys.h"
#include "lamp_conf.h"
#include "concentrator_conf.h"

typedef struct	LampParas
{
	u8 brightness;
	u16 light_up_day;
	u16 light_up_total;
	float active_energy_day;
	float active_energy_total;
	float reactive_energy_day;
	float reactive_energy_total;
	float voltage;
	float current;
	float active_power;
	float power_factor;
	float frequency;

}__attribute__((packed))LampParas_S;


typedef struct	LampState
{
	u16 address;								//�豸��ַ
	u8 lamp_num;								//��ͷ��
	
	LampParas_S lamp_paras[MAX_LAMP_CH_NUM];	//��ͷ����
	
	RUN_MODE_E run_mode;						//���Կ�ģʽ
	u8 controller;								//������Դ
	u8 control_time[7];							//����ʱ��
	s8 temperature;								//�¶�
	u16 tilt_angle;								//��б�Ƕ�
	u8 dev_time[15];							//�豸��ǰʱ��
	
}__attribute__((packed))LampState_S;

















void LampEventCheckPolling(void);






















#endif
