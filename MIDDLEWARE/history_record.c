#include "history_record.h"
#include "fattester.h"
#include "concentrator_comm.h"
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

	current_date = get_days_by_calendar(calendar.w_year,calendar.w_month,calendar.w_date);
	TimeToString((u8 *)&tmp[5],calendar.w_year, calendar.w_month, calendar.w_date, calendar.hour, calendar.min, calendar.sec);
	memset(&tmp[5],0,2);	//ȥ����ĸ�λ
	memset(&tmp[13],0,6);	//tmp[]��ǰ2�ֽ�Ϊ�գ�Ϊrecord_numԤ����7~12Ϊ�����գ�13~���Ϊ��

	switch(alarm_report->device_type)
	{
		case (u8)CONCENTRATOR:
			sprintf((char *)base_path, "1:CONCEN/ALARM");
		break;

		case (u8)LAMP_CONTROLLER:
			sprintf((char *)base_path, "1:LAMP/ALARM");
		break;

		case (u8)RELAY:
			sprintf((char *)base_path, "1:RELAY/ALARM");
		break;

		case (u8)INPUT_COLLECTOR:
			sprintf((char *)base_path, "1:INPUT/ALARM");
		break;

		case (u8)ELECTRIC_METER:
			sprintf((char *)base_path, "1:METER/ALARM");
		break;

		case (u8)LUMETER:
			sprintf((char *)base_path, "1:LUMETER/ALARM");
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

			file_date = get_days_by_calendar(year,month,date);

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
			else if(search_str((u8 *)fileinfo.fname, (u8 *)&tmp[7]) != -1)	//�ж���û�н�����ļ�
			{
				memset(tmp,0,5);
				memcpy(tmp,fileinfo.fname,2);							//tmp[]��ǰ2�ֽ�Ϊrecord_num

				StrToHex((u8 *)&record_num, tmp, 1);					//����������ļ����м�����¼,ת��Ϊ����

				if(record_num == 0xFF)
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

//ͨ�����ڶλ�ȡ�澯/�¼�/��־/����
//device_type �豸����
//msg_type 0�澯 1�¼� 2��־
//s_date��ʼ����
//e_date��������
//���� ����
u16 GetAlarmEventNumFromDateSegment(EventHistory_S event_history)
{
	u8 res = 0;
	u8 base_path[32] = {0};
	u8 file_name[32] = {0};
	u16 current_date = 0;
	u16 file_date = 0;
	u16 start_date = 0;
	u16 end_date = 0;
	u16 year = 0;
	u8 month = 0;
	u8 date = 0;
	char tmp[3] = {0};
	u16 current_num = 0;
	u16 record_num = 0;

	xSemaphoreTake(xMutex_SPI_FLASH, portMAX_DELAY);

	if(&event_history == NULL)
	{
		return 0;
	}

	current_date = get_days_by_calendar(calendar.w_year,calendar.w_month,calendar.w_date);

	year = ((event_history.start_date[2] - 0x30) * 10) +
		   (event_history.start_date[3] - 0x30) + 2000;

	month = ((event_history.start_date[4] - 0x30) * 10) +
		    (event_history.start_date[5] - 0x30);

	date = ((event_history.start_date[6] - 0x30) * 10) +
		   (event_history.start_date[7] - 0x30);

	start_date = get_days_by_calendar(year,month,date);

	year = ((event_history.end_date[2] - 0x30) * 10) +
		   (event_history.end_date[3] - 0x30) + 2000;

	month = ((event_history.end_date[4] - 0x30) * 10) +
		    (event_history.end_date[5] - 0x30);

	date = ((event_history.end_date[6] - 0x30) * 10) +
		   (event_history.end_date[7] - 0x30);

	end_date = get_days_by_calendar(year,month,date);

	switch((u8)event_history.device_type)
	{
		case (u8)CONCENTRATOR:
			sprintf((char *)base_path, "1:CONCEN/");
		break;

		case (u8)LAMP_CONTROLLER:
			sprintf((char *)base_path, "1:LAMP/");
		break;

		case (u8)RELAY:
			sprintf((char *)base_path, "1:RELAY/");
		break;

		case (u8)INPUT_COLLECTOR:
			sprintf((char *)base_path, "1:INPUT/");
		break;

		case (u8)ELECTRIC_METER:
			sprintf((char *)base_path, "1:METER/");
		break;

		case (u8)LUMETER:
			sprintf((char *)base_path, "1:LUMETER/");
		break;

		default:
//			return res;
		break;
	}

	switch((u8)event_history.event_type)
	{
		case (u8)TYPE_ALARM:
			strcat((char *)base_path,"ALARM");
		break;

		case (u8)TYPE_EVENT:
			strcat((char *)base_path,"EVENT");
		break;

		case (u8)TYPE_JOURNAL:
			strcat((char *)base_path,"JOURNAL");
		break;
		
		case (u8)TYPE_STATE:
			strcat((char *)base_path,"STATE");
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

			file_date = get_days_by_calendar(year,month,date);

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
			   (fileinfo.fname[9] != 'A' && fileinfo.fname[9] != 'E' && fileinfo.fname[9] != 'J'))	//�ж��ļ����ھ�������Ƿ񳬹�30��
			{
				memset(file_name,0,32);
				strcat((char *)file_name,(char *)base_path);
				strcat((char *)file_name,"/");
				strcat((char *)file_name,fileinfo.fname);

				mf_unlink(file_name);									//����30����ļ���Ҫɾ��
			}
			else if(start_date <= file_date && file_date <= end_date)	//�ж���û�������ڷ�Χ��
			{
				memset(tmp,0,3);
				memcpy(tmp,fileinfo.fname,2);							//tmp[]��ǰ2�ֽ�Ϊrecord_num

				StrToHex((u8 *)&current_num, tmp, 1);					//����������ļ����м�����¼,ת��Ϊ����

				record_num += current_num;
			}
		}

		res = mf_closedir();
	}

	xSemaphoreGive(xMutex_SPI_FLASH);

	return record_num;
}

//ͨ�����ڶλ�ȡ�澯/�¼�/��־/����
//device_type �豸����
//msg_type 0�澯 1�¼� 2��־
//s_date��ʼ����
//e_date��������
//���� ����
void GetAlarmEventContentFromDateSegmentAndSendToServer(EventHistory_S event_history)
{
	u8 res = 0;
	u8 i = 0;
	u8 base_path[32] = {0};
	u8 file_name[32] = {0};
	u16 current_date = 0;
	u16 file_date = 0;
	u16 start_date = 0;
	u16 end_date = 0;
	u16 year = 0;
	u8 month = 0;
	u8 date = 0;
	char tmp[10] = {0};
	u8 read_buf[1024] = {0};
	u16 patch_num = 1;
	u32 read_len = 0;
	u32 read_pos = 0;
	u16 end_pos = 0;
	u16 msg_len = 0;

	ServerFrameStruct_S *server_frame_struct = NULL;		//������Ӧ������

	xSemaphoreTake(xMutex_SPI_FLASH, portMAX_DELAY);

	if(&event_history == NULL)
	{
		goto GET_OUT;
	}

	current_date = get_days_by_calendar(calendar.w_year,calendar.w_month,calendar.w_date);

	year = ((event_history.start_date[2] - 0x30) * 10) +
		   (event_history.start_date[3] - 0x30) + 2000;

	month = ((event_history.start_date[4] - 0x30) * 10) +
		    (event_history.start_date[5] - 0x30);

	date = ((event_history.start_date[6] - 0x30) * 10) +
		   (event_history.start_date[7] - 0x30);

	start_date = get_days_by_calendar(year,month,date);

	year = ((event_history.end_date[2] - 0x30) * 10) +
		   (event_history.end_date[3] - 0x30) + 2000;

	month = ((event_history.end_date[4] - 0x30) * 10) +
		    (event_history.end_date[5] - 0x30);

	date = ((event_history.end_date[6] - 0x30) * 10) +
		   (event_history.end_date[7] - 0x30);

	end_date = get_days_by_calendar(year,month,date);

	switch((u8)event_history.device_type)
	{
		case (u8)CONCENTRATOR:
			sprintf((char *)base_path, "1:CONCEN/");
		break;

		case (u8)LAMP_CONTROLLER:
			sprintf((char *)base_path, "1:LAMP/");
		break;

		case (u8)RELAY:
			sprintf((char *)base_path, "1:RELAY/");
		break;

		case (u8)INPUT_COLLECTOR:
			sprintf((char *)base_path, "1:INPUT/");
		break;

		case (u8)ELECTRIC_METER:
			sprintf((char *)base_path, "1:METER/");
		break;

		case (u8)LUMETER:
			sprintf((char *)base_path, "1:LUMETER/");
		break;

		default:
//			return res;
		break;
	}

	switch((u8)event_history.event_type)
	{
		case (u8)TYPE_ALARM:
			strcat((char *)base_path,"ALARM");
		break;

		case (u8)TYPE_EVENT:
			strcat((char *)base_path,"EVENT");
		break;

		case (u8)TYPE_JOURNAL:
			strcat((char *)base_path,"JOURNAL");
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
				if(server_frame_struct != NULL)
				{
					if(i <= 1)
					{
						DeleteServerFrameStruct(server_frame_struct);
					}
					else if(i > 1)
					{
						server_frame_struct->para_num = i;

						ConvertFrameStructToFrame(server_frame_struct);
					}
				}

				break;  //������/��ĩβ��,�˳�
			}

			year = ((fileinfo.fname[2] - 0x30) * 10) +
				   (fileinfo.fname[3] - 0x30) + 2000;

			month = ((fileinfo.fname[4] - 0x30) * 10) +
				   (fileinfo.fname[5] - 0x30);

			date = ((fileinfo.fname[6] - 0x30) * 10) +
				   (fileinfo.fname[7] - 0x30);

			file_date = get_days_by_calendar(year,month,date);

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
			   (fileinfo.fname[9] != 'A' && fileinfo.fname[9] != 'E' && fileinfo.fname[9] != 'J'))	//�ж��ļ����ھ�������Ƿ񳬹�30��
			{
				memset(file_name,0,32);
				strcat((char *)file_name,(char *)base_path);
				strcat((char *)file_name,"/");
				strcat((char *)file_name,fileinfo.fname);

				mf_unlink(file_name);									//����30����ļ���Ҫɾ��
			}
			else if(start_date <= file_date && file_date <= end_date)	//�ж���û�������ڷ�Χ��
			{
				memset(file_name,0,32);
				strcat((char *)file_name,(char *)base_path);
				strcat((char *)file_name,"/");
				strcat((char *)file_name,fileinfo.fname);

				res = mf_open(file_name, FA_OPEN_ALWAYS|FA_WRITE|FA_READ);

				if(res == 0)
				{
					read_pos = 0;

					while(1)
					{
						res = f_read(file,&read_buf[read_pos],512,&br);

						if(res != 0)	//������/�ļ��ѹر�/�˳�
						{
							break;
						}
						else if(res == 0)
						{
							if(br < 512)	//�ļ��Ѷ���
							{
								read_len = br;

								res = mf_close();
							}
							else
							{
								read_len = 512;
							}

							read_pos = read_pos + read_len;
							read_len = read_pos;

							ANALYSIS_LOOP:
							if(i == 0 && server_frame_struct == NULL)
							{
								server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

								if(server_frame_struct == NULL)
								{
									res = mf_close();
									res = mf_closedir();

									goto GET_OUT;
								}
								else
								{
									InitServerFrameStruct(server_frame_struct);

									server_frame_struct->msg_type 	= (u8)SYNC_RESPONSE;	//��Ӧ����������
									server_frame_struct->msg_len 	= 10;
									server_frame_struct->err_code 	= (u8)NO_ERR;
									server_frame_struct->msg_id		= 0x00A5;
									server_frame_struct->para_num	= EVENT_NUM_OF_BATCH + 1;

									server_frame_struct->msg_id += ((((u16)event_history.device_type) << 8) & 0xFF00);

									server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

									if(server_frame_struct->para == NULL)
									{
										DeleteServerFrameStruct(server_frame_struct);

										res = mf_close();
										res = mf_closedir();

										goto GET_OUT;
									}
									else
									{
										server_frame_struct->para[i].type = 0x8001;
										memset(tmp,0,10);
										sprintf(tmp, "%d",patch_num ++);
										server_frame_struct->para[i].len = strlen(tmp);
										server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
										if(server_frame_struct->para[i].value != NULL)
										{
											memcpy(server_frame_struct->para[i].value,tmp,server_frame_struct->para[i].len + 1);
										}
										i ++;
									}
								}
							}

							end_pos = MyStrstr(read_buf, "\r\n", read_len, 2);

							if(end_pos != 0xFFFF)
							{
								server_frame_struct->para[i].type = 0x4001 + i;
								msg_len = end_pos;
								server_frame_struct->para[i].len = end_pos;
								server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
								if(server_frame_struct->para[i].value != NULL)
								{
									server_frame_struct->para[i].value[server_frame_struct->para[i].len] = 0;
									memcpy(server_frame_struct->para[i].value,read_buf,server_frame_struct->para[i].len);
								}
								printf("%s",server_frame_struct->para[i].value);
								i ++;

								if(i == EVENT_NUM_OF_BATCH + 1)
								{
									i = 0;

									ConvertFrameStructToFrame(server_frame_struct);

									server_frame_struct = NULL;
									
									delay_ms(250);
								}

								if(read_len > msg_len)	//����δ�����������
								{
									read_len = read_len - end_pos - 2;
									read_pos = read_len;

									memcpy(read_buf,&read_buf[end_pos + 2],read_len);

									goto ANALYSIS_LOOP;
								}
								else						//�������ݾ�������
								{
									read_pos = 0;
									read_len = 0;
									msg_len = 0;
								}
							}
						}
					}
				}
			}
		}

		res = mf_closedir();
	}

	GET_OUT:
	xSemaphoreGive(xMutex_SPI_FLASH);
}

//ɾ���澯�ϱ��ṹ��
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
			alarm_report->device_channel = NULL;
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

		memcpy(d_alarm_report->occur_time,s_alarm_report->occur_time,15);
	}
}

void DeleteEventHistory(EventHistory_S *event_history)
{
	if(event_history != NULL)
	{
		if(event_history->start_date != NULL)
		{
			vPortFree(event_history->start_date );
			event_history->start_date  = NULL;
		}

		if(event_history->end_date != NULL)
		{
			vPortFree(event_history->end_date );
			event_history->end_date  = NULL;
		}

		vPortFree(event_history);
		event_history = NULL;
	}
}

//���澯��Ϣ�Ž��澯���ͺʹ洢����
void PushAlarmReportToAlarmQueue(AlarmReport_S *alarm_report)
{
	AlarmReport_S *alarm_report_cpy = NULL;
	
	if(alarm_report != NULL)
	{
		alarm_report_cpy = (AlarmReport_S *)pvPortMalloc(sizeof(AlarmReport_S));
		
		if(alarm_report_cpy != NULL)
		{
			CopyAlarmReport(alarm_report,alarm_report_cpy);
			
			xSemaphoreTake(xMutex_Push_xQueue_AlarmReportStore, portMAX_DELAY);
		
			if(xQueueSend(xQueue_AlarmReportStore,(void *)&alarm_report_cpy,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_AlarmReportStore fail.\r\n");
#endif
				DeleteAlarmReport(alarm_report_cpy);
			}
			
			xSemaphoreGive(xMutex_Push_xQueue_AlarmReportStore);
		}

		xSemaphoreTake(xMutex_Push_xQueue_AlarmReportSend, portMAX_DELAY);
		
		if(xQueueSend(xQueue_AlarmReportSend,(void *)&alarm_report,(TickType_t)10) != pdPASS)
		{
#ifdef DEBUG_LOG
			printf("send xQueue_AlarmReportSend fail.\r\n");
#endif
			DeleteAlarmReport(alarm_report);
		}
		
		xSemaphoreGive(xMutex_Push_xQueue_AlarmReportSend);
	}
}

































