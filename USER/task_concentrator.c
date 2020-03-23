#include "task_concentrator.h"
#include "delay.h"
#include <time.h>
#include "common.h"
#include "concentrator.h"
#include "server_protocol.h"
#include "task_handle_server_frame.h"



TaskHandle_t xHandleTaskCONCENTRATOR = NULL;



void vTaskCONCENTRATOR(void *pvParameters)
{
	while(1)
	{
		RecvAndHandleFrameStruct();			//±¨ÎÄ½âÎö
		
		delay_ms(50);
	}
}

























































