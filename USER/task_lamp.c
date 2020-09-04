#include "task_lamp.h"
#include "delay.h"
#include "common.h"
#include "lamp_comm.h"
#include "rx8010s.h"






TaskHandle_t xHandleTaskLAMP = NULL;
unsigned portBASE_TYPE SatckLAMP;

void vTaskLAMP(void *pvParameters)
{
	while(1)
	{
		LampRecvLampStateAndSendToServer();					//发送单灯运行状态包
		
		LampRecvAndHandleFrameStruct();						//接收并解析报文
		
		LampAutoSynchronizeTime();							//集控自动为单灯周期性对时
		
		LampAutoGetCurrentState();							//集控自动周期性采集灯具状态

		delay_ms(100);
		
		SatckLAMP = uxTaskGetStackHighWaterMark(NULL);
	}
}


void LampAutoSynchronizeTime(void)
{
	static time_t time = DEFAULT_TIME_SEC;
	LampPlcExecuteTask_S *task = NULL;
	u8 *data = NULL;
	
	if(GetSysTick1s() - time >= LampBasicConfig.auto_sync_time_cycle)
	{
		time = GetSysTick1s();
		
		task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));
		data = (u8 *)pvPortMalloc(6 * sizeof(u8));
		
		if(task != NULL && data != NULL)
		{
			memset(task,0,sizeof(LampPlcExecuteTask_S));
			
			task->broadcast_type = 0;
			task->execute_type = 0;
			task->cmd_code = 0x0101;
			task->dev_num = LampNumList.number;
			task->execute_total_num = 1;
			task->data = data;
			task->data_len = 6;
			
			if(LampBasicConfig.auto_report_plc_state & (1 << 0))
			{
				task->notify_enable = 1;
			}
			
			*(data + 0) = calendar.w_year - 2000;
			*(data + 1) = calendar.w_month;
			*(data + 2) = calendar.w_date;
			*(data + 3) = calendar.hour;
			*(data + 4) = calendar.min;
			*(data + 5) = calendar.sec;
			
			if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
				DeleteLampPlcExecuteTask(task);
			}			
		}
		else
		{
			if(task != NULL)
			{
				vPortFree(task);
			}
			
			if(data != NULL)
			{
				vPortFree(data);
			}
		}
	}
}

void LampAutoGetCurrentState(void)
{
	static time_t time = DEFAULT_TIME_SEC;
	LampPlcExecuteTask_S *task = NULL;
	
	if(GetSysTick1s() - time >= (LampBasicConfig.state_collection_cycle * 60))
	{
		time = GetSysTick1s();
		
		task = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));
		
		if(task != NULL)
		{
			memset(task,0,sizeof(LampPlcExecuteTask_S));
			
			task->broadcast_type = 0;
			task->execute_type = 2;
			task->cmd_code = 0x0170;
			task->dev_num = LampNumList.number;

			if(LampBasicConfig.auto_report_plc_state & (1 << 2))
			{
				task->notify_enable = 1;
			}
			
			if(xQueueSend(xQueue_LampPlcExecuteTaskToPlc,(void *)&task,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_LampPlcFrame fail.\r\n");
#endif
				DeleteLampPlcExecuteTask(task);
			}			
		}
	}
}
















































