#include "task_plc.h"
#include "delay.h"
#include "usart.h"
#include "common.h"
#include "lamp_comm.h"
#include "plc.h"
#include "history_record.h"





TaskHandle_t xHandleTaskPLC = NULL;
unsigned portBASE_TYPE SatckPLC;


void vTaskPLC(void *pvParameters)
{
	PlcFrame_S *user_frame = NULL;

	while(1)
	{
		RecvAndExecuteLampPlcExecuteTask();						//处理lamp任务发来的任务

		if(user_frame == NULL)
		{
			user_frame = RecvAndAnalysisPlcBottomFrame(1);		//接收并解析PLC底层数据，将用户数据取出
		}

		if(user_frame != NULL)
		{
			AnalysisPlcUserFrame(user_frame);					//解析PLC用户数据

			DeletePlcFrame(user_frame);
			user_frame = NULL;
		}

		delay_ms(200);

		SatckPLC = uxTaskGetStackHighWaterMark(NULL);
	}
}

void RecvAndExecuteLampPlcExecuteTask(void)
{
	static time_t time_sec = 0;
	u8 ret = 0;
	u8 i = 0;
	u8 j = 0;
	u8 k = 0;
	u8 got_frame = 0;
	BaseType_t xResult;
	LampConfig_S lamp_config;
	LampPlcExecuteTaskState_S task_state;

	static LampPlcExecuteTask_S *task_progress = NULL;
	static LampPlcExecuteTask_S *task_backup = NULL;
	PlcFrame_S frame;

	if(task_progress == NULL)
	{
		xResult = xQueueReceive(xQueue_LampPlcExecuteTaskToPlc,(void *)&task_progress,(TickType_t)pdMS_TO_TICKS(1));

		if(xResult == pdPASS)
		{
			time_sec = GetSysTick1s();			//记录开始时间

			if(task_progress->notify_enable == 1)
			{
				LampPlcExecuteTask_S *task_notify = NULL;

				task_notify = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

				if(task_notify != NULL)
				{
					ret = CopyLampPlcExecuteTask(task_notify,task_progress);

					if(ret == 1)
					{
						if(xQueueSend(xQueue_LampPlcExecuteTaskFromPlc,(void *)&task_notify,(TickType_t)10) != pdPASS)
						{
#ifdef DEBUG_LOG
							printf("send xQueue_LampPlcExecuteTaskFromPlc fail.\r\n");
#endif
							DeleteLampPlcExecuteTask(task_notify);
						}
					}
					else
					{
						DeleteLampPlcExecuteTask(task_notify);
						task_notify = NULL;
					}
				}
			}
		}
	}

	xResult = xQueueReceive(xQueue_LampPlcExecuteTaskState,(void *)&task_state,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS)
	{
		switch(task_state.state)
		{
			case (u8)STATE_START:
				if(task_progress == NULL)
				{
					if(task_state.cmd_code == task_backup->cmd_code)
					{
						task_progress = task_backup;

						task_backup = NULL;
					}
				}
			break;

			case (u8)STATE_STOP:
				if(task_progress != NULL)
				{
					if(task_state.cmd_code == task_progress->cmd_code)
					{
						task_progress->state = STATE_FINISHED;

						goto FINISHED;
					}
				}
			break;

			case (u8)STATE_SUSPEND:
				if(task_progress != NULL)
				{
					if(task_state.cmd_code == task_progress->cmd_code)
					{
						if(task_backup != NULL)
						{
							DeleteLampPlcExecuteTask(task_backup);

							task_backup = NULL;
						}

						if(task_backup == NULL)
						{
							task_backup = task_progress;

							task_progress = NULL;

							goto FINISHED;
						}
					}
				}
			break;

			default:
			break;
		}
	}

	if(task_progress != NULL)
	{
		if(task_progress->broadcast_type == 0)
		{
			if(task_progress->execute_type == 0)
			{
				memset(&frame,0,sizeof(PlcFrame_S));

				frame.address = 0xFFFF;						//直接广播
				frame.type = 0;
				frame.wait_ack = 0;
				frame.resp_ack = 0;
				task_progress->success_num = task_progress->dev_num;
				task_progress->failed_num = task_progress->dev_num;
				task_progress->executed_num ++;

				if(task_progress->executed_num <= task_progress->execute_total_num)
				{
					got_frame = CombinePlcUserFrame(task_progress,&frame);
				}
				else
				{
					task_progress->state = STATE_FINISHED;
				}
			}
			else
			{
				if(task_progress->cmd_code <= 0x010F && task_progress->broadcast_state == 0)
				{
					task_progress->broadcast_state = 1;

					memset(&frame,0,sizeof(PlcFrame_S));

					frame.address = 0xFFFF;					//先执行广播
					frame.type = 0;
					frame.wait_ack = 0;
					frame.resp_ack = 0;

					got_frame = CombinePlcUserFrame(task_progress,&frame);
				}

				READ_DEVICE_CONF1:
				ret = ReadSpecifyLampNumList(task_progress->executed_index);

				if(ret == 1)
				{
					memset(&lamp_config,0,sizeof(LampConfig_S));

					ret = ReadLampConfig(task_progress->executed_index,&lamp_config);

					if(ret == 1)
					{
						memset(&frame,0,sizeof(PlcFrame_S));

						frame.address = lamp_config.address;		//单播
						frame.type = 1;
						frame.wait_ack = 1;
						frame.resp_ack = task_progress->execute_type - 1;
						task_progress->executed_num ++;

						if(task_progress->executed_num <= task_progress->execute_total_num)
						{
							got_frame = CombinePlcUserFrame(task_progress,&frame);
						}
						else
						{
							task_progress->state = STATE_FINISHED;
						}
					}
				}

				task_progress->executed_index ++;

				if(task_progress->executed_index <= MAX_LAMP_CONF_NUM)
				{
					if(got_frame == 0)
					{
						goto READ_DEVICE_CONF1;
					}
				}
				else
				{
					task_progress->state = STATE_FINISHED;
				}
			}
		}
		else if(task_progress->broadcast_type == 1)
		{
			if(task_progress->execute_type == 0)
			{
				memset(&frame,0,sizeof(PlcFrame_S));						//直接组播

				frame.address = 0xFFFF;
				frame.type = 0;
				frame.group_num = task_progress->group_num;
				for(i = 0; i < task_progress->group_num; i ++)
				{
					frame.group_id[i] = (u8)task_progress->group_dev_id[i];
				}
				frame.wait_ack = 0;
				frame.resp_ack = 0;
				task_progress->success_num = task_progress->dev_num;
				task_progress->failed_num = task_progress->dev_num;
				task_progress->executed_num ++;

				if(task_progress->executed_num <= task_progress->execute_total_num)
				{
					got_frame = CombinePlcUserFrame(task_progress,&frame);
				}
				else
				{
					task_progress->state = STATE_FINISHED;
				}
			}
			else
			{
				if(task_progress->cmd_code <= 0x010F && task_progress->broadcast_state == 0)
				{
					task_progress->broadcast_state = 1;

					memset(&frame,0,sizeof(PlcFrame_S));						//先组播

					frame.address = 0xFFFF;
					frame.type = 0;
					frame.group_num = task_progress->group_num;
					for(i = 0; i < task_progress->group_num; i ++)
					{
						frame.group_id[i] = (u8)task_progress->group_dev_id[i];
					}
					frame.wait_ack = 0;
					frame.resp_ack = 0;

					got_frame = CombinePlcUserFrame(task_progress,&frame);
				}

				READ_DEVICE_CONF2:
				ret = ReadSpecifyLampNumList(task_progress->executed_index);

				if(ret == 1)
				{
					memset(&lamp_config,0,sizeof(LampConfig_S));

					ret = ReadLampConfig(task_progress->executed_index,&lamp_config);

					if(ret == 1)
					{
						for(j = 0; j < MAX_LAMP_GROUP_NUM; j ++)
						{
							for(k = 0; k < MAX_LAMP_GROUP_NUM; k ++)
							{
								if(lamp_config.group[j] == task_progress->group_dev_id[k])
								{
									memset(&frame,0,sizeof(PlcFrame_S));

									frame.address = lamp_config.address;		//单播
									frame.type = 1;
									frame.wait_ack = 1;
									frame.resp_ack = task_progress->execute_type - 1;
									task_progress->executed_num ++;

									if(task_progress->executed_num <= task_progress->execute_total_num)
									{
										got_frame = CombinePlcUserFrame(task_progress,&frame);
									}
									else
									{
										task_progress->state = STATE_FINISHED;
									}
								}
							}
						}
					}
				}

				task_progress->executed_index ++;

				if(task_progress->executed_index <= MAX_LAMP_CONF_NUM)
				{
					if(got_frame == 0)
					{
						goto READ_DEVICE_CONF2;
					}
				}
				else
				{
					task_progress->state = STATE_FINISHED;
				}
			}
		}
		else if(task_progress->broadcast_type == 2)
		{
			if(task_progress->group_dev_id[task_progress->executed_index] != 0)
			{
				memset(&frame,0,sizeof(PlcFrame_S));

				frame.address = task_progress->group_dev_id[task_progress->executed_index];
				frame.type = 1;
				frame.wait_ack = 1;
				frame.resp_ack = task_progress->execute_type - 1;
				task_progress->executed_num ++;

				if(task_progress->executed_num <= task_progress->execute_total_num)
				{
					got_frame = CombinePlcUserFrame(task_progress,&frame);
				}
				else
				{
					task_progress->state = STATE_FINISHED;
				}
			}

			task_progress->executed_index ++;

			if(task_progress->executed_index >= task_progress->dev_num)
			{
				if(task_progress->executed_num < task_progress->execute_total_num)
				{
					task_progress->executed_index = 0;
				}
				else
				{
					task_progress->state = STATE_FINISHED;
				}
			}
		}

		if(got_frame == 1)
		{
			task_progress->state = SendPlcFrameToDeviceAndWaitResponse(frame);		//向PLC发送数据并等待返回
		}

		if(frame.buf != NULL)
		{
			vPortFree(frame.buf);
			frame.buf = NULL;
		}

		if(task_progress->state >= STATE_SUCCESS && task_progress->state <= STATE_TIMEOUT)
		{
			if(task_progress->state == STATE_SUCCESS)			//执行成功
			{
				task_progress->success_num ++;
			}
			else if(task_progress->state == STATE_FAIL)			//执行失败
			{
				task_progress->failed_num ++;
			}
			else if(task_progress->state == STATE_TIMEOUT)		//执行超时
			{
				task_progress->failed_num ++;
				task_progress->timeout_num ++;
			}

			task_progress->state = STATE_START;

			if(task_progress->notify_enable == 1)
			{
				LampPlcExecuteTask_S *task_notify = NULL;

				task_notify = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

				if(task_notify != NULL)
				{
					ret = CopyLampPlcExecuteTask(task_notify,task_progress);

					if(ret == 1)
					{
						if(xQueueSend(xQueue_LampPlcExecuteTaskFromPlc,(void *)&task_notify,(TickType_t)10) != pdPASS)
						{
#ifdef DEBUG_LOG
							printf("send xQueue_LampPlcExecuteTaskFromPlc fail.\r\n");
#endif
							DeleteLampPlcExecuteTask(task_notify);
						}
					}
					else
					{
						DeleteLampPlcExecuteTask(task_notify);
						task_notify = NULL;
					}
				}
			}
		}
		else if(task_progress->state == STATE_FINISHED)
		{
			FINISHED:
			if(frame.buf != NULL)
			{
				vPortFree(frame.buf);
				frame.buf = NULL;
			}

			task_progress->spent_time = GetSysTick1s() - time_sec;		//计算总耗时

			if(task_progress->notify_enable == 1)
			{
				if(xQueueSend(xQueue_LampPlcExecuteTaskFromPlc,(void *)&task_progress,(TickType_t)10) != pdPASS)
				{
#ifdef DEBUG_LOG
					printf("send xQueue_LampPlcExecuteTaskFromPlc fail.\r\n");
#endif
					DeleteLampPlcExecuteTask(task_progress);
					task_progress = NULL;
				}
			}
			else
			{
				DeleteLampPlcExecuteTask(task_progress);
				task_progress = NULL;
			}
		}
	}
}

u8 CombinePlcUserFrame(LampPlcExecuteTask_S *task,PlcFrame_S *frame)
{
	u8 ret = 1;
	u8 buf[256] = {0};
	LampStrategyGroupSwitch_S *strategy_switch = NULL;

	if(frame->buf != NULL)
	{
		return 0;
	}

	frame->cmd_code = (u8)(task->cmd_code - 0x0100);

	switch(task->cmd_code)
	{
		case 0x0100:	//透传

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
				memcpy(frame->buf,task->data,frame->len);
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

			frame->len = 1 + strategy_switch->group_num + 1 + 6;

			buf[0] = strategy_switch->group_num;
			memcpy(&buf[1],strategy_switch->group_id,strategy_switch->group_num);
			buf[1 + strategy_switch->group_num] = strategy_switch->type;
			buf[1 + strategy_switch->group_num + 1] = (strategy_switch->time[2] - 0x30) * 10 + (strategy_switch->time[3] - 0x30);
			buf[1 + strategy_switch->group_num + 2] = (strategy_switch->time[4] - 0x30) * 10 + (strategy_switch->time[5] - 0x30);
			buf[1 + strategy_switch->group_num + 3] = (strategy_switch->time[6] - 0x30) * 10 + (strategy_switch->time[7] - 0x30);
			buf[1 + strategy_switch->group_num + 4] = (strategy_switch->time[8] - 0x30) * 10 + (strategy_switch->time[9] - 0x30);
			buf[1 + strategy_switch->group_num + 5] = (strategy_switch->time[10] - 0x30) * 10 + (strategy_switch->time[11] - 0x30);
			buf[1 + strategy_switch->group_num + 6] = (strategy_switch->time[12] - 0x30) * 10 + (strategy_switch->time[13] - 0x30);

			frame->buf = (u8 *)pvPortMalloc(frame->len * sizeof(u8));

			if(frame->buf != NULL)
			{
				memcpy(frame->buf,buf,frame->len);
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

		case 0x0170:	//状态查询

		break;

		case 0x0172:	//状态历史查询（预留）

		break;

		case 0x01A3:	//告警配置

		break;

		case 0x01A4:	//告警配置查询

		break;

		case 0x01A5:	//告警历史查询（预留）

		break;

		case 0x01D2:	//重新写址

		break;

		case 0x01D7:	//基础/场景/任务配置同步

		break;

		case 0x01D8:	//节点搜索

		break;

		case 0x01F0:	//请求下载固件包

		break;

		case 0x01F1:	//下载固件包

		break;

		case 0x01F2:	//完成下载

		break;

		case 0x01F3:	//开始升级

		break;

		case 0x01F4:	//暂停升级

		break;

		case 0x01F5:	//停止升级

		break;

		case 0x01F6:	//版本查询

		break;

		default:

		break;
	}

	return ret;
}

u8 CombinePlcBottomFrame(PlcFrame_S *frame,u8 *outbuf)
{
	u8 out_len = 0;
	u8 address[6] = {0};

	if(frame->cmd_code == 0x00)
	{
		return 0;
	}

	if(frame->type == 0)	//广播
	{
		memset(address,0xFF,6);
	}
	else					//单播
	{
		address[4] = (u8)((frame->address >> 8) & 0x00FF);
		address[5] = (u8)(frame->address & 0x00FF);
	}

	out_len = plc_pack_user_data(address,frame->buf,frame->len,outbuf + 5);

	out_len = plc_combine_data(0x14,outbuf + 5,out_len,outbuf);

	return out_len;
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

	if(in_frame.type == 0)
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
	else if(in_frame.wait_ack == 1)	//单播并且需要等待响应
	{
		time_out = LampBasicConfig.response_timeout * 100;

		while(time_out)
		{
			time_out --;

			delay_ms(10);

			ack_frame = RecvAndAnalysisPlcBottomFrame(0);

			if(ack_frame != NULL)
			{
				if(ack_frame->address == in_frame.address)
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

void AnalysisPlcUserFrame(PlcFrame_S *user_frame)
{
	LampState_S *lamp_state = NULL;
	AlarmReport_S *lamp_alarm = NULL;

	u8 cmd_code = 0;

	switch(cmd_code)
	{
		case 0x00:

		break;

		case 0x80:

		break;

		default:

		break;
	}
}

//mode 0只解析不清空 1解析并清空
PlcFrame_S* RecvAndAnalysisPlcBottomFrame(u8 mode)
{
	PlcFrame_S *ack_frame = NULL;

	u8 *inbuf = NULL;
	u8 len = 0;
	u8 *msg = NULL;
	u8 cmd_id = 0;
	u8 check_sum_read = 0;
	u8 check_sum_cal = 0;
	u8 check_xor_read = 0;
	u8 check_xor_cal = 0;
	u16 frame_len = 0;
	u16 user_data_len = 0;
	u8 address[6] = {0};

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

	cmd_id = *(inbuf + 4);

	memcpy(address,inbuf + 8,6);

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

			msg = inbuf + 16;

			ack_frame = (PlcFrame_S *)pvPortMalloc(sizeof(PlcFrame_S));

			if(ack_frame != NULL)
			{
				ack_frame->type = 1;
				ack_frame->address = ((((u16)address[4]) << 8) & 0xFF00) + address[5];
				ack_frame->wait_ack = 1;
				ack_frame->resp_ack = 1;
				ack_frame->cmd_code = 0;

				ack_frame->buf = (u8 *)pvPortMalloc(user_data_len * sizeof(u8));

				if(ack_frame->buf != NULL)
				{
					ack_frame->len = (u8)user_data_len;
					memcpy(ack_frame->buf,msg,user_data_len);
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

u8 CopyLampPlcExecuteTask(LampPlcExecuteTask_S *d_task,LampPlcExecuteTask_S *s_task)
{
	u8 ret = 1;

	if(s_task == NULL || s_task == NULL)
	{
		return 0;
	}
	else
	{
		memset(d_task,0,sizeof(LampPlcExecuteTask_S));
	}

	memcpy(d_task,s_task,sizeof(LampPlcExecuteTask_S));

	d_task->data = NULL;

	d_task->data = (void *)pvPortMalloc(d_task->data_len * sizeof(u8));

	memcpy(d_task->data,s_task->data,d_task->data_len);

	return ret;
}

//释放PlcFrame结构体申请的内存
void DeletePlcFrame(PlcFrame_S *plc_frame)
{
	if(plc_frame != NULL)
	{
		if(plc_frame->buf != NULL)
		{
			vPortFree(plc_frame->buf);
			plc_frame->buf = NULL;
		}

		vPortFree(plc_frame);
		plc_frame = NULL;
	}
}

//释放LampPlcExecuteTask结构体申请的内存
void DeleteLampPlcExecuteTask(LampPlcExecuteTask_S *task)
{
	if(task != NULL)
	{
		if(task->data != NULL)
		{
			vPortFree(task->data);
			task->data = NULL;
		}

		vPortFree(task);
		task = NULL;
	}
}

































