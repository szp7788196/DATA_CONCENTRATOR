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
//	time_t time_s = 0;
//	
//	time_s = GetSysTick1s();
	
	while(1)
	{
		RecvAndHandleFrameStruct();			//报文解析
		
		if(FlagSystemReBoot == 1)			//接收到重启的命令
		{
			FlagSystemReBoot = 0;
			delay_ms(5000);

			__disable_fault_irq();			//重启指令
			NVIC_SystemReset();
		}
		
		if(FlagReConnectToServer == 1)		//接收到重新连接服务器指令
		{
			delay_ms(5000);

			FlagReConnectToServer = 2;
		}
		
		delay_ms(50);
	}
}

























































