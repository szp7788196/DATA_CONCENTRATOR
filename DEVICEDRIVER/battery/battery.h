#ifndef __BATTERY_H
#define __BATTERY_H

#include "sys.h"


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































#endif
