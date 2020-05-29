#include "task_rs485.h"
#include "delay.h"
#include "usart.h"
#include "common.h"





TaskHandle_t xHandleTaskRS485 = NULL;


void vTaskRS485(void *pvParameters)
{
	while(1)
	{
		RecvRs485FrameQueueAndSendToDeviceAndWaitResponse();

		delay_ms(200);
	}
}


//接收任务发来的rs485数据包并发送到相应的设备
void RecvRs485FrameQueueAndSendToDeviceAndWaitResponse(void)
{
	u16 time_out = 0;
	u8 re_send_times = 0;
	u8 responsed = 0;
	Rs485Frame_S *send_rs485_frame = NULL;
	Rs485Frame_S *resp_rs485_frame = NULL;
	QueueHandle_t xQueue_Rs485XxFrame = NULL;
	BaseType_t xResult;

	xResult = xQueueReceive(xQueue_Rs485Rs485Frame,(void *)&send_rs485_frame,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS)
	{
		re_send_times = 0;
		responsed = 0;

		RE_SEND:
		UsartSendString(UART5,send_rs485_frame->buf, send_rs485_frame->len);

		time_out = 100;

		while(time_out)
		{
			time_out --;

			delay_ms(10);

			if(Usart5RecvEnd == 0xAA)
			{
				Usart5RecvEnd = 0;

				if(Usart5FrameLen >= 8)
				{
					time_out = 0;
					responsed = 1;

					resp_rs485_frame = (Rs485Frame_S *)pvPortMalloc(sizeof(Rs485Frame_S));

					if(resp_rs485_frame != NULL)
					{
						resp_rs485_frame->device_type = send_rs485_frame->device_type;

						resp_rs485_frame->len = Usart5FrameLen;

						resp_rs485_frame->buf = (u8 *)pvPortMalloc(resp_rs485_frame->len * sizeof(u8));

						if(resp_rs485_frame->buf != NULL)
						{
							memcpy(resp_rs485_frame->buf,Usart5RxBuf,resp_rs485_frame->len);

							switch((u8)resp_rs485_frame->device_type)
							{
								case (u8)RELAY:						//继电器模块
									xQueue_Rs485XxFrame = xQueue_RelayRs485Frame;
								break;

								case (u8)INPUT_COLLECTOR:			//输入量采集模块
									xQueue_Rs485XxFrame = xQueue_InputCollectorRs485Frame;
								break;

								case (u8)ELECTRICITY_METER:			//电表
									xQueue_Rs485XxFrame = xQueue_ElectricityMeterRs485Frame;
								break;

								case (u8)LUMETER:					//光感传感器
									xQueue_Rs485XxFrame = xQueue_LumeterRs485Frame;
								break;

								default:
								break;
							}

							if(xQueueSend(xQueue_Rs485XxFrame,(void *)&resp_rs485_frame,(TickType_t)10) != pdPASS)
							{
#ifdef DEBUG_LOG
								printf("send xQueue_Rs485XxFrame fail.\r\n");
#endif
								DeleteRs485Frame(resp_rs485_frame);
							}
						}
						else
						{
							DeleteRs485Frame(resp_rs485_frame);
						}
					}

					Usart5FrameLen = 0;
				}
			}
		}

		if(responsed == 0)
		{
			re_send_times ++;

			if(re_send_times <= 2)
			{
				goto RE_SEND;
			}
		}

		DeleteRs485Frame(send_rs485_frame);
	}
}


//释放Rs485Frame结构体申请的内存
void DeleteRs485Frame(Rs485Frame_S *rs485_frame)
{
	if(rs485_frame != NULL)
	{
		if(rs485_frame->buf != NULL)
		{
			vPortFree(rs485_frame->buf);
			rs485_frame->buf = NULL;
		}

		vPortFree(rs485_frame);
		rs485_frame = NULL;
	}
}

































