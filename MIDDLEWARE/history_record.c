#include "history_record.h"
#include "fattester.h"
#include "concentrator_comm.h"
#include "common.h"
#include "server_protocol.h"
#include "rx8010s.h"
#include "exfuns.h"


//将告警历史存入SPIFLASH
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
	memset(&tmp[5],0,2);	//去掉年的高位
	memset(&tmp[13],0,6);	//tmp[]中前2字节为空，为record_num预留，7~12为年月日，13~最后为空

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
			res = f_readdir(&dir, &fileinfo);                   		//读取目录下的一个文件

			if (res != FR_OK || fileinfo.fname[0] == 0)
			{
				break;  //错误了/到末尾了,退出
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
			   fileinfo.fname[9] != 'A')								//判断文件日期距离今天是否超过30天
			{
				memset(file_name,0,32);
				strcat((char *)file_name,(char *)base_path);
				strcat((char *)file_name,"/");
				strcat((char *)file_name,fileinfo.fname);

				mf_unlink(file_name);									//超过30天的文件需要删除
			}
			else if(search_str((u8 *)fileinfo.fname, (u8 *)&tmp[7]) != -1)	//判断有没有今天的文件
			{
				memset(tmp,0,5);
				memcpy(tmp,fileinfo.fname,2);							//tmp[]中前2字节为record_num

				StrToHex((u8 *)&record_num, tmp, 1);					//读出今天的文件里有几条记录,转换为整数

				if(record_num == 0xFF)
				{
					memset(file_name,0,32);
					strcat((char *)file_name,(char *)base_path);
					strcat((char *)file_name,"/");
					strcat((char *)file_name,fileinfo.fname);

					mf_unlink(file_name);								//一个文件中存满255条记录后删除此文件

					record_num = 0;
				}
			}
		}

		memset(file_name,0,32);
		memset(tmp,0,4);												//tmp[]中前2字节重新放入新的record_num
		HexToStr(tmp, (u8 *)&record_num, 1);
		strcat((char *)file_name,(char *)base_path);					//根目录
		strcat((char *)file_name,"/");
		strcat((char *)file_name,tmp);									//数量record_num
		strcat((char *)file_name,&tmp[7]);								//年月日
		strcat((char *)file_name,".A");

		record_num += 1;
		if(record_num > 0xFF)
		{
			record_num = 0;
		}
		memset(file_name_new,0,32);
		memset(tmp,0,4);												//tmp[]中前4字节重新放入新的record_num
		HexToStr(tmp, (u8 *)&record_num, 1);
		strcat((char *)file_name_new,(char *)base_path);				//基本路径
		strcat((char *)file_name_new,"/");
		strcat((char *)file_name_new,tmp);								//数量record_num
		strcat((char *)file_name_new,&tmp[7]);							//年月日
		strcat((char *)file_name_new,".A");

		res = mf_open(file_name, FA_OPEN_ALWAYS|FA_WRITE|FA_READ);

		if(res == 0)
		{
			res = mf_lseek(f_size(file));

			if(res == 0)
			{
				memset(wr_buf,0,256);

				strcat(wr_buf,"A");										//告警指令
				if(alarm_report->record_type == 1)
				{
					strcat(wr_buf,"0");
				}
				else if(alarm_report->record_type == 0)
				{
					strcat(wr_buf,"1");
				}
				strcat(wr_buf,"|");

				memset(tmp,0,8);										//设备类型
				sprintf(tmp, "%d",alarm_report->device_type);
				strcat(wr_buf,tmp);
				strcat(wr_buf,"|");

				memset(tmp,0,8);										//告警类型
				sprintf(tmp, "%d",alarm_report->alarm_type);
				strcat(wr_buf,tmp);
				strcat(wr_buf,"|");

				strcat(wr_buf,(char *)alarm_report->device_address);	//设备地址
				strcat(wr_buf,"|");

				strcat(wr_buf,(char *)alarm_report->device_channel);	//设备通道
				strcat(wr_buf,"|");

				strcat(wr_buf,(char *)alarm_report->current_value);		//当前值
				strcat(wr_buf,"|");

				memset(tmp,0,256);										//设置值
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

				strcat(wr_buf,(char *)alarm_report->reference_value);	//参考值
				strcat(wr_buf,"|");

				strcat(wr_buf,(char *)alarm_report->occur_time);		//发生时间
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
					mf_unlink(file_name_new);							//删除重名文件
				}
			}
		}

		res = mf_closedir();
	}

	xSemaphoreGive(xMutex_SPI_FLASH);

	return res;
}

//通过日期段获取告警/事件/日志/条数
//device_type 设备类型
//msg_type 0告警 1事件 2日志
//s_date起始日期
//e_date结束日期
//返回 条数
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
			res = f_readdir(&dir, &fileinfo);                   		//读取目录下的一个文件

			if (res != FR_OK || fileinfo.fname[0] == 0)
			{
				break;  //错误了/到末尾了,退出
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
			   (fileinfo.fname[9] != 'A' && fileinfo.fname[9] != 'E' && fileinfo.fname[9] != 'J'))	//判断文件日期距离今天是否超过30天
			{
				memset(file_name,0,32);
				strcat((char *)file_name,(char *)base_path);
				strcat((char *)file_name,"/");
				strcat((char *)file_name,fileinfo.fname);

				mf_unlink(file_name);									//超过30天的文件需要删除
			}
			else if(start_date <= file_date && file_date <= end_date)	//判断有没有在日期范围内
			{
				memset(tmp,0,3);
				memcpy(tmp,fileinfo.fname,2);							//tmp[]中前2字节为record_num

				StrToHex((u8 *)&current_num, tmp, 1);					//读出今天的文件里有几条记录,转换为整数

				record_num += current_num;
			}
		}

		res = mf_closedir();
	}

	xSemaphoreGive(xMutex_SPI_FLASH);

	return record_num;
}

//通过日期段获取告警/事件/日志/条数
//device_type 设备类型
//msg_type 0告警 1事件 2日志
//s_date起始日期
//e_date结束日期
//返回 条数
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

	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

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
			res = f_readdir(&dir, &fileinfo);                   		//读取目录下的一个文件

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

				break;  //错误了/到末尾了,退出
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
			   (fileinfo.fname[9] != 'A' && fileinfo.fname[9] != 'E' && fileinfo.fname[9] != 'J'))	//判断文件日期距离今天是否超过30天
			{
				memset(file_name,0,32);
				strcat((char *)file_name,(char *)base_path);
				strcat((char *)file_name,"/");
				strcat((char *)file_name,fileinfo.fname);

				mf_unlink(file_name);									//超过30天的文件需要删除
			}
			else if(start_date <= file_date && file_date <= end_date)	//判断有没有在日期范围内
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

						if(res != 0)	//错误了/文件已关闭/退出
						{
							break;
						}
						else if(res == 0)
						{
							if(br < 512)	//文件已读完
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

									server_frame_struct->msg_type 	= (u8)SYNC_RESPONSE;	//响应服务器类型
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

								if(read_len > msg_len)	//还有未处理完的数据
								{
									read_len = read_len - end_pos - 2;
									read_pos = read_len;

									memcpy(read_buf,&read_buf[end_pos + 2],read_len);

									goto ANALYSIS_LOOP;
								}
								else						//所有数据均处理完
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

//删除告警上报结构体
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

//复制一个AlarmReport
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

//将告警信息放进告警发送和存储队列
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

































