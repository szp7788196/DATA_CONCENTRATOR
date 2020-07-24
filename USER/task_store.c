#include "task_store.h"
#include "delay.h"
#include "history_record.h"
#include "common.h"
#include "fattester.h"
#include "exfuns.h"




TaskHandle_t xHandleTaskSTORE = NULL;
unsigned portBASE_TYPE SatckSTORE;
u32 total_size = 0;
u32 free_size = 0;

void vTaskSTORE(void *pvParameters)
{
	while(1)
	{
		RecvAndStoreAlarmReport();
		RecvAndSendEventHistoryToServer();

		delay_ms(500);
		
		SatckSTORE = uxTaskGetStackHighWaterMark(NULL);
	}
}

//接收并存储告警历史记录
void RecvAndStoreAlarmReport(void)
{
	AlarmReport_S *alarm_report = NULL;
	BaseType_t xResult;

	xResult = xQueueReceive(xQueue_AlarmReportStore,(void *)&alarm_report,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS)
	{
		StoreAlarmToSpiFlash(alarm_report);

		mf_scan_files("1:");

		DeleteAlarmReport(alarm_report);
	}
	
	exf_getfree("1:",&total_size,&free_size);
}

void RecvAndSendEventHistoryToServer(void)
{
	EventHistory_S *event_history = NULL;
	BaseType_t xResult;

	xResult = xQueueReceive(xQueue_HistoryRecordRead,(void *)&event_history,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS)
	{
		GetAlarmEventContentFromDateSegmentAndSendToServer(*event_history);

		DeleteEventHistory(event_history);
	}
}






































