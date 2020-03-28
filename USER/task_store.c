#include "task_store.h"
#include "delay.h"
#include "event_alarm.h"
#include "common.h"




TaskHandle_t xHandleTaskSTORE = NULL;

void vTaskSTORE(void *pvParameters)
{
	while(1)
	{
		RecvAndStoreAlarmReport();
		
		delay_ms(500);
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
		
//		mf_scan_files("1:CONCEN/ALARM");
		
		DeleteAlarmReport(alarm_report);
	}
}








































