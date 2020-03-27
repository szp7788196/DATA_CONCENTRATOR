#include "event_alarm.h"
#include "fattester.h"	
#include "concentrator.h"
#include "common.h"
#include "server_protocol.h"
#include "rx8010s.h"
#include "exfuns.h"	 



u8 StoreAlarmToSpiFlash(AlarmReport_S *alarm_report)
{
	u8 res = 0;
	u16 i = 0;
	u8 base_path[32] = {0};
	u8 file_name[32] = {0};
	u16 current_date = 0;
	u16 file_date = 0;
	u16 year = 0;
	u8 month = 0;
	u8 date = 0;
	u16 value_len = 0;
	char tmp[256];
	char wr_buf[256];
	
	if(alarm_report == NULL)
	{
		return res;
	}
	
	xSemaphoreTake(xMutex_SPI_FLASH, portMAX_DELAY);
	
	current_date = get_days_form_calendar(calendar.w_year,calendar.w_month,calendar.w_date);
	
	switch(alarm_report->device_type)
	{
		case (u8)CONCENTRATOR:
			memcpy(base_path,"1:CONCEN/ALARM",14);
		break;
		
		case (u8)LAMP_CONTROLLER:
			memcpy(base_path,"1:LAMP/ALARM",12);
		break;
		
		case (u8)RELAY:
			memcpy(base_path,"1:RELAY/ALARM",13);
		break;
		
		case (u8)INPUT_COLLECTOR:
			memcpy(base_path,"1:INPUT/ALARM",13);
		break;
		
		case (u8)ELECTRIC_METER:
			memcpy(base_path,"1:METER/ALARM",13);
		break;
		
		case (u8)LUMETER:
			memcpy(base_path,"1:LUMETER/ALARM",15);
		break;
		
		default:
//			return res;
		break;
	}
	
	res = mf_opendir(base_path);
	
	if(res == 0)
	{
		res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
		
		while(1)
		{
			if (res != FR_OK || fileinfo.fname[0] == 0) 
			{
				break;  //������/��ĩβ��,�˳�
			}
			   
			year = ((fileinfo.fname[0] - 0x30) * 1000) + 
				   ((fileinfo.fname[1] - 0x30) * 100) + 
				   ((fileinfo.fname[2] - 0x30) * 10) + 
				   (fileinfo.fname[3] - 0x30);
			
			month = ((fileinfo.fname[4] - 0x30) * 10) + 
				   (fileinfo.fname[5] - 0x30);
			
			date = ((fileinfo.fname[6] - 0x30) * 10) + 
				   (fileinfo.fname[7] - 0x30);
			
			file_date = get_days_form_calendar(year,month,date);
			
			if(current_date - file_date >= 30)		//�ж��ļ����ھ�������Ƿ񳬹�30��
			{
				memset(file_name,0,32);
				strcat((char *)file_name,(char *)base_path);
				strcat((char *)file_name,fileinfo.fname);
				
				mf_unlink(file_name);
			}
		}
		
		memset(file_name,0,32);
		strcat((char *)file_name,(char *)base_path);
		
		res = mf_open(file_name, FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
		
		if(res == 0)
		{
			res = mf_lseek(f_size(file));
			
			if(res == 0)
			{
				memset(wr_buf,0,256);
				
				strcat(wr_buf,"A");								//�澯ָ��
				if(alarm_report->record_type == 1)
				{
					strcat(wr_buf,"0");
				}
				else if(alarm_report->record_type == 0)
				{
					strcat(wr_buf,"1");
				}
				strcat(wr_buf,"|");
				
				memset(tmp,0,8);								//�豸����
				sprintf(tmp, "%d",alarm_report->device_type);
				strcat(wr_buf,tmp);
				strcat(wr_buf,"|");
				
				memset(tmp,0,8);								//�澯����
				sprintf(tmp, "%d",alarm_report->alarm_type);
				strcat(wr_buf,tmp);
				strcat(wr_buf,"|");
				
				memset(tmp,0,8);								//�豸��ַ
				sprintf(tmp, "%d",alarm_report->device_address);
				strcat(wr_buf,tmp);
				strcat(wr_buf,"|");
				
				memset(tmp,0,8);								//�豸ͨ��
				sprintf(tmp, "%d",alarm_report->device_channel);
				strcat(wr_buf,tmp);
				strcat(wr_buf,"|");
				
				strcat(wr_buf,(char *)alarm_report->current_value);		//��ǰֵ
				strcat(wr_buf,"|");
				
				memset(tmp,0,256);								//����ֵ
				value_len = strlen((char *)alarm_report->set_value);
				memcpy(tmp,alarm_report->set_value,value_len);
				for(i = 0; i < value_len; i ++)
				{
					if(tmp[i] == '|')
					{
						tmp[i] = ';';
					}
				}
				strcat(wr_buf,tmp);
				strcat(wr_buf,"|");
				
				strcat(wr_buf,(char *)alarm_report->reference_value);	//�ο�ֵ
				strcat(wr_buf,"|");
				
				strcat(wr_buf,(char *)alarm_report->occur_time);		//����ʱ��
				strcat(wr_buf,"\r\n");
				
				value_len = strlen(wr_buf);
				
				res = mf_write((u8 *)wr_buf,value_len);
			}
			
			res = mf_close();
		}
		
		res = mf_closedir();
	}
	
	xSemaphoreGive(xMutex_SPI_FLASH);
	
	return res;
}








































