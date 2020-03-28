#include "task_concentrator.h"
#include "delay.h"
#include <time.h>
#include "common.h"
#include "concentrator.h"
#include "server_protocol.h"
#include "task_handle_server_frame.h"
#include "ff.h"
#include "exfuns.h"
#include "fattester.h"
#include "event_alarm.h"
#include "rx8010s.h"



TaskHandle_t xHandleTaskCONCENTRATOR = NULL;

AlarmReport_S *AlarmReport = NULL;

void xxxxxx(void)
{
	AlarmReport_S *AlarmReport = NULL;
	AlarmReport_S *AlarmReportCpy = NULL;
	
	AlarmReport = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));
	AlarmReportCpy = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));
	
	if(AlarmReport != NULL)
	{
		AlarmReport->record_type = 1;
		AlarmReport->device_type = 0;
		AlarmReport->alarm_type = 1;
		
		AlarmReport->device_address = (u8 *)pvPortMalloc(11 * sizeof(u8));
		
		if(AlarmReport->device_address != NULL)
		{
			memset(AlarmReport->device_address,0,11);
			memset(AlarmReport->device_address,'d',10);
		}
		
		AlarmReport->device_channel = (u8 *)pvPortMalloc(11 * sizeof(u8));
		
		if(AlarmReport->device_channel != NULL)
		{
			memset(AlarmReport->device_channel,0,11);
			memset(AlarmReport->device_channel,'s',10);
		}
		
		AlarmReport->current_value = (u8 *)pvPortMalloc(11 * sizeof(u8));
		
		if(AlarmReport->current_value != NULL)
		{
			memset(AlarmReport->current_value,0,11);
			memset(AlarmReport->current_value,'v',10);
		}
		
		AlarmReport->set_value = (u8 *)pvPortMalloc(11 * sizeof(u8));
		
		if(AlarmReport->set_value != NULL)
		{
			memset(AlarmReport->set_value,0,11);
			memset(AlarmReport->set_value,'i',10);
		}
		
		AlarmReport->reference_value = (u8 *)pvPortMalloc(11 * sizeof(u8));
		
		if(AlarmReport->reference_value != NULL)
		{
			memset(AlarmReport->reference_value,0,11);
			memset(AlarmReport->reference_value,'p',10);
		}
		
		memset(AlarmReport->occur_time,0,15);
		TimeToString(AlarmReport->occur_time,calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
		
		
		CopyAlarmReport(AlarmReport,AlarmReportCpy);
		
		xQueueSend(xQueue_AlarmReportSend,(void *)&AlarmReport,(TickType_t)10);
		xQueueSend(xQueue_AlarmReportStore,(void *)&AlarmReport,(TickType_t)10);
	}
}

void vTaskCONCENTRATOR(void *pvParameters)
{
	u16 i = 0;
//	u8 res = 0;

//	delay_ms(5000);
//	res = mf_opendir("1:CONCEN/ALARM");

//	if(res == FR_OK)
//	{
//		res=mf_open("1:CONCEN/ALARM/000200328.A", FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
//		res=mf_lseek(f_size(file));
//		res=mf_write("AB\r\n",4);
//		res=mf_close();

//		res=mf_open("1:CONCEN/ALARM/000200328.A", FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
//		mf_read(100);
//		res=mf_close();



//		mf_scan_files("1:CONCEN/ALARM");

//		mf_unlink("1:CONCEN/ALARM/000200328.A");
//		mf_scan_files("1:CONCEN/ALARM");


//		res = mf_closedir();
//	}

//	delay_ms(5000);
//	
//	AlarmReport_S *AlarmReport = NULL;
//	AlarmReport = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));
//	
//	if(AlarmReport != NULL)
//	{
//		AlarmReport->record_type = 1;
//		AlarmReport->device_type = 0;
//		AlarmReport->alarm_type = 1;
//		AlarmReport->device_address = 5;
//		AlarmReport->device_channel = 5;
//		
//		AlarmReport->current_value = (u8 *)pvPortMalloc(11 * sizeof(u8));
//		
//		if(AlarmReport->current_value != NULL)
//		{
//			memset(AlarmReport->current_value,0,11);
//			memset(AlarmReport->current_value,'v',10);
//		}
//		
//		AlarmReport->set_value = (u8 *)pvPortMalloc(11 * sizeof(u8));
//		
//		if(AlarmReport->set_value != NULL)
//		{
//			memset(AlarmReport->set_value,0,11);
//			memset(AlarmReport->set_value,'i',10);
//		}
//		
//		AlarmReport->reference_value = (u8 *)pvPortMalloc(11 * sizeof(u8));
//		
//		if(AlarmReport->reference_value != NULL)
//		{
//			memset(AlarmReport->reference_value,0,11);
//			memset(AlarmReport->reference_value,'p',10);
//		}
//		
//		memset(AlarmReport->occur_time,0,15);
//		TimeToString(AlarmReport->occur_time,calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
//		
//		
//		xQueueSend(xQueue_AlarmReportSend,(void *)&AlarmReport,(TickType_t)10);
//		xQueueSend(xQueue_AlarmReportStore,(void *)&AlarmReport,(TickType_t)10);
		
//		for(i  = 0; i < 60000; i ++)
//		{
//			StoreAlarmToSpiFlash(AlarmReport);
//			
//			mf_scan_files("1:CONCEN/ALARM");
//		}
//	}
	

	while(1)
	{
//		if(i ++ == 100)
//		{
//			i = 0;
//			
//			xxxxxx();
//		}
		
		RecvAndHandleFrameStruct();						//报文解析

		AutoSendFrameToServer();						//发送登录/心跳/告警灯信息

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

		delay_ms(50);
	}
}

























































