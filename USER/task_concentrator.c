#include "task_concentrator.h"
#include "delay.h"
#include <time.h>
#include "common.h"
#include "concentrator_comm.h"
#include "server_protocol.h"
#include "task_handle_server_frame.h"
#include "ff.h"
#include "exfuns.h"
#include "fattester.h"
#include "history_record.h"
#include "rx8010s.h"
#include "concentrator_event.h"



TaskHandle_t xHandleTaskCONCENTRATOR = NULL;
unsigned portBASE_TYPE SatckCONCENTRATOR;

void vTaskCONCENTRATOR(void *pvParameters)
{
	while(1)
	{
		ConcentratorRecvAndHandleFrameStruct();			//报文解析

		AutoSendFrameToServer();						//发送登录/心跳/告警灯信息

		ConcentratorEventCheckPolling();				//告警等事件轮训

		if(FlagSystemReBoot == 1)						//接收到重启的命令
		{
			FlagSystemReBoot = 0;
			delay_ms(5000);

			__disable_fault_irq();						//重启指令
			NVIC_SystemReset();
		}

		if(FlagReConnectToServer == 1)					//接收到重新连接服务器指令
		{
			delay_ms(5000);

			FlagReConnectToServer = 2;
			LoginResponse = 0;
		}

		if(FrameWareState.state == FIRMWARE_DOWNLOADED)	//固件下载完成,重启系统
		{
			delay_ms(5000);

			__disable_fault_irq();						//重启指令
			NVIC_SystemReset();
		}

		delay_ms(100);

		SatckCONCENTRATOR = uxTaskGetStackHighWaterMark(NULL);
	}
}

























































