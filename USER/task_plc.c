#include "task_plc.h"
#include "delay.h"
#include "usart.h"
#include "common.h"
#include "plc_protocol.h"





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
		else
		{
			if(Usart2RecvEnd != 0x00 && Usart2FrameLen != 0x00)
			{
				TransTransmissionFrame_S  *trans_trans_frame = NULL;
			
				trans_trans_frame = (TransTransmissionFrame_S *)pvPortMalloc(sizeof(TransTransmissionFrame_S));
				
				if(trans_trans_frame != NULL)
				{
					trans_trans_frame->device_type = RELAY;
					
					trans_trans_frame->address = 0;
					trans_trans_frame->channel = 0;
					trans_trans_frame->trans_moudle = 1;
					
					trans_trans_frame->len = Usart2FrameLen;
					
					trans_trans_frame->buf = (u8 *)pvPortMalloc(sizeof(u8) * Usart2FrameLen);
					
					if(trans_trans_frame->buf != NULL)
					{
						memcpy(trans_trans_frame->buf,Usart2RxBuf,Usart2FrameLen);
						
						if(xQueueSend(xQueue_TransTransFrame,(void *)&trans_trans_frame,(TickType_t)10) != pdPASS)
						{
	#ifdef DEBUG_LOG
							printf("send xQueue_TransTransFrame fail.\r\n");
	#endif
							DeleteTransTransmissionFrame(trans_trans_frame);
						}
					}
					else
					{
						DeleteTransTransmissionFrame(trans_trans_frame);
					}
				}
			}
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
					memset(task_notify,0,sizeof(LampPlcExecuteTask_S));

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
		frame.buf = NULL;

		if(task_progress->cmd_code == 0x01D5)
		{
			if(task_progress->executed_num < task_progress->execute_total_num)
			{
				got_frame = CombinePlcUserFrame(task_progress,&frame,lamp_config);
			}
			else
			{
				goto FINISHED;
			}
		}
		else
		{
			if(task_progress->broadcast_type == 0)
			{
				if(task_progress->execute_type == 0)
				{
					memset(&frame,0,sizeof(PlcFrame_S));

					frame.address = 0xFFFFFFFF;						//直接广播
					frame.type = task_progress->broadcast_type;
					frame.wait_ack = 0;
					frame.resp_ack = 0;
					task_progress->success_num = task_progress->dev_num;
					task_progress->failed_num = 0;

					if(task_progress->executed_num < task_progress->execute_total_num)
					{
						got_frame = CombinePlcUserFrame(task_progress,&frame,lamp_config);
					}
					else
					{
						goto FINISHED;
					}

					task_progress->executed_num ++;
				}
				else
				{
					if(task_progress->cmd_code <= 0x010F && task_progress->broadcast_state == 0)
					{
						task_progress->broadcast_state = 1;

						memset(&frame,0,sizeof(PlcFrame_S));

						frame.address = 0xFFFFFFFF;					//先执行广播
						frame.type = task_progress->broadcast_type;
						frame.wait_ack = 0;
						frame.resp_ack = 0;

						got_frame = CombinePlcUserFrame(task_progress,&frame,lamp_config);
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
							frame.type = 2;
							frame.wait_ack = 1;
							frame.resp_ack = task_progress->execute_type - 1;

							if(task_progress->executed_num < task_progress->execute_total_num)
							{
								got_frame = CombinePlcUserFrame(task_progress,&frame,lamp_config);
							}
							else
							{
								goto FINISHED;
							}

							task_progress->executed_num ++;
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
						goto FINISHED;
					}
				}
			}
			else if(task_progress->broadcast_type == 1)
			{
				if(task_progress->execute_type == 0)
				{
					memset(&frame,0,sizeof(PlcFrame_S));						//直接组播

					frame.address = 0xFFFFFFFF;
					frame.type = task_progress->broadcast_type;
					frame.group_num = task_progress->group_num;
					for(i = 0; i < task_progress->group_num; i ++)
					{
						frame.group_id[i] = (u8)task_progress->group_dev_id[i];
					}
					frame.wait_ack = 0;
					frame.resp_ack = 0;
					task_progress->success_num = task_progress->dev_num;
					task_progress->failed_num = 0;

					if(task_progress->executed_num < task_progress->execute_total_num)
					{
						got_frame = CombinePlcUserFrame(task_progress,&frame,lamp_config);
					}
					else
					{
						goto FINISHED;
					}

					task_progress->executed_num ++;
				}
				else
				{
					if(task_progress->cmd_code <= 0x010F && task_progress->broadcast_state == 0)
					{
						task_progress->broadcast_state = 1;

						memset(&frame,0,sizeof(PlcFrame_S));						//先组播

						frame.address = 0xFFFFFFFF;
						frame.type = task_progress->broadcast_type;
						frame.group_num = task_progress->group_num;
						for(i = 0; i < task_progress->group_num; i ++)
						{
							frame.group_id[i] = (u8)task_progress->group_dev_id[i];
						}
						frame.wait_ack = 0;
						frame.resp_ack = 0;

						got_frame = CombinePlcUserFrame(task_progress,&frame,lamp_config);
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
										frame.type = 2;
										frame.wait_ack = 1;
										frame.resp_ack = task_progress->execute_type - 1;

										if(task_progress->executed_num < task_progress->execute_total_num)
										{
											got_frame = CombinePlcUserFrame(task_progress,&frame,lamp_config);
										}
										else
										{
											goto FINISHED;
										}

										task_progress->executed_num ++;

										k = MAX_LAMP_GROUP_NUM;
										j = MAX_LAMP_GROUP_NUM;
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
						goto FINISHED;
					}
				}
			}
			else if(task_progress->broadcast_type == 2)
			{
				if(task_progress->group_dev_id[task_progress->executed_index] != 0)
				{
					memset(&frame,0,sizeof(PlcFrame_S));

					frame.address = task_progress->group_dev_id[task_progress->executed_index];
					frame.type = task_progress->broadcast_type;
					frame.wait_ack = 1;
					frame.resp_ack = task_progress->execute_type - 1;

					if(task_progress->executed_num < task_progress->execute_total_num)
					{
						got_frame = CombinePlcUserFrame(task_progress,&frame,lamp_config);
					}
					else
					{
						goto FINISHED;
					}

					task_progress->executed_num ++;
				}
				else
				{
					goto FINISHED;
				}

				task_progress->executed_index ++;

				if(task_progress->executed_index > task_progress->dev_num)
				{
					if(task_progress->executed_num < task_progress->execute_total_num)
					{
						task_progress->executed_index = 0;
					}
					else
					{
						goto FINISHED;
					}
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
			if(task_progress->execute_type == 1 || task_progress->execute_type == 2)
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
			}

			task_progress->state = STATE_START;

			if(task_progress->notify_enable == 1)
			{
				LampPlcExecuteTask_S *task_notify = NULL;

				task_notify = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

				if(task_notify != NULL)
				{
					memset(task_notify,0,sizeof(LampPlcExecuteTask_S));

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

		if(task_progress->state == STATE_FINISHED)
		{
			FINISHED:
			if(frame.buf != NULL)
			{
				vPortFree(frame.buf);
				frame.buf = NULL;
			}

			task_progress->state = STATE_FINISHED;

			task_progress->spent_time = GetSysTick1s() - time_sec;		//计算总耗时

			if(task_progress->notify_enable == 1)
			{
				LampPlcExecuteTask_S *task_notify = NULL;

				task_notify = (LampPlcExecuteTask_S *)pvPortMalloc(sizeof(LampPlcExecuteTask_S));

				if(task_notify != NULL)
				{
					memset(task_notify,0,sizeof(LampPlcExecuteTask_S));

					ret = CopyLampPlcExecuteTask(task_notify,task_progress);

					if(ret == 1)
					{
						if(xQueueSend(xQueue_LampPlcExecuteTaskFromPlc,(void *)&task_notify,(TickType_t)10) != pdPASS)
						{
#ifdef DEBUG_LOG
							printf("send xQueue_LampPlcExecuteTaskFromPlc fail.\r\n");
#endif
							DeleteLampPlcExecuteTask(task_notify);
							task_notify = NULL;
						}
					}
				}

				DeleteLampPlcExecuteTask(task_progress);
				task_progress = NULL;
			}
			else
			{
				DeleteLampPlcExecuteTask(task_progress);
				task_progress = NULL;
			}
		}
	}
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

































