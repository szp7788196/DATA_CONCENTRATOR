#include "plc_protocol.h"
#include "stmflash.h"
#include "plc.h"
#include "lamp_event.h"
#include "history_record.h"



u8 CombinePlcUserFrame(LampPlcExecuteTask_S *task,PlcFrame_S *frame,LampConfig_S lamp_config)
{
	u8 ret = 1;
	u8 ret1 = 0;
	u16 i = 0;
	u8 buf[256] = {0};
	u32 bag_buf[32] = {0};
	LampStrategyGroupSwitch_S *strategy_switch = NULL;
	LampAlarmConf_S *lamp_alarm_conf = NULL;
	LampSenceConfig_S appointment;
	LampTask_S lamp_task;
	PlcTask_S plc_task;
	u32 new_add = 0;
	u16 bag_num = 0;
	u16 flash_read_index = 0;
	u8 mode_id = 0;
	u16 crc_16 = 0;
	static u8 add_index = 0;

	if(frame->buf != NULL)
	{
		return 0;
	}

	frame->cmd_code = (u8)(task->cmd_code - 0x0100);

	switch(task->cmd_code)
	{
		case 0x0100:	//透传
			frame->len = task->data_len;

			frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

			if(frame->buf != NULL)
			{
				memcpy(frame->buf,task->data,frame->len);
			}
		break;

		case 0x0101:	//校时
			frame->len = task->data_len;

			frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

			if(frame->buf != NULL)
			{
				memcpy(frame->buf,task->data,frame->len);
			}
		break;

		case 0x0102:	//复位
			frame->len = task->data_len;

			frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

			if(frame->buf != NULL)
			{
				memcpy(frame->buf,task->data,frame->len);
			}
		break;

		case 0x0103:	//调光
			frame->len = task->data_len;

			frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

			if(frame->buf != NULL)
			{
				memcpy(buf,task->data,task->data_len);

				if(buf[0] == 0x02)
				{
					buf[2] = buf[1];
				}

				memcpy(frame->buf,buf,frame->len);
			}
		break;

		case 0x0104:	//闪测
			frame->len = task->data_len;

			frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

			if(frame->buf != NULL)
			{
				memcpy(frame->buf,task->data,frame->len);
			}
		break;

		case 0x0105:	//模式切换
			strategy_switch = (LampStrategyGroupSwitch_S *)task->data;
			frame->len = task->data_len;

			frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

			if(frame->buf != NULL)
			{
				memcpy(frame->buf,strategy_switch,frame->len);
			}
		break;

		case 0x0106:	//手自控切换
			frame->len = task->data_len;

			frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

			if(frame->buf != NULL)
			{
				memcpy(frame->buf,task->data,frame->len);
			}
		break;

		case 0x0170:	//状态查询 数据为空

		break;

		case 0x0172:	//状态历史查询（预留） 数据为空

		break;

		case 0x01A3:	//告警配置
			lamp_alarm_conf = (LampAlarmConf_S *)task->data;
			frame->len = task->data_len;

			frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

			if(frame->buf != NULL)
			{
				memcpy(frame->buf,lamp_alarm_conf,frame->len);
			}
		break;

		case 0x01A4:	//告警配置查询 数据为空

		break;

		case 0x01A5:	//告警历史查询（预留）

		break;

		case 0x01D0:		//基础配置指令
			lamp_config.address = ConcentratorGateWayID.number;

			frame->len = sizeof(LampConfig_S);

			frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

			if(frame->buf != NULL)
			{
				memcpy(frame->buf,&lamp_config,frame->len);
			}
		break;

		case 0x01D2:	//重新写址
			frame->len = task->data_len;

			frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

			if(frame->buf != NULL)
			{
				memcpy(&new_add,task->data,frame->len);

				buf[0] = (u8)((new_add >> 24) & 0x000000FF);
				buf[1] = (u8)((new_add >> 16) & 0x000000FF);
				buf[2] = (u8)((new_add >>  8) & 0x000000FF);
				buf[3] = (u8)((new_add >>  0) & 0x000000FF);

				memcpy(frame->buf,buf,frame->len);
			}
		break;

		case 0x01D3:	//场景配置指令
			if(task->broadcast_type == 2)			//单播
			{
				mode_id = task->executed_num / task->dev_num;
			}
			else									//组播和广播
			{
				mode_id = task->executed_num;
			}

			memset(&appointment,0,sizeof(LampSenceConfig_S));

			ret = ReadLampAppointment(mode_id,&appointment);

			if(ret == 1)
			{
				frame->len = sizeof(LampSenceConfig_S);

				frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

				if(frame->buf != NULL)
				{
					memcpy(frame->buf,&appointment,frame->len);
				}
			}
		break;

		case 0x01D5:	//任务配置指令
			READ_TASK_CONF:
			ret = ReadSpecifyLampStrategyNumList(task->executed_index);
		
			if(ret == 1)
			{
				memset(&lamp_task,0,sizeof(LampTask_S));
				
				ret1 = ReadLampTaskConfig(task->executed_index,&lamp_task);
				
				if(ret1 == 1)
				{
					memset(frame,0,sizeof(PlcFrame_S));
					
					if(lamp_task.ctrl_mode == 0 || lamp_task.ctrl_mode ==1)		//组播或广播
					{
						frame->cmd_code = (u8)(task->cmd_code - 0x0100);
						frame->address = 0xFFFFFFFF;
						frame->type = lamp_task.ctrl_mode;
						frame->wait_ack = 0;
						frame->resp_ack = 0;
						
						if(lamp_task.ctrl_mode ==1)
						{
							for(i = 0; i < MAX_LAMP_GROUP_NUM; i ++)
							{
								if((u8)lamp_task.group_add[i] != 0)
								{
									frame->group_id[i] = (u8)lamp_task.group_add[i];
								}
								else
								{
									frame->group_num = i;
									
									break;
								}
							}	
						}
					}
					else
					{
						frame->cmd_code = (u8)(task->cmd_code - 0x0100);
						frame->address = lamp_task.group_add[add_index ++];
						frame->type = lamp_task.ctrl_mode;
						frame->wait_ack = 1;
						frame->resp_ack = 0;
						
						if(lamp_task.group_add[add_index] == 0)
						{
							add_index = 0;
						}
					}
					
					memset(&plc_task,0,sizeof(PlcTask_S));
					
					plc_task.group_id 		= lamp_task.group_id;
					plc_task.type 			= lamp_task.type;
					plc_task.executor 		= lamp_task.executor;
					plc_task.time 			= lamp_task.time;
					plc_task.time_option 	= lamp_task.time_option;
					plc_task.brightness[0] 	= lamp_task.brightness[0];
					plc_task.brightness[1] 	= lamp_task.brightness[1];
					
					frame->len = sizeof(PlcTask_S);

					frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

					if(frame->buf != NULL)
					{
						memcpy(frame->buf,&plc_task,frame->len);
					}
					
					task->executed_num ++;
				}
			}
			
			if(lamp_task.ctrl_mode == 2 && add_index != 0)
			{
				task->executed_index = task->executed_index;
			}
			else
			{
				task->executed_index ++;
			}
			
			if(task->executed_index <= MAX_LAMP_STRATEGY_NUM)
			{
				if(ret1 == 0)
				{
					add_index = 0;
					
					goto READ_TASK_CONF;
				}
			}
			else
			{
				task->state = STATE_FINISHED;
			}
		break;

		case 0x01D8:	//节点搜索 数据为空

		break;

		case 0x01F3:	//开始升级
			bag_num = LampFrameWareState.total_size / LAMP_FIRMWARE_BAG_SEND_SIZE;

			if(task->broadcast_type == 2)			//单播
			{
				flash_read_index = task->executed_num / task->dev_num;
			}
			else									//组播和广播
			{
				flash_read_index = task->executed_num;
			}

			buf[0] = (u8)(((flash_read_index + 1) >> 8) & 0x00FF);
			buf[1] = (u8)(((flash_read_index + 1) >> 0) & 0x00FF);

			buf[2] = (u8)((bag_num >> 8) & 0x00FF);
			buf[3] = (u8)((bag_num >> 0) & 0x00FF);

			buf[4] = (u8)((LAMP_FIRMWARE_BAG_SEND_SIZE >> 8) & 0x00FF);
			buf[5] = (u8)((LAMP_FIRMWARE_BAG_SEND_SIZE >> 0) & 0x00FF);

			STMFLASH_Read(LAMP_FIRMWARE_BUCKUP_FLASH_BASE_ADD +
						 (LAMP_FIRMWARE_BAG_SEND_SIZE * flash_read_index),
						  bag_buf,LAMP_FIRMWARE_BAG_SEND_SIZE >> 2);

			for(i = 0; i < LAMP_FIRMWARE_BAG_SEND_SIZE >> 2; i ++)
			{
				buf[6 + (i * 4) + 3] = (u8)((bag_buf[i] >> 24) & 0x000000FF);
				buf[6 + (i * 4) + 2] = (u8)((bag_buf[i] >> 16) & 0x000000FF);
				buf[6 + (i * 4) + 1] = (u8)((bag_buf[i] >>  8) & 0x000000FF);
				buf[6 + (i * 4) + 0] = (u8)((bag_buf[i] >>  0) & 0x000000FF);
			}
			
			crc_16 = CRC16(&buf[6],LAMP_FIRMWARE_BAG_SEND_SIZE);
			
			buf[6 + LAMP_FIRMWARE_BAG_SEND_SIZE + 0] = (u8)((crc_16 >> 8) & 0x00FF);
			buf[6 + LAMP_FIRMWARE_BAG_SEND_SIZE + 1] = (u8)((crc_16 >> 0) & 0x00FF);
			
			frame->len = 6 + LAMP_FIRMWARE_BAG_SEND_SIZE + 2;

			frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

			if(frame->buf != NULL)
			{
				memcpy(frame->buf,buf,frame->len);
			}
		break;

		case 0x01F6:	//版本查询 数据为空

		break;

		default:

		break;
	}

	return ret;
}

u8 CombinePlcBottomFrame(PlcFrame_S *frame,u8 *outbuf)
{
	u8 i = 0;
	u8 len = 0;
	u8 buf[256] = {0};
	u8 address[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

	buf[0] = (u8)((frame->address >> 24) & 0x000000FF);
	buf[1] = (u8)((frame->address >> 16) & 0x000000FF);
	buf[2] = (u8)((frame->address >>  8) & 0x000000FF);
	buf[3] = (u8)((frame->address >>  0) & 0x000000FF);

	buf[4] = frame->cmd_code;
	buf[5] = frame->type;

	switch(frame->type)
	{
		case 0:
			memcpy(buf + 6,frame->buf,frame->len);

			len = 6 + frame->len;
		break;

		case 1:
			buf[6] = frame->group_num;

			for(i = 0; i < frame->group_num; i ++)
			{
				buf[7 + i] = frame->group_id[i];
			}

			memcpy(buf + 7 + frame->group_num,frame->buf,frame->len);

			len = 7 + frame->group_num + frame->len;
		break;

		case 2:
			memcpy(buf + 6,frame->buf,frame->len);

			len = 6 + frame->len;
		break;

		default:

		break;
	}

	len = plc_pack_user_data(address,buf,len,outbuf + 5);

	len = plc_combine_data(0x14,outbuf + 5,len,outbuf);

	return len;
}

EXECUTE_STATE_E SendPlcFrameToDeviceAndWaitResponse(PlcFrame_S in_frame)
{
	EXECUTE_STATE_E ret = STATE_START;
	PlcFrame_S *ack_frame = NULL;
	u8 send_buf[256] = {0};
	u8 send_len = 0;
	u16 time_out = 0;
	u8 re_send_times = 0;
	u8 responsed = 0;

	send_len = CombinePlcBottomFrame(&in_frame,send_buf);

	if(send_len == 0)
	{
		ret = STATE_FAIL;

		goto GET_OUT;
	}

	RE_SEND:
	xSemaphoreTake(xMutex_USART2, portMAX_DELAY);

	UsartSendString(USART2,send_buf, send_len);

	xSemaphoreGive(xMutex_USART2);

	if(in_frame.type == 0 || in_frame.type == 1)				//广播和组播
	{
		delay_ms(LampBasicConfig.broadcast_interval_time);

		re_send_times ++;

		if(re_send_times >= LampBasicConfig.broadcast_times)
		{
			ret = STATE_SUCCESS;

			goto GET_OUT;
		}
		else
		{
			goto RE_SEND;
		}
	}
	else if(in_frame.type == 2 && in_frame.wait_ack == 1)	//单播并且需要等待响应
	{
		time_out = 5 * 100;

		while(time_out)
		{
			time_out --;

			delay_ms(10);

			ack_frame = RecvAndAnalysisPlcBottomFrame(0);

			if(ack_frame != NULL)
			{
				if(in_frame.cmd_code == 0xD2)
				{
					in_frame.address = ((((u32)in_frame.buf[0]) << 24) & 0xFF000000) + 
							           ((((u32)in_frame.buf[1]) << 16) & 0x00FF0000) + 
							           ((((u32)in_frame.buf[2]) <<  8) & 0x0000FF00) + 
							           ((((u32)in_frame.buf[3]) <<  0) & 0x000000FF);
				}

				if(ack_frame->address == in_frame.address &&
				  (ack_frame->cmd_code == 0x80 || 
				   ack_frame->cmd_code == in_frame.cmd_code))
				{
					time_out = 0;
					responsed = 1;

					ret = STATE_SUCCESS;
				}
				
				DeletePlcFrame(ack_frame);
				ack_frame = NULL;
			}
		}

		if(responsed == 0)
		{
			re_send_times ++;

			if(re_send_times <= LampBasicConfig.retransmission_times)
			{
				goto RE_SEND;
			}
			else
			{
				ret = STATE_TIMEOUT;
			}
		}
	}

	GET_OUT:
	return ret;
}

//mode 0只解析不清空 1解析并清空
PlcFrame_S* RecvAndAnalysisPlcBottomFrame(u8 mode)
{
	PlcFrame_S *ack_frame = NULL;

	u8 *inbuf = NULL;
	u8 len = 0;
	u8 cmd_id = 0;
	u8 check_sum_read = 0;
	u8 check_sum_cal = 0;
	u8 check_xor_read = 0;
	u8 check_xor_cal = 0;
	u16 frame_len = 0;
	u16 user_data_len = 0;
	u32 local_add = 0;
	u8 dev_add[6] = {0};

	xSemaphoreTake(xMutex_USART2, portMAX_DELAY);

	if(Usart2RecvEnd != 0xAA)
	{
		goto GET_OUT;
	}

	if(Usart2FrameLen < 8)
	{
		Usart2RecvEnd = 0;
		Usart2FrameLen = 0;

		goto GET_OUT;
	}

	inbuf = Usart2RxBuf;
	len = Usart2FrameLen;

	if(mode == 1)
	{
		Usart2RecvEnd = 0;
		Usart2FrameLen = 0;
	}

	check_sum_read = *(inbuf + len - 2);

	check_sum_cal = at_csum(inbuf + 1,len - 3);

	if(check_sum_read != check_sum_cal)
	{
		goto GET_OUT;
	}

	check_xor_read = *(inbuf + len - 1);

	check_xor_cal = at_cxor(inbuf + 1,len - 3);

	if(check_xor_read != check_xor_cal)
	{
		goto GET_OUT;
	}

	if(*(inbuf + 0) != 0x79)
	{
		goto GET_OUT;
	}

	frame_len = (((u16)(*(inbuf + 2))) << 8) + (u16)(*(inbuf + 1));

	if(frame_len + 7 != len)
	{
		goto GET_OUT;
	}

	cmd_id = *(inbuf + 4);

	memcpy(dev_add,inbuf + 8,6);

	if(cmd_id != 0x15)
	{
		Usart2RecvEnd = 0;
		Usart2FrameLen = 0;
	}

	switch(cmd_id)
	{
		case 0x0B:		//读取模块地址应答

		break;

		case 0x0C:		//设置模块地址应答

		break;

		case 0x15:		//接收到数据
			user_data_len = (((u16)(*(inbuf + 15))) << 8) + (u16)(*(inbuf + 14));

			if(frame_len != user_data_len + 3 + 6 + 2)
			{
				goto GET_OUT;
			}

			ack_frame = (PlcFrame_S *)pvPortMalloc(sizeof(PlcFrame_S));

			if(ack_frame != NULL)
			{
				memset(ack_frame,0,sizeof(PlcFrame_S));
				

				ack_frame->address = ((((u32)dev_add[2]) << 24) & 0xFF000000) +
				                     ((((u32)dev_add[3]) << 16) & 0x00FF0000) +
				                     ((((u32)dev_add[4]) <<  8) & 0x0000FF00) +
				                     ((((u32)dev_add[5]) <<  0) & 0x000000FF);		//PLC设备地址
				ack_frame->wait_ack = 1;
				ack_frame->resp_ack = 1;

				local_add = ((((u16)(*(inbuf + 16))) << 24) & 0xFF000000) +
						    ((((u16)(*(inbuf + 17))) << 16) & 0x00FF0000) +
						    ((((u16)(*(inbuf + 18))) <<  8) & 0x0000FF00) +
						    ((((u16)(*(inbuf + 19))) <<  0) & 0x000000FF);			//集控器地址

				if(local_add == ConcentratorGateWayID.number)
				{
					ack_frame->cmd_code = *(inbuf + 20);							//命令字
					ack_frame->type = *(inbuf + 21);								//播送方式

					if(ack_frame->type == 2)										//必须为单播模式
					{
						ack_frame->len = user_data_len - 6;							//实际数据域长度 除去地址 命令字 播送类型 组号个数 组号剩余的数据域长度

						ack_frame->buf = (u8 *)pvPortMalloc(ack_frame->len * sizeof(u8));

						if(ack_frame->buf != NULL)
						{
							memcpy(ack_frame->buf,inbuf + 22,ack_frame->len);
						}
						else
						{
							DeletePlcFrame(ack_frame);
							ack_frame = NULL;
						}
					}
					else
					{
						DeletePlcFrame(ack_frame);
						ack_frame = NULL;
					}
				}
				else
				{
					DeletePlcFrame(ack_frame);
					ack_frame = NULL;
				}
			}
		break;

		default:
		break;
	}

	GET_OUT:
	xSemaphoreGive(xMutex_USART2);

	return ack_frame;
}

//解析用户数据
void AnalysisPlcUserFrame(PlcFrame_S *user_frame)
{
	switch(user_frame->cmd_code)
	{
		case 0x00:			//数据透传
			PlcTransparentTransmission(user_frame);
		break;

		case 0x70:			//状态查询
			PlcQueryLampState(user_frame);
		break;

		case 0x71:			//状态上报
			PlcQueryLampState(user_frame);
		break;

		case 0xA0:			//告警上报
			PlcReportLampAlarm(user_frame,1);
		break;

		case 0xA1:			//告警消除上报
			PlcReportLampAlarm(user_frame,0);
		break;

		case 0xA4:			//告警配置查询
			PlcGetLampAlarmConfig(user_frame);
		break;

		case 0xA5:			//告警历史查询

		break;

		case 0xD1:			//基础配置查询
			PlcGetLampBasicConfig(user_frame);
		break;

		case 0xD4:			//场景查询
			PlcGetLampAppointment(user_frame);
		break;

		case 0xD6:			//任务查询
			
		break;

		case 0xD8:			//节点搜索
			PlcGetLampBasicConfig(user_frame);
		break;

		case 0xF6:			//版本查询
			PlcGetLampFrameWareVersion(user_frame);
		break;

		case 0x80:			//设备ACK

		break;

		default:

		break;
	}
}

void PlcTransparentTransmission(PlcFrame_S *user_frame)
{
	u8 i = 0;
	char *buf = NULL;

	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	buf = (char *)pvPortMalloc(user_frame->len * 2 + 1);

	if(server_frame_struct != NULL && buf != NULL)
	{
		InitServerFrameStruct(server_frame_struct);

		server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
		server_frame_struct->msg_len 	= 10;
		server_frame_struct->err_code 	= (u8)NO_ERR;
		server_frame_struct->msg_id		= 0x0100 + user_frame->cmd_code;
		server_frame_struct->para_num 	= 1;

		server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

		if(server_frame_struct->para != NULL)
		{
			server_frame_struct->para[i].type = 0x4101;
			memset(buf,0,user_frame->len * 2 + 1);
			HexToStr(buf, user_frame->buf, user_frame->len);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			ConvertFrameStructToFrame(server_frame_struct);
		}
		else
		{
			DeleteServerFrameStruct(server_frame_struct);
		}
	}
	else
	{
		if(server_frame_struct != NULL)
		{
			DeleteServerFrameStruct(server_frame_struct);
		}

		if(buf != NULL)
		{
			vPortFree(buf);
		}
	}
}

void PlcQueryLampState(PlcFrame_S *user_frame)
{
	LampState_S *lamp_state = NULL;

	if(user_frame->len == sizeof(LampState_S))
	{
		lamp_state = (LampState_S *)pvPortMalloc(user_frame->len);

		if(lamp_state != NULL)
		{
			memcpy(lamp_state,user_frame->buf,user_frame->len);

			if(xQueueSend(xQueue_LampState,(void *)&lamp_state,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_LampState fail.\r\n");
#endif
				vPortFree(lamp_state);
			}
		}
	}
}

void PlcReportLampAlarm(PlcFrame_S *user_frame,u8 record_type)
{
	u8 alarm_type = 0;

	alarm_type = *(user_frame->buf + 0);

	switch(alarm_type)
	{
		case LAMP_LAMP_FAULT_ALARM:
			LampLamp_Power_CapacitorFaultAlarm(user_frame,record_type);
		break;

		case LAMP_POWER_MODULE_FAULT_ALARM:
			LampLamp_Power_CapacitorFaultAlarm(user_frame,record_type);
		break;

		case LAMP_CAPACITOR_FAULT_ALARM:
			LampLamp_Power_CapacitorFaultAlarm(user_frame,record_type);
		break;

		case LMAP_RELAY_FAULT_ALARM:
			LampRelayFaultAlarm(user_frame,record_type);
		break;

		case LAMP_OVRE_LOW_TEMPERATURE_ALARM:
			LampOverLowTemperatureAlarm(user_frame,record_type);
		break;

		case LAMP_LEAKAGE_ALARM:
			LampLeakageAlarm(user_frame,record_type);
		break;

		case LAMP_GATE_MAGNETISM_ALARM:
			LampGateMagnetismAlarm(user_frame,record_type);
		break;

		case LAMP_POLE_TILT_ALARM:
			LampPoleTiltAlarm(user_frame,record_type);
		break;

		case LAMP_ELECTRICAL_PARA_OVER_THRE_ALARM:
			LampElectricalParaOverThreAlarm(user_frame,record_type);
		break;

		case LAMP_ABNORMAL_LIGHT_ON_ALARM:
			LampAbnormalLightOnOffAlarm(user_frame,record_type);
		break;

		case LAMP_ABNORMAL_LIGHT_OFF_ALARM:
			LampAbnormalLightOnOffAlarm(user_frame,record_type);
		break;

		case LAMP_LIGHT_ON_ABNORMAL_ALARM:
			LampLightOnAbnormalAlarm(user_frame,record_type);
		break;

		case LAMP_TASK_STATE_ABNORMAL_ALARM:
			LampTaskStateAbnormalAlarm(user_frame,record_type);
		break;

		default:
		break;
	}
}

void PlcGetLampAlarmConfig(PlcFrame_S *user_frame)
{
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	char tmp[10] = {0};
	char buf[150] = {0};
	LampAlarmConf_S *lamp_alarm_conf = NULL;

	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	if(user_frame->len != sizeof(LampAlarmConf_S))
	{
		return;
	}

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL)
	{
		InitServerFrameStruct(server_frame_struct);

		server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
		server_frame_struct->msg_len 	= 10;
		server_frame_struct->err_code 	= (u8)NO_ERR;
		server_frame_struct->msg_id		= 0x0100 + user_frame->cmd_code;
		server_frame_struct->para_num 	= 22;

		server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

		if(server_frame_struct->para != NULL)
		{
			lamp_alarm_conf = (LampAlarmConf_S *)user_frame->buf;

			server_frame_struct->para[i].type = 0x4101;
			memset(buf,0,25);
			sprintf(buf, "%08X",user_frame->address);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8102;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->lamp_fault_alarm_enable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8103;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->power_module_fault_alarm_enable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8104;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->capacitor_fault_alarm_enable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8105;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->relay_fault_alarm_enable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8106;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->temperature_alarm_enable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x4107;
			memset(buf,0,25);
			sprintf(tmp, "%d",lamp_alarm_conf->temperature_alarm_low_thre);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->temperature_alarm_high_thre);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->temperature_alarm_duration);
			strcat(buf,tmp);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8108;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->leakage_alarm_enable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x4109;
			memset(buf,0,25);
			sprintf(tmp, "%d",lamp_alarm_conf->leakage_alarm_c_thre);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->leakage_alarm_v_thre);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->leakage_alarm_duration);
			strcat(buf,tmp);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x810A;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->gate_magnetism_alarm_enable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x810B;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->gate_magnetism_alarm_type);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x810C;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->post_tilt_alarm_enable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x410D;
			memset(buf,0,25);
			sprintf(tmp, "%d",lamp_alarm_conf->post_tilt_alarm_thre);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->post_tilt_alarm_duration);
			strcat(buf,tmp);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x810E;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->electrical_para_alarm_enable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x410F;
			memset(buf,0,150);
			for(j = 0; j < MAX_LAMP_CH_NUM; j ++)
			{
				for(k = 0; k < MAX_LAMP_ALARM_E_PARA_NUM; k ++)
				{
					if(lamp_alarm_conf->electrical_para_alarm_thre[j][k].para_id != 0 &&
					   lamp_alarm_conf->electrical_para_alarm_thre[j][k].channel != 0)
					{
						sprintf(tmp, "%d",lamp_alarm_conf->electrical_para_alarm_thre[j][k].channel);
						strcat(buf,tmp);
						strcat(buf,",");
						sprintf(tmp, "%d",lamp_alarm_conf->electrical_para_alarm_thre[j][k].para_id);
						strcat(buf,tmp);
						strcat(buf,",");
						sprintf(tmp, "%d",lamp_alarm_conf->electrical_para_alarm_thre[j][k].min_value);
						strcat(buf,tmp);
						strcat(buf,",");
						sprintf(tmp, "%d",lamp_alarm_conf->electrical_para_alarm_thre[j][k].min_range);
						strcat(buf,tmp);
						strcat(buf,",");
						sprintf(tmp, "%d",lamp_alarm_conf->electrical_para_alarm_thre[j][k].max_value);
						strcat(buf,tmp);
						strcat(buf,",");
						sprintf(tmp, "%d",lamp_alarm_conf->electrical_para_alarm_thre[j][k].max_range);
						strcat(buf,tmp);
						strcat(buf,",");
						sprintf(tmp, "%d",lamp_alarm_conf->electrical_para_alarm_thre[j][k].duration_time);
						strcat(buf,tmp);
						strcat(buf,"|");
					}
				}
			}
			buf[strlen(buf) -1] = 0;
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8110;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->abnormal_light_on_alarm_enable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x4111;
			memset(buf,0,25);
			sprintf(tmp, "%d",lamp_alarm_conf->abnormal_light_on_alarm_c_thre);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->abnormal_light_on_alarm_p_thre);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->abnormal_light_on_alarm_duration);
			strcat(buf,tmp);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8112;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->abnormal_light_off_alarm_enable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x4103;
			memset(buf,0,25);
			sprintf(tmp, "%d",lamp_alarm_conf->abnormal_light_off_alarm_c_thre);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->abnormal_light_off_alarm_p_thre);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->abnormal_light_off_alarm_duration);
			strcat(buf,tmp);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8114;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->light_on_fault_alarm_enable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x4115;
			memset(buf,0,50);
			sprintf(tmp, "%d",lamp_alarm_conf->light_on_fault_alarm_rated_power[0]);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->light_on_fault_alarm_low_thre[0]);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->light_on_fault_alarm_high_thre[0]);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->light_on_fault_alarm_duration[0]);
			strcat(buf,tmp);
			strcat(buf,"|");
			sprintf(tmp, "%d",lamp_alarm_conf->light_on_fault_alarm_rated_power[1]);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->light_on_fault_alarm_low_thre[1]);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->light_on_fault_alarm_high_thre[1]);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_alarm_conf->light_on_fault_alarm_duration[1]);
			strcat(buf,tmp);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x8116;
			memset(buf,0,25);
			sprintf(buf, "%d",lamp_alarm_conf->task_light_state_fault_alarm_enhable);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			ConvertFrameStructToFrame(server_frame_struct);
		}
		else
		{
			DeleteServerFrameStruct(server_frame_struct);
		}
	}
}

void PlcGetLampBasicConfig(PlcFrame_S *user_frame)
{
	u8 i = 0;
	u8 j = 0;
	char tmp[10] = {0};
	char buf[100] = {0};
	LampConfig_S *lamp_conf = NULL;

	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	if(user_frame->len != sizeof(LampConfig_S))
	{
		return;
	}

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL)
	{
		InitServerFrameStruct(server_frame_struct);

		server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
		server_frame_struct->msg_len 	= 10;
		server_frame_struct->err_code 	= (u8)NO_ERR;
		server_frame_struct->msg_id		= 0x0100 + user_frame->cmd_code;
		server_frame_struct->para_num 	= 2;

		server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

		if(server_frame_struct->para != NULL)
		{
			lamp_conf = (LampConfig_S *)user_frame->buf;

			server_frame_struct->para[i].type = 0x4101;
			memset(buf,0,25);
			sprintf(buf, "%08X",user_frame->address);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x4102;
			memset(buf,0,100);
			sprintf(tmp, "%08X",lamp_conf->address);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_conf->advance_time);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_conf->delay_time);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%f",lamp_conf->longitude);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%f",lamp_conf->latitude);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_conf->light_wane);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",lamp_conf->auto_report);
			strcat(buf,tmp);
			strcat(buf,"|");
			sprintf(tmp, "%d",lamp_conf->adjust_type);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",((lamp_conf->default_brightness >> 4) & 0x0F) * 10);
			strcat(buf,tmp);
			strcat(buf,",");
			sprintf(tmp, "%d",(lamp_conf->default_brightness & 0x0F) * 10);
			strcat(buf,tmp);
			strcat(buf,"|");
			for(j = 0; j < MAX_LAMP_GROUP_NUM; j ++)
			{
				if(lamp_conf->group[j] != 0)
				{
					sprintf(tmp, "%d",lamp_conf->group[j]);
					strcat(buf,tmp);
					strcat(buf,",");
				}
			}
			buf[strlen(buf) - 1] = 0;
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			ConvertFrameStructToFrame(server_frame_struct);
		}
		else
		{
			DeleteServerFrameStruct(server_frame_struct);
		}
	}
}

void PlcGetLampAppointment(PlcFrame_S *user_frame)
{
	u8 i = 0;
	u8 j = 0;
	char tmp[10] = {0};
	char buf[256] = {0};
	LampSenceConfig_S *lamp_sence_conf = NULL;

	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	if(user_frame->len != sizeof(LampSenceConfig_S))
	{
		return;
	}

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL)
	{
		InitServerFrameStruct(server_frame_struct);

		server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
		server_frame_struct->msg_len 	= 10;
		server_frame_struct->err_code 	= (u8)NO_ERR;
		server_frame_struct->msg_id		= 0x0100 + user_frame->cmd_code;
		server_frame_struct->para_num 	= 1;

		server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

		if(server_frame_struct->para != NULL)
		{
			lamp_sence_conf = (LampSenceConfig_S *)user_frame->buf + 1;

			server_frame_struct->para[i].type = 0x4101;
			memset(buf,0,256);
			memset(tmp,0,10);
			sprintf(tmp, "%d",lamp_sence_conf->group_id);
			strcat(buf,tmp);
			strcat(buf,",");

			memset(tmp,0,10);
			sprintf(tmp, "%d",lamp_sence_conf->priority);
			strcat(buf,tmp);
			strcat(buf,"|");

			for(j = 0; j < lamp_sence_conf->time_range_num; j ++)
			{
				if(lamp_sence_conf->range[j].s_month  == 0 &&
				   lamp_sence_conf->range[j].s_date   == 0 &&
				   lamp_sence_conf->range[j].s_hour   == 0 &&
				   lamp_sence_conf->range[j].s_minute == 0 &&
				   lamp_sence_conf->range[j].e_month  == 0 &&
				   lamp_sence_conf->range[j].e_date   == 0 &&
				   lamp_sence_conf->range[j].e_hour   == 0 &&
				   lamp_sence_conf->range[j].e_minute == 0)
				{
					strcat(buf,"0,0");
				}
				else
				{
					memset(tmp,0,10);
					sprintf(tmp, "%02d",lamp_sence_conf->range[j].s_month);
					strcat(buf,tmp);
					memset(tmp,0,10);
					sprintf(tmp, "%02d",lamp_sence_conf->range[j].s_date);
					strcat(buf,tmp);
					memset(tmp,0,10);
					sprintf(tmp, "%02d",lamp_sence_conf->range[j].s_hour);
					strcat(buf,tmp);
					memset(tmp,0,10);
					sprintf(tmp, "%02d",lamp_sence_conf->range[j].s_minute);
					strcat(buf,tmp);
					strcat(buf,",");
					memset(tmp,0,10);
					sprintf(tmp, "%02d",lamp_sence_conf->range[j].e_month);
					strcat(buf,tmp);
					memset(tmp,0,10);
					sprintf(tmp, "%02d",lamp_sence_conf->range[j].e_date);
					strcat(buf,tmp);
					memset(tmp,0,10);
					sprintf(tmp, "%02d",lamp_sence_conf->range[j].e_hour);
					strcat(buf,tmp);
					memset(tmp,0,10);
					sprintf(tmp, "%02d",lamp_sence_conf->range[j].e_minute);
					strcat(buf,tmp);
				}
				strcat(buf,",");
				memset(tmp,0,10);
				sprintf(tmp, "%02X",lamp_sence_conf->range[j].week_enable);
				strcat(buf,tmp);
				if(j < lamp_sence_conf->time_range_num - 1)
				{
					strcat(buf,"|");
				}
			}
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			ConvertFrameStructToFrame(server_frame_struct);
		}
		else
		{
			DeleteServerFrameStruct(server_frame_struct);
		}
	}
}

void PlcGetLampFrameWareVersion(PlcFrame_S *user_frame)
{
	u8 i = 0;
	u16 version = 0;
	char tmp[10] = {0};
	char buf[25] = {0};

	ServerFrameStruct_S *server_frame_struct = NULL;		//用于响应服务器

	if(user_frame->len != 2)
	{
		return;
	}

	server_frame_struct = (ServerFrameStruct_S *)pvPortMalloc(sizeof(ServerFrameStruct_S));

	if(server_frame_struct != NULL)
	{
		InitServerFrameStruct(server_frame_struct);

		server_frame_struct->msg_type 	= (u8)DEVICE_REQUEST_UP;	//响应服务器类型
		server_frame_struct->msg_len 	= 10;
		server_frame_struct->err_code 	= (u8)NO_ERR;
		server_frame_struct->msg_id		= 0x0100 + user_frame->cmd_code;
		server_frame_struct->para_num 	= 2;

		server_frame_struct->para = (Parameter_S *)pvPortMalloc(server_frame_struct->para_num * sizeof(Parameter_S));

		if(server_frame_struct->para != NULL)
		{
			version = ((((u16)(*(user_frame->buf + 0))) << 8) & 0xFF00) + *(user_frame->buf + 1);

			server_frame_struct->para[i].type = 0x3101;
			memset(buf,0,25);
			sprintf(buf, "%08X",user_frame->address);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			server_frame_struct->para[i].type = 0x4102;
			memset(buf,0,25);
			memset(tmp,0,10);
			sprintf(tmp, "%02d",version / 10);
			strcat(buf,tmp);
			strcat(buf,".");
			sprintf(tmp, "%02d",version % 10);
			strcat(buf,tmp);
			server_frame_struct->para[i].len = strlen(buf);
			server_frame_struct->para[i].value = (u8 *)pvPortMalloc((server_frame_struct->para[i].len + 1) * sizeof(u8));
			if(server_frame_struct->para[i].value != NULL)
			{
				memcpy(server_frame_struct->para[i].value,buf,server_frame_struct->para[i].len + 1);
			}
			i ++;

			ConvertFrameStructToFrame(server_frame_struct);
		}
		else
		{
			DeleteServerFrameStruct(server_frame_struct);
		}
	}
}



























































