#include "task_lumeter.h"
#include "lumeter_comm.h"
#include "lumeter_event.h"
#include "lumeter.h"



TaskHandle_t xHandleTaskLUMETER = NULL;
unsigned portBASE_TYPE SatckLUMETER;

void vTaskLUMETER(void *pvParameters)
{
	while(1)
	{
		LumeterRecvAndHandleFrameStruct();			//报文解析

		LumeterEventCheckPolling();					//告警等事件轮训

		LumeterCollectCurrentState();				//采集输入量采集模块状态

		LumeterRecvAndHandleDeviceFrame();			//接收并处理外置输入量采集模块返回的数据

		LumeterGetAppValue();						//获取参与逻辑运算的光照值

		delay_ms(100);

		SatckLUMETER = uxTaskGetStackHighWaterMark(NULL);
	}
}


void LumeterGetAppValue(void)
{
	u8 i = 0;
	u32 sum = 0;
	static time_t time_s = 0;

	if(LumeterBasicConfig.data_source == 0)
	{
		if(GetSysTick10ms() - time_s >= (LumeterBasicConfig.collect_cycle / 10))
		{
			time_s = GetSysTick10ms();

			for(i = 0; i < LumeterConfigNum.number; i ++)
			{
				sum += LumeterState[i].value;
			}

			LumeterAppValue = sum / LumeterConfigNum.number;
		}
	}
	else
	{
		LumeterAppValue = LumeterValueFromServer;
	}
}

//采集光照计状态
void LumeterCollectCurrentState(void)
{
	u8 i = 0;
	static time_t time_s = 0;

	if(LumeterBasicConfig.collect_cycle != 0)
	{
		if(GetSysTick10ms() - time_s >= (LumeterBasicConfig.collect_cycle / 10))
		{
			time_s = GetSysTick10ms();

			for(i = 0; i < LumeterConfigNum.number; i ++)
			{
				GetBuiltOutLumeterState(LumeterConfig[i]);
			}
		}
	}
}

void LumeterPushNewDateToStack(u32 *stack,u8 depth,u32 new_data,u8 *mode)
{
	u8 j = 0;

	if(*mode == 1)
	{
		*mode = 0;

		for(j = 0; j < depth; j ++)
		{
			*(stack + j) = new_data;
		}
	}
	else
	{
		for(j = 1; j < LumeterBasicConfig.stack_depth; j ++)
		{
			*(stack + j - 1) = *(stack + j);
		}

		*(stack + depth - 1) = new_data;
	}
}

u32 LumeterGetAverageValue(u32 *stack,u8 depth,u8 ignore_num)
{
	u32 sum = 0;
	u32 average_value = 0;
	u8 k = 0;
	u8 head = 0;
	u8 range = 0;

	head = ignore_num;
	range = depth - (ignore_num * 2);

	for(k = 0; k < range; k ++)
	{
		sum += *(stack + head + k);
	}

	average_value = sum / range;

	return average_value;
}

//接收并处理外置输入量采集模块返回的数据
void LumeterRecvAndHandleDeviceFrame(void)
{
	u8 ret = 0;
	u8 i = 0;
	u32 *sort_buf = NULL;
	Rs485Frame_S *recv_rs485_frame = NULL;
	BaseType_t xResult;
	LumeterCollectState_S collect_state;
	static u8 init_flag[MAX_LUMETER_CONF_NUM] = {1,1,1,1};

	xResult = xQueueReceive(xQueue_LumeterRs485Frame,(void *)&recv_rs485_frame,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS)
	{
		ret = AnalysisBuiltOutLumeterFrame(recv_rs485_frame->buf,recv_rs485_frame->len,&collect_state);

		if(ret == 1)
		{
			if(collect_state.update == 1)	//外置继电器模块采集状态有更新
			{
				sort_buf = (u32 *)pvPortMalloc(LumeterBasicConfig.stack_depth * sizeof(u32));

				if(sort_buf == NULL)
				{
					return;
				}

				collect_state.update = 0;

				for(i = 0; i < LumeterConfigNum.number; i ++)
				{
					if(LumeterState[i].address == collect_state.address &&
					   LumeterState[i].channel == collect_state.channel)
					{
						memset(sort_buf,0,LumeterBasicConfig.stack_depth * sizeof(u32));

						LumeterState[i].waitting_resp = 0;									//复位等待状态
						LumeterState[i].no_resp_time = 0;									//复位无响应次数

						LumeterPushNewDateToStack(LumeterState[i].stack,
												  LumeterBasicConfig.stack_depth,
												  collect_state.value,
												  &init_flag[i]);							//入栈

						memcpy(sort_buf,LumeterState[i].stack,LumeterBasicConfig.stack_depth * sizeof(u32));

						bubbleSort(sort_buf,LumeterBasicConfig.stack_depth);				//排序

						LumeterState[i].value = LumeterGetAverageValue(LumeterState[i].stack,
																	   LumeterBasicConfig.stack_depth,
																	   LumeterBasicConfig.ignore_num);		//忽略最大值和最小值并计算平均值

						if(LumeterState[i].collect_time < LumeterBasicConfig.stack_depth * 2)
						{
							LumeterState[i].collect_time ++;								//记录采集次数
						}
						else
						{
							LumeterState[i].ref_value = LumeterState[i].value;
							LumeterState[i].unchange_time = GetSysTick1s();
						}

						break;
					}
				}

				vPortFree(sort_buf);
			}
		}
		else
		{
			TransTransmissionFrame_S  *trans_trans_frame = NULL;

			trans_trans_frame = (TransTransmissionFrame_S *)pvPortMalloc(sizeof(TransTransmissionFrame_S));

			if(trans_trans_frame != NULL)
			{
				trans_trans_frame->device_type = LUMETER;

				trans_trans_frame->address = 0;
				trans_trans_frame->channel = 0;
				trans_trans_frame->trans_moudle = 1;

				trans_trans_frame->len = recv_rs485_frame->len;

				trans_trans_frame->buf = (u8 *)pvPortMalloc(sizeof(u8) * recv_rs485_frame->len);

				if(trans_trans_frame->buf != NULL)
				{
					memcpy(trans_trans_frame->buf,recv_rs485_frame->buf,recv_rs485_frame->len);

					if(xQueueSend(xQueue_TransTransFrame,(void *)&trans_trans_frame,(TickType_t)10) != pdPASS)
					{
#ifdef DEBUG_LOG
						printf("send xQueue_TransTransFrame fail 5.\r\n");
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

		DeleteRs485Frame(recv_rs485_frame);
	}
}





































