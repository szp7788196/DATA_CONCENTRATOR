#ifndef __ELECTRICITY_METER_EVENT_H
#define __ELECTRICITY_METER_EVENT_H

#include "sys.h"


#define ELECTRICITY_METER_PARAS_THRE_OVER	1		//电参数越限告警




typedef struct	MultipleCnt1
{
	u8 v;
	u8 c;
	u8 pf;

}__attribute__((packed))MultipleCnt1_S;

typedef struct	MultipleCnt2
{
	s8 cnt[8];

}__attribute__((packed))MultipleCnt2_S;




void ElectricityMeterEventCheckPolling(void);

void ElectricityMeterDetectParasStateChanges(void);

void ElectricityMeterCombineParasThreOverAlarm(u8 i,u8 j,u8 m,double reference_value);
void ElectricityMeterDetectParasThreOver(void);
void ElectricityMeterDetectParasThreUnder(void);






























#endif
