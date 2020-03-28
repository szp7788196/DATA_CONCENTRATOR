#include "event_alarm.h"
#include "fattester.h"
#include "concentrator.h"
#include "common.h"
#include "server_protocol.h"
#include "rx8010s.h"
#include "exfuns.h"


//���澯��ʷ����SPIFLASH
u8 StoreAlarmToSpiFlash(AlarmReport_S *alarm_report)
{
	u8 res = 0;
	u16 i = 0;
	u8 base_path[32] = {0};
	u8 file_name[32] = {0};
	u8 file_name_new[32] = {0};
	u16 current_date = 0;
	u16 file_date = 0;
	u16 year = 0;
	u8 month = 0;
	u8 date = 0;
	u16 value_len = 0;
	u16 record_num = 0;
	char tmp[256] = {0};
	char wr_buf[256] = {0};

	if(alarm_report == NULL)
	{
		return res;
	}

	xSemaphoreTake(xMutex_SPI_FLASH, portMAX_DELAY);

	current_date = get_days_form_calendar(calendar.w_year,calendar.w_month,calendar.w_date);
	TimeToString((u8 *)&tmp[5],calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
	memset(&tmp[5],0,2);	//ȥ����ĸ�λ
	memset(&tmp[13],0,6);	//tmp[]��ǰ2�ֽ�Ϊ�գ�Ϊrecord_numԤ����7~12Ϊ�����գ�13~���Ϊ��

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
		while(1)
		{
			res = f_readdir(&dir, &fileinfo);                   		//��ȡĿ¼�µ�һ���ļ�

			if (res != FR_OK || fileinfo.fname[0] == 0)
			{
				break;  //������/��ĩβ��,�˳�
			}

			year = ((fileinfo.fname[2] - 0x30) * 10) +
				   (fileinfo.fname[3] - 0x30) + 2000;

			month = ((fileinfo.fname[4] - 0x30) * 10) +
				   (fileinfo.fname[5] - 0x30);

			date = ((fileinfo.fname[6] - 0x30) * 10) +
				   (fileinfo.fname[7] - 0x30);

			file_date = get_days_form_calendar(year,month,date);

			if(current_date - file_date >= 30 ||
			   current_date < file_date ||
			   fileinfo.fname[0] < '0' ||
			   fileinfo.fname[0] > 'F' ||
			   fileinfo.fname[1] < '0' ||
			   fileinfo.fname[1] > 'F' ||
			   year < 2020 ||
			   month < 1 ||
			   month > 12 ||
			   date < 1 ||
			   date > 31 ||
			   fileinfo.fname[8] != '.' ||
			   fileinfo.fname[9] != 'A')								//�ж��ļ����ھ�������Ƿ񳬹�30��
			{
				memset(file_name,0,32);
				strcat((char *)file_name,(char *)base_path);
				strcat((char *)file_name,"/");
				strcat((char *)file_name,fileinfo.fname);

				mf_unlink(file_name);									//����30����ļ���Ҫɾ��
			}

			if(search_str((u8 *)fileinfo.fname, (u8 *)&tmp[7]) != -1)	//�ж���û�н�����ļ�
			{
				memset(tmp,0,5);
				memcpy(tmp,fileinfo.fname,2);							//tmp[]��ǰ2�ֽ�Ϊrecord_num

				StrToHex((u8 *)&record_num, tmp, 1);					//����������ļ����м�����¼,ת��Ϊ����

				if(record_num == 0x20)
				{
					memset(file_name,0,32);
					strcat((char *)file_name,(char *)base_path);
					strcat((char *)file_name,"/");
					strcat((char *)file_name,fileinfo.fname);

					mf_unlink(file_name);								//һ���ļ��д���255����¼��ɾ�����ļ�

					record_num = 0;
				}
			}
		}

		memset(file_name,0,32);
		memset(tmp,0,4);												//tmp[]��ǰ2�ֽ����·����µ�record_num
		HexToStr(tmp, (u8 *)&record_num, 1);
		strcat((char *)file_name,(char *)base_path);					//��Ŀ¼
		strcat((char *)file_name,"/");
		strcat((char *)file_name,tmp);									//����record_num
		strcat((char *)file_name,&tmp[7]);								//������
		strcat((char *)file_name,".A");

		record_num += 1;
		if(record_num > 0xFF)
		{
			record_num = 0;
		}
		memset(file_name_new,0,32);
		memset(tmp,0,4);												//tmp[]��ǰ4�ֽ����·����µ�record_num
		HexToStr(tmp, (u8 *)&record_num, 1);
		strcat((char *)file_name_new,(char *)base_path);				//����·��
		strcat((char *)file_name_new,"/");
		strcat((char *)file_name_new,tmp);								//����record_num
		strcat((char *)file_name_new,&tmp[7]);							//������
		strcat((char *)file_name_new,".A");

		res = mf_open(file_name, FA_OPEN_ALWAYS|FA_WRITE|FA_READ);

		if(res == 0)
		{
			res = mf_lseek(f_size(file));

			if(res == 0)
			{
				memset(wr_buf,0,256);

				strcat(wr_buf,"A");										//�澯ָ��
				if(alarm_report->record_type == 1)
				{
					strcat(wr_buf,"0");
				}
				else if(alarm_report->record_type == 0)
				{
					strcat(wr_buf,"1");
				}
				strcat(wr_buf,"|");

				memset(tmp,0,8);										//�豸����
				sprintf(tmp, "%d",alarm_report->device_type);
				strcat(wr_buf,tmp);
				strcat(wr_buf,"|");

				memset(tmp,0,8);										//�澯����
				sprintf(tmp, "%d",alarm_report->alarm_type);
				strcat(wr_buf,tmp);
				strcat(wr_buf,"|");

				strcat(wr_buf,(char *)alarm_report->device_address);	//�豸��ַ
				strcat(wr_buf,"|");

				strcat(wr_buf,(char *)alarm_report->device_channel);	//�豸ͨ��
				strcat(wr_buf,"|");

				strcat(wr_buf,(char *)alarm_report->current_value);		//��ǰֵ
				strcat(wr_buf,"|");

				memset(tmp,0,256);										//����ֵ
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

			if(res == 0)
			{
				res = mf_rename(file_name,file_name_new);

				if(res != 0)
				{
					mf_unlink(file_name_new);							//ɾ�������ļ�
				}
			}
		}

		res = mf_closedir();
	}

	xSemaphoreGive(xMutex_SPI_FLASH);

	return res;
}


void DeleteAlarmReport(AlarmReport_S *alarm_report)
{
	if(alarm_report != NULL)
	{
		if(alarm_report->device_address != NULL)
		{
			vPortFree(alarm_report->device_address);
			alarm_report->device_address = NULL;
		}
		
		if(alarm_report->device_channel != NULL)
		{
			vPortFree(alarm_report->device_channel);
			alarm_report->current_value = NULL;
		}
		
		if(alarm_report->current_value != NULL)
		{
			vPortFree(alarm_report->current_value);
			alarm_report->current_value = NULL;
		}
		
		if(alarm_report->set_value != NULL)
		{
			vPortFree(alarm_report->set_value);
			alarm_report->set_value = NULL;
		}
		
		if(alarm_report->reference_value != NULL)
		{
			vPortFree(alarm_report->reference_value);
			alarm_report->reference_value = NULL;
		}
		
		if(alarm_report->occur_time != NULL)
		{
			vPortFree(alarm_report->occur_time);
			alarm_report->occur_time = NULL;
		}
		
		vPortFree(alarm_report);
		alarm_report = NULL;
	}
}

//����һ��AlarmReport
void CopyAlarmReport(AlarmReport_S *s_alarm_report,AlarmReport_S *d_alarm_report)
{
	u16 len = 0;
	
	if(s_alarm_report != NULL && d_alarm_report != NULL)
	{
		d_alarm_report->record_type = s_alarm_report->record_type;
		d_alarm_report->device_type = s_alarm_report->device_type;
		d_alarm_report->alarm_type = s_alarm_report->alarm_type;
		
		if(s_alarm_report->device_address != NULL)
		{
			len = strlen((char *)s_alarm_report->device_address);
			d_alarm_report->device_address = (u8 *)pvPortMalloc((len + 1) * sizeof(u8));
			memcpy(d_alarm_report->device_address,s_alarm_report->device_address,len + 1);
		}
		
		if(s_alarm_report->device_channel != NULL)
		{
			len = strlen((char *)s_alarm_report->device_channel);
			d_alarm_report->device_channel = (u8 *)pvPortMalloc((len + 1) * sizeof(u8));
			memcpy(d_alarm_report->device_channel,s_alarm_report->device_channel,len + 1);
		}
		
		if(s_alarm_report->current_value != NULL)
		{
			len = strlen((char *)s_alarm_report->current_value);
			d_alarm_report->current_value = (u8 *)pvPortMalloc((len + 1) * sizeof(u8));
			memcpy(d_alarm_report->current_value,s_alarm_report->current_value,len + 1);
		}
		
		if(s_alarm_report->set_value != NULL)
		{
			len = strlen((char *)s_alarm_report->set_value);
			d_alarm_report->set_value = (u8 *)pvPortMalloc((len + 1) * sizeof(u8));
			memcpy(d_alarm_report->set_value,s_alarm_report->set_value,len + 1);
		}
		
		if(s_alarm_report->reference_value != NULL)
		{
			len = strlen((char *)s_alarm_report->reference_value);
			d_alarm_report->reference_value = (u8 *)pvPortMalloc((len + 1) * sizeof(u8));
			memcpy(d_alarm_report->reference_value,s_alarm_report->reference_value,len + 1);
		}
		
		if(s_alarm_report->occur_time != NULL)
		{
			len = strlen((char *)s_alarm_report->occur_time);
			d_alarm_report->occur_time = (u8 *)pvPortMalloc((len + 1) * sizeof(u8));
			memcpy(d_alarm_report->occur_time,s_alarm_report->occur_time,len + 1);
		}
	}
}





































