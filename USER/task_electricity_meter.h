#ifndef __ELECTRICITY_METER_H
#define __ELECTRICITY_METER_H

#include "sys.h"
#include "rtos_task.h"


extern TaskHandle_t xHandleTaskELECTRICITY_METER;

void vTaskELECTRICITY_METER(void *pvParameters);




void ElectricityMeterCollectCurrentState(void);

void ElectricityMeterCollectCurrentState(void);
void ElectricityMeterRecvAndHandleDeviceFrame(void);


































#endif
