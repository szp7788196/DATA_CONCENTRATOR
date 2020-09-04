#include "lamp_event.h"
#include "rx8010s.h"








void LampLamp_Power_CapacitorFaultAlarm(PlcFrame_S *user_frame,u8 record_type)
{
	u8 para_len = 0;
	char tmp[10] = {0};
	char buf[25] = {0};
	AlarmReport_S *alarm_report = NULL;
	
	if(user_frame->len != 11)
	{
		return;
	}
	
	alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

	if(alarm_report != NULL)
	{
		alarm_report->record_type = record_type;
		
		alarm_report->device_type = (u8)LAMP_CONTROLLER;
		alarm_report->alarm_type = *(user_frame->buf + 0);
		
		memset(buf,0,10);
		sprintf(buf, "%08X",user_frame->address);
		para_len = strlen(buf);
		alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_address != NULL)
		{
			memcpy(alarm_report->device_address,buf,para_len + 1);
		}
		
		memset(buf,0,5);
		sprintf(buf, "%d",2);
		para_len = strlen(buf);
		alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_channel != NULL)
		{
			memcpy(alarm_report->device_channel,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",*(user_frame->buf + 1));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 2));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->current_value != NULL)
		{
			memcpy(alarm_report->current_value,buf,para_len + 1);
		}
		
		memset(buf,0,2);
		para_len = 0;
		alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->set_value != NULL)
		{
			memcpy(alarm_report->set_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 3))) << 8) & 0xFF00) + *(user_frame->buf + 4));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 5))) << 8) & 0xFF00) + *(user_frame->buf + 6));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 7))) << 8) & 0xFF00) + *(user_frame->buf + 8));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->reference_value != NULL)
		{
			memcpy(alarm_report->reference_value,buf,para_len + 1);
		}
		
		alarm_report->occur_time[14] = 0;
		TimeToString(alarm_report->occur_time,
					 calendar.w_year,
					 calendar.w_month,
					 calendar.w_date,
					 calendar.hour,
					 calendar.min,calendar.sec);
		
		PushAlarmReportToAlarmQueue(alarm_report);
	}
}

void LampRelayFaultAlarm(PlcFrame_S *user_frame,u8 record_type)
{
	u8 para_len = 0;
	char tmp[10] = {0};
	char buf[25] = {0};
	AlarmReport_S *alarm_report = NULL;
	
	if(user_frame->len != 4)
	{
		return;
	}
	
	alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

	if(alarm_report != NULL)
	{
		alarm_report->record_type = record_type;
		
		alarm_report->device_type = (u8)LAMP_CONTROLLER;
		alarm_report->alarm_type = *(user_frame->buf + 0);
		
		memset(buf,0,10);
		sprintf(buf, "%08X",user_frame->address);
		para_len = strlen(buf);
		alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_address != NULL)
		{
			memcpy(alarm_report->device_address,buf,para_len + 1);
		}
		
		memset(buf,0,5);
		sprintf(buf, "%d",2);
		para_len = strlen(buf);
		alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_channel != NULL)
		{
			memcpy(alarm_report->device_channel,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",*(user_frame->buf + 1));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 2));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->current_value != NULL)
		{
			memcpy(alarm_report->current_value,buf,para_len + 1);
		}
		
		memset(buf,0,2);
		para_len = 0;
		alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->set_value != NULL)
		{
			memcpy(alarm_report->set_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(buf, "%d",*(user_frame->buf + 3));
		para_len = strlen(buf);
		alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->reference_value != NULL)
		{
			memcpy(alarm_report->reference_value,buf,para_len + 1);
		}
		
		alarm_report->occur_time[14] = 0;
		TimeToString(alarm_report->occur_time,
					 calendar.w_year,
					 calendar.w_month,
					 calendar.w_date,
					 calendar.hour,
					 calendar.min,calendar.sec);
		
		PushAlarmReportToAlarmQueue(alarm_report);
	}
}

void LampOverLowTemperatureAlarm(PlcFrame_S *user_frame,u8 record_type)
{
	u8 para_len = 0;
	char tmp[10] = {0};
	char buf[25] = {0};
	AlarmReport_S *alarm_report = NULL;
	
	if(user_frame->len != 6)
	{
		return;
	}
	
	alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

	if(alarm_report != NULL)
	{
		alarm_report->record_type = record_type;
		
		alarm_report->device_type = (u8)LAMP_CONTROLLER;
		alarm_report->alarm_type = *(user_frame->buf + 0);
		
		memset(buf,0,10);
		sprintf(buf, "%08X",user_frame->address);
		para_len = strlen(buf);
		alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_address != NULL)
		{
			memcpy(alarm_report->device_address,buf,para_len + 1);
		}
		
		memset(buf,0,5);
		sprintf(buf, "%d",2);
		para_len = strlen(buf);
		alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_channel != NULL)
		{
			memcpy(alarm_report->device_channel,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(buf, "%d",*(user_frame->buf + 1));
		para_len = strlen(buf);
		alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->current_value != NULL)
		{
			memcpy(alarm_report->current_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",*(user_frame->buf + 2));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 3));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 4))) << 8) & 0xFF00) + *(user_frame->buf + 5));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->set_value != NULL)
		{
			memcpy(alarm_report->set_value,buf,para_len + 1);
		}
		
		memset(buf,0,2);
		para_len = 0;
		alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->reference_value != NULL)
		{
			memcpy(alarm_report->reference_value,buf,para_len + 1);
		}
		
		alarm_report->occur_time[14] = 0;
		TimeToString(alarm_report->occur_time,
					 calendar.w_year,
					 calendar.w_month,
					 calendar.w_date,
					 calendar.hour,
					 calendar.min,calendar.sec);
		
		PushAlarmReportToAlarmQueue(alarm_report);
	}
}

void LampLeakageAlarm(PlcFrame_S *user_frame,u8 record_type)
{
	u8 para_len = 0;
	char tmp[10] = {0};
	char buf[25] = {0};
	AlarmReport_S *alarm_report = NULL;
	
	if(user_frame->len != 11)
	{
		return;
	}
	
	alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

	if(alarm_report != NULL)
	{
		alarm_report->record_type = record_type;
		
		alarm_report->device_type = (u8)LAMP_CONTROLLER;
		alarm_report->alarm_type = *(user_frame->buf + 0);
		
		memset(buf,0,10);
		sprintf(buf, "%08X",user_frame->address);
		para_len = strlen(buf);
		alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_address != NULL)
		{
			memcpy(alarm_report->device_address,buf,para_len + 1);
		}
		
		memset(buf,0,5);
		sprintf(buf, "%d",2);
		para_len = strlen(buf);
		alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_channel != NULL)
		{
			memcpy(alarm_report->device_channel,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 1))) << 8) & 0xFF00) + *(user_frame->buf + 2));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 3))) << 8) & 0xFF00) + *(user_frame->buf + 4));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->current_value != NULL)
		{
			memcpy(alarm_report->current_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 5))) << 8) & 0xFF00) + *(user_frame->buf + 6));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 7))) << 8) & 0xFF00) + *(user_frame->buf + 8));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 4))) << 9) & 0xFF00) + *(user_frame->buf + 10));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->set_value != NULL)
		{
			memcpy(alarm_report->set_value,buf,para_len + 1);
		}
		
		memset(buf,0,2);
		para_len = 0;
		alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->reference_value != NULL)
		{
			memcpy(alarm_report->reference_value,buf,para_len + 1);
		}
		
		alarm_report->occur_time[14] = 0;
		TimeToString(alarm_report->occur_time,
					 calendar.w_year,
					 calendar.w_month,
					 calendar.w_date,
					 calendar.hour,
					 calendar.min,calendar.sec);
		
		PushAlarmReportToAlarmQueue(alarm_report);
	}
}

void LampGateMagnetismAlarm(PlcFrame_S *user_frame,u8 record_type)
{
	u8 para_len = 0;
	char buf[25] = {0};
	AlarmReport_S *alarm_report = NULL;
	
	if(user_frame->len != 3)
	{
		return;
	}
	
	alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

	if(alarm_report != NULL)
	{
		alarm_report->record_type = record_type;
		
		alarm_report->device_type = (u8)LAMP_CONTROLLER;
		alarm_report->alarm_type = *(user_frame->buf + 0);
		
		memset(buf,0,10);
		sprintf(buf, "%08X",user_frame->address);
		para_len = strlen(buf);
		alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_address != NULL)
		{
			memcpy(alarm_report->device_address,buf,para_len + 1);
		}
		
		memset(buf,0,5);
		sprintf(buf, "%d",2);
		para_len = strlen(buf);
		alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_channel != NULL)
		{
			memcpy(alarm_report->device_channel,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(buf, "%d",*(user_frame->buf + 1));
		para_len = strlen(buf);
		alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->current_value != NULL)
		{
			memcpy(alarm_report->current_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(buf, "%d",*(user_frame->buf + 2));
		para_len = strlen(buf);
		alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->set_value != NULL)
		{
			memcpy(alarm_report->set_value,buf,para_len + 1);
		}
		
		memset(buf,0,2);
		para_len = 0;
		alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->reference_value != NULL)
		{
			memcpy(alarm_report->reference_value,buf,para_len + 1);
		}
		
		alarm_report->occur_time[14] = 0;
		TimeToString(alarm_report->occur_time,
					 calendar.w_year,
					 calendar.w_month,
					 calendar.w_date,
					 calendar.hour,
					 calendar.min,calendar.sec);
		
		PushAlarmReportToAlarmQueue(alarm_report);
	}
}

void LampPoleTiltAlarm(PlcFrame_S *user_frame,u8 record_type)
{
	u8 para_len = 0;
	char tmp[10] = {0};
	char buf[25] = {0};
	AlarmReport_S *alarm_report = NULL;
	
	if(user_frame->len != 6)
	{
		return;
	}
	
	alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

	if(alarm_report != NULL)
	{
		alarm_report->record_type = record_type;
		
		alarm_report->device_type = (u8)LAMP_CONTROLLER;
		alarm_report->alarm_type = *(user_frame->buf + 0);
		
		memset(buf,0,10);
		sprintf(buf, "%08X",user_frame->address);
		para_len = strlen(buf);
		alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_address != NULL)
		{
			memcpy(alarm_report->device_address,buf,para_len + 1);
		}
		
		memset(buf,0,5);
		sprintf(buf, "%d",2);
		para_len = strlen(buf);
		alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_channel != NULL)
		{
			memcpy(alarm_report->device_channel,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(buf, "%d",*(user_frame->buf + 1));
		para_len = strlen(buf);
		alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->current_value != NULL)
		{
			memcpy(alarm_report->current_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",*(user_frame->buf + 2));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 3))) << 8) & 0xFF00) + *(user_frame->buf + 4));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->set_value != NULL)
		{
			memcpy(alarm_report->set_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(buf, "%d",*(user_frame->buf + 5));
		para_len = strlen(buf);
		alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->reference_value != NULL)
		{
			memcpy(alarm_report->reference_value,buf,para_len + 1);
		}
		
		alarm_report->occur_time[14] = 0;
		TimeToString(alarm_report->occur_time,
					 calendar.w_year,
					 calendar.w_month,
					 calendar.w_date,
					 calendar.hour,
					 calendar.min,calendar.sec);
		
		PushAlarmReportToAlarmQueue(alarm_report);
	}
}

void LampElectricalParaOverThreAlarm(PlcFrame_S *user_frame,u8 record_type)
{
	u8 para_len = 0;
	char tmp[10] = {0};
	char buf[25] = {0};
	AlarmReport_S *alarm_report = NULL;
	
	if(user_frame->len != 14)
	{
		return;
	}
	
	alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

	if(alarm_report != NULL)
	{
		alarm_report->record_type = record_type;
		
		alarm_report->device_type = (u8)LAMP_CONTROLLER;
		alarm_report->alarm_type = *(user_frame->buf + 0);
		
		memset(buf,0,10);
		sprintf(buf, "%08X",user_frame->address);
		para_len = strlen(buf);
		alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_address != NULL)
		{
			memcpy(alarm_report->device_address,buf,para_len + 1);
		}
		
		memset(buf,0,5);
		sprintf(buf, "%d",2);
		para_len = strlen(buf);
		alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_channel != NULL)
		{
			memcpy(alarm_report->device_channel,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(buf, "%d",*(user_frame->buf + 1));
		para_len = strlen(buf);
		alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->current_value != NULL)
		{
			memcpy(alarm_report->current_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",*(user_frame->buf + 2));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 3));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 4))) << 8) & 0xFF00) + *(user_frame->buf + 5));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 6));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 7))) << 8) & 0xFF00) + *(user_frame->buf + 8));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 9));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 10))) << 8) & 0xFF00) + *(user_frame->buf + 11));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->set_value != NULL)
		{
			memcpy(alarm_report->set_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(buf, "%d",((((u16)(*(user_frame->buf + 12))) << 8) & 0xFF00) + *(user_frame->buf + 13));
		para_len = strlen(buf);
		alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->reference_value != NULL)
		{
			memcpy(alarm_report->reference_value,buf,para_len + 1);
		}
		
		alarm_report->occur_time[14] = 0;
		TimeToString(alarm_report->occur_time,
					 calendar.w_year,
					 calendar.w_month,
					 calendar.w_date,
					 calendar.hour,
					 calendar.min,calendar.sec);
		
		PushAlarmReportToAlarmQueue(alarm_report);
	}
}

void LampAbnormalLightOnOffAlarm(PlcFrame_S *user_frame,u8 record_type)
{
	u8 para_len = 0;
	char tmp[10] = {0};
	char buf[25] = {0};
	AlarmReport_S *alarm_report = NULL;
	
	if(user_frame->len != 12)
	{
		return;
	}
	
	alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

	if(alarm_report != NULL)
	{
		alarm_report->record_type = record_type;
		
		alarm_report->device_type = (u8)LAMP_CONTROLLER;
		alarm_report->alarm_type = *(user_frame->buf + 0);
		
		memset(buf,0,10);
		sprintf(buf, "%08X",user_frame->address);
		para_len = strlen(buf);
		alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_address != NULL)
		{
			memcpy(alarm_report->device_address,buf,para_len + 1);
		}
		
		memset(buf,0,5);
		sprintf(buf, "%d",2);
		para_len = strlen(buf);
		alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_channel != NULL)
		{
			memcpy(alarm_report->device_channel,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(buf, "%d",*(user_frame->buf + 1));
		para_len = strlen(buf);
		alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->current_value != NULL)
		{
			memcpy(alarm_report->current_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 2))) << 8) & 0xFF00) + *(user_frame->buf + 3));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 4))) << 8) & 0xFF00) + *(user_frame->buf + 5));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 6))) << 8) & 0xFF00) + *(user_frame->buf + 7));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->set_value != NULL)
		{
			memcpy(alarm_report->set_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 8))) << 8) & 0xFF00) + *(user_frame->buf + 9));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 10))) << 8) & 0xFF00) + *(user_frame->buf + 11));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->reference_value != NULL)
		{
			memcpy(alarm_report->reference_value,buf,para_len + 1);
		}
		
		alarm_report->occur_time[14] = 0;
		TimeToString(alarm_report->occur_time,
					 calendar.w_year,
					 calendar.w_month,
					 calendar.w_date,
					 calendar.hour,
					 calendar.min,calendar.sec);
		
		PushAlarmReportToAlarmQueue(alarm_report);
	}
}

void LampLightOnAbnormalAlarm(PlcFrame_S *user_frame,u8 record_type)
{
	u8 para_len = 0;
	char tmp[10] = {0};
	char buf[25] = {0};
	AlarmReport_S *alarm_report = NULL;
	
	if(user_frame->len != 17)
	{
		return;
	}
	
	alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

	if(alarm_report != NULL)
	{
		alarm_report->record_type = record_type;
		
		alarm_report->device_type = (u8)LAMP_CONTROLLER;
		alarm_report->alarm_type = *(user_frame->buf + 0);
		
		memset(buf,0,10);
		sprintf(buf, "%08X",user_frame->address);
		para_len = strlen(buf);
		alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_address != NULL)
		{
			memcpy(alarm_report->device_address,buf,para_len + 1);
		}
		
		memset(buf,0,5);
		sprintf(buf, "%d",2);
		para_len = strlen(buf);
		alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_channel != NULL)
		{
			memcpy(alarm_report->device_channel,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",*(user_frame->buf + 1));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 2))) << 8) & 0xFF00) + *(user_frame->buf + 3));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->current_value != NULL)
		{
			memcpy(alarm_report->current_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 4))) << 8) & 0xFF00) + *(user_frame->buf + 5));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 6));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 7));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 8))) << 8) & 0xFF00) + *(user_frame->buf + 9));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->set_value != NULL)
		{
			memcpy(alarm_report->set_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",*(user_frame->buf + 10));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 11))) << 8) & 0xFF00) + *(user_frame->buf + 12));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 13))) << 8) & 0xFF00) + *(user_frame->buf + 14));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 15))) << 8) & 0xFF00) + *(user_frame->buf + 16));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->reference_value != NULL)
		{
			memcpy(alarm_report->reference_value,buf,para_len + 1);
		}
		
		alarm_report->occur_time[14] = 0;
		TimeToString(alarm_report->occur_time,
					 calendar.w_year,
					 calendar.w_month,
					 calendar.w_date,
					 calendar.hour,
					 calendar.min,calendar.sec);
		
		PushAlarmReportToAlarmQueue(alarm_report);
	}
}

void LampTaskStateAbnormalAlarm(PlcFrame_S *user_frame,u8 record_type)
{
	u8 para_len = 0;
	char tmp[10] = {0};
	char buf[25] = {0};
	AlarmReport_S *alarm_report = NULL;
	
	if(user_frame->len < 12)
	{
		return;
	}
	
	alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

	if(alarm_report != NULL)
	{
		alarm_report->record_type = record_type;
		
		alarm_report->device_type = (u8)LAMP_CONTROLLER;
		alarm_report->alarm_type = *(user_frame->buf + 0);
		
		memset(buf,0,10);
		sprintf(buf, "%08X",user_frame->address);
		para_len = strlen(buf);
		alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_address != NULL)
		{
			memcpy(alarm_report->device_address,buf,para_len + 1);
		}
		
		memset(buf,0,5);
		sprintf(buf, "%d",2);
		para_len = strlen(buf);
		alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->device_channel != NULL)
		{
			memcpy(alarm_report->device_channel,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",*(user_frame->buf + 1));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 2));
		strcat(buf,tmp);
		para_len = strlen(buf);
		alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->current_value != NULL)
		{
			memcpy(alarm_report->current_value,buf,para_len + 1);
		}
		
		memset(buf,0,2);
		para_len = 0;
		alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->set_value != NULL)
		{
			memcpy(alarm_report->set_value,buf,para_len + 1);
		}
		
		memset(buf,0,25);
		sprintf(tmp, "%d",*(user_frame->buf + 3));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 4));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 5));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",((((u16)(*(user_frame->buf + 6))) << 8) & 0xFF00) + *(user_frame->buf + 7));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 8));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 9));
		strcat(buf,tmp);
		strcat(buf,",");
		sprintf(tmp, "%d",*(user_frame->buf + 10));
		strcat(buf,tmp);

		buf[strlen(buf) -1] = 0;		//去掉最后一个','
		para_len = strlen(buf);
		alarm_report->reference_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
		if(alarm_report->reference_value != NULL)
		{
			memcpy(alarm_report->reference_value,buf,para_len + 1);
		}
		
		alarm_report->occur_time[14] = 0;
		TimeToString(alarm_report->occur_time,
					 calendar.w_year,
					 calendar.w_month,
					 calendar.w_date,
					 calendar.hour,
					 calendar.min,calendar.sec);
		
		PushAlarmReportToAlarmQueue(alarm_report);
	}
}
































