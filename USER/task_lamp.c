#include "task_lamp.h"
#include "delay.h"
#include "common.h"
#include "lamp_comm.h"





TaskHandle_t xHandleTaskLAMP = NULL;
unsigned portBASE_TYPE SatckLAMP;

void vTaskLAMP(void *pvParameters)
{
	while(1)
	{
		
		
		LampRecvLampStateAndSendToServer();					//���͵�������״̬��

		delay_ms(100);
		
		SatckLAMP = uxTaskGetStackHighWaterMark(NULL);
	}
}




















































