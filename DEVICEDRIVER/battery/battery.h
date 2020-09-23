#ifndef __BATTERY_H
#define __BATTERY_H

#include "sys.h"


#define ADC_CH_NUM		2
#define ADC_POINT_NUM	64

#define BATTERY_VOLTAGE_SCALE	1.0f
#define BATTERY_CURRENT_SCALE	1.0f

#define BATTERY_CHARGING		1
#define BATTERY_DISCHARGING		0

#define BATTERY_LOW_BATTERY		9.5f




typedef struct	BatteryManagement
{
	u8 state;							//���״̬
	float charge_voltage;				//����ѹ
	float charge_current;				//�ŵ����
	float discharge_voltage;			//�ŵ��ѹ
	float discharge_current;			//������
	u8 dod;								//�ŵ���ȣ��ڵ��ʹ�ù����У���طų����������ض��������İٷֱ�
	u8 soc;								//�ɵ�״̬�����ʣ������������İٷֱ�
	u8 soh;								//��ؽ���״̬��(�������������ʡ������)����شӳ���״̬����һ���ı��ʷŵ絽��ֹ��ѹ���ų���������������Ӧ�ı�������ı�ֵ
	
}BatteryManagement_S;







extern BatteryManagement_S BatteryManagement;



void ADC1_DMA_Init(void);
u16 ADCGetAverageVal(u8 ch);
float GetCurrentVal(void);
float GetVoltageVal(void);
void BetteryGetParas(void);



























#endif
