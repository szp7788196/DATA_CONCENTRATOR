#include "task_sensor.h"
#include "delay.h"
#include "ht7038.h"



TaskHandle_t xHandleTaskSENSOR = NULL;

void vTaskSENSOR(void *pvParameters)
{
	HT7038ConfigClibration();
	
	while(1)
	{
		HT7038GetParameters();
		
		delay_ms(1000);
	}
}







































