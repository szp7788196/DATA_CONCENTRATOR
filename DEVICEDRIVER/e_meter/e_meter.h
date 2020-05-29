#ifndef __E_METER_H
#define __E_METER_H

#include "sys.h"
#include "electricity_meter_conf.h"









void HT7038GetParameters(void);
void ElectricityMeterGetBuiltInParas(ElectricityMeterState_S *state);
void ElectricityMeterGetBuiltOutParas(ElectricityMeterState_S state);
u16 PackBuiltOutElectricityMeterFrame(u8 address,u8 mode,u8 *outbuf);
void AnalysisBuiltOutElectricityMeterFrame(u8 *buf,u16 len,ElectricityMeterCollectState_S *meter_state);



































#endif
