#include "task_electricity_meter.h"
#include "delay.h"
#include "ht7038.h"



TaskHandle_t xHandleTaskELECTRICITY_METER = NULL;

void vTaskELECTRICITY_METER(void *pvParameters)
{
	HT7038ConfigClibration();
	
	while(1)
	{
		HT7038GetParameters();
		
		delay_ms(1000);
	}
}







































