#include "concentrator_event.h"
#include "concentrator_conf.h"
#include "history_record.h"
#include "e_meter.h"
#include "string.h"
#include "battery.h"
#include "common.h"
#include "rx8010s.h"



void ConcentratorEventCheckPolling(void)
{
	ConcentratorAlarmPowerFailure();
}

//断电告警
void ConcentratorAlarmPowerFailure(void)
{
	static s16 cnt = 0;
	u8 i = 0;
	u8 para_len = 0;
	char tmp[10] = {0};
	char buf[25] = {0};
	static u8 occur = 0;
	static u8 record = 0;
	float voltage = 0.0f;
	AlarmReport_S *alarm_report = NULL;

	if(ConcentratorAlarmConfig.power_off_alarm_enable == 1)		//告警以使能
	{
		voltage = BulitInMeterParas.voltage[PHASE_A];

		for(i = PHASE_A; i <= PHASE_C; i ++)					//选取ABC三项中电压最大的一相
		{
			if (voltage < BulitInMeterParas.voltage[i])
			{
				voltage = BulitInMeterParas.voltage[i];
			}
		}

		if(occur == 0)											//告警未发生或已解除
		{
			if(voltage <= (float)ConcentratorAlarmConfig.power_off_alarm_thre)
			{
				cnt ++;

				if(cnt == 300)									//低电压持续约30秒
				{
					cnt = 300;

					occur = 1;									//发生断电告警

					record = 1;
				}
			}
			else
			{
				cnt = 0;
			}
		}
		else
		{
			if(voltage > (float)ConcentratorAlarmConfig.power_off_alarm_thre &&
			  (voltage - (float)ConcentratorAlarmConfig.power_off_alarm_thre) /
			  (float)ConcentratorAlarmConfig.power_off_alarm_thre >=
			  ((float)ConcentratorAlarmConfig.power_off_alarm_rm_percent / 100.0f))
			{
				cnt --;

				if(cnt == -300)									//低电压持续约30秒
				{
					cnt = -300;

					occur = 0;									//发生断电告警

					record = 1;
				}
			}
			else
			{
				cnt = 0;
			}
		}

		if(record == 1)
		{
			record = 0;

			alarm_report = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));

			if(alarm_report != NULL)
			{
				if(cnt == 300)
				{
					alarm_report->record_type = 1;
				}
				else if(cnt == -300)
				{
					alarm_report->record_type = 0;
				}
				
				alarm_report->device_type = (u8)CONCENTRATOR;
				alarm_report->alarm_type = CONCENTRATOR_POWER_OFF;

				memset(buf,0,25);
				sprintf(buf, "%d",0);
				para_len = strlen(buf);
				alarm_report->device_address = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
				if(alarm_report->device_address != NULL)
				{
					memcpy(alarm_report->device_address,buf,para_len + 1);
				}

				memset(buf,0,25);
				sprintf(buf, "%d",0);
				para_len = strlen(buf);
				alarm_report->device_channel = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
				if(alarm_report->device_channel != NULL)
				{
					memcpy(alarm_report->device_channel,buf,para_len + 1);
				}

				memset(buf,0,25);
				sprintf(buf, "%f",voltage);
				para_len = strlen(buf);
				alarm_report->current_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
				if(alarm_report->current_value != NULL)
				{
					memcpy(alarm_report->current_value,buf,para_len + 1);
				}

				memset(buf,0,25);
				sprintf(tmp, "%d",ConcentratorAlarmConfig.power_off_alarm_thre);
				strcat(buf,tmp);
				strcat(buf,",");
				sprintf(tmp, "%d",ConcentratorAlarmConfig.power_off_alarm_rm_percent);
				strcat(buf,tmp);
				para_len = strlen(buf);
				alarm_report->set_value = (u8 *)pvPortMalloc((para_len + 1) * sizeof(u8));
				if(alarm_report->set_value != NULL)
				{
					memcpy(alarm_report->set_value,buf,para_len + 1);
				}

				memset(buf,0,25);
				sprintf(tmp, "%f",BatteryManagement.discharge_voltage);
				strcat(buf,tmp);
				strcat(buf,",");
				sprintf(tmp, "%f",BatteryManagement.discharge_current);
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
	}
}
















































