#include "task_store.h"
#include "delay.h"
#include "history_record.h"
#include "common.h"
#include "fattester.h"




TaskHandle_t xHandleTaskSTORE = NULL;
unsigned portBASE_TYPE SatckSTORE;

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

u16 file_cnt = 0;
//接收并存储告警历史记录
void RecvAndStoreAlarmReport(void)
{
	AlarmReport_S *alarm_report = NULL;
	BaseType_t xResult;

	xResult = xQueueReceive(xQueue_AlarmReportStore,(void *)&alarm_report,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS)
	{
		StoreAlarmToSpiFlash(alarm_report);

		mf_scan_files("1:CONCEN/ALARM");
		
		file_cnt ++;

		DeleteAlarmReport(alarm_report);
	}
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






































