#ifndef __BATTERY_H
#define __BATTERY_H

#include "sys.h"


typedef struct	BatteryManagement
{
	u8 state;							//电池状态
	float charge_voltage;				//充电电压
	float charge_current;				//放电电流
	float discharge_voltage;			//放电电压
	float discharge_current;			//充电电流
	u8 dod;								//放电深度，在电池使用过程中，电池放出的容量与电池而定容量的百分比
	u8 soc;								//荷电状态，电池剩余电量与额定容量的百分比
	u8 soh;								//电池健康状态，(包括容量、功率、内阻等)，电池从充满状态下以一定的倍率放电到截止电压所放出的容量与其所对应的标称容量的比值
	
}BatteryManagement_S;







extern BatteryManagement_S BatteryManagement;































#endif
