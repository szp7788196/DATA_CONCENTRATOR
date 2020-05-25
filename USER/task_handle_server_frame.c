#include "task_handle_server_frame.h"
#include "server_protocol.h"
#include "concentrator_comm.h"
#include "fifo.h"
#include "usart6.h"
#include "common.h"




TaskHandle_t xHandleTaskHANDLE_SERVER_FRAME = NULL;



void vTaskHANDLE_SERVER_FRAME(void *pvParameters)
{
	BaseType_t xResult;
	ServerFrame_S *rx_frame = NULL;

	while(1)
	{
		xResult = xQueueReceive(xQueue_ServerFrameRx,(void *)&rx_frame,(TickType_t)pdMS_TO_TICKS(1));

		if(xResult == pdPASS )
		{
			ServerFrameHandle(rx_frame);
			
			DeleteServerFrame(rx_frame);
		}

		delay_ms(100);
	}
}

//将待发送的数据推进消息队列
void PushTheFrameToTxQueue(ServerFrame_S *tx_frame)
{
	xSemaphoreTake(xMutex_Push_xQueue_ServerFrameTx, portMAX_DELAY);

	{
		QueueHandle_t xQueue_XxFrameTx = NULL;
		
		switch((u8)tx_frame->connection_mode)
		{
			case (u8)MODE_4G:
				xQueue_XxFrameTx = xQueue_4gFrameTx;
			break;

			case (u8)MODE_ETH:
				xQueue_XxFrameTx = xQueue_EthFrameTx;
			break;
			
			case (u8)MODE_NB_IOT:
				xQueue_XxFrameTx = xQueue_NB_IoTFrameTx;
			break;
			
			case (u8)MODE_WIFI:
				xQueue_XxFrameTx = xQueue_WifiFrameTx;
			break;
			
			default:
				DeleteServerFrame(tx_frame);
			break;
		}

		if(xQueue_XxFrameTx != NULL)
		{
			if(xQueueSend(xQueue_XxFrameTx,(void *)&tx_frame,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_XxFrameTx fail.\r\n");
#endif
				DeleteServerFrame(tx_frame);
			}
		}
	}

	xSemaphoreGive(xMutex_Push_xQueue_ServerFrameTx);
}

//接收网络数据并做断帧粘帧处理
void RecvNetFrameAndPushToRxQueue(CONNECTION_MODE_E connection_mode)
{
	xSemaphoreTake(xMutex_Push_xQueue_ServerFrameRx, portMAX_DELAY);
	
	{
		u16 head_pos = 0xFFFF;
		u16 tail_pos = 0xFFFF;
		static time_t time_r = 0;
		static u16 recv_pos = 0;
		static u16 recv_len = 0;
		static u16 frame_len = 0;
		static u8 frame_head = 0x02;
		static u8 frame_tail = 0x03;
		static u8 recv_buf[NET_RX_FRAME_MAX_LEN] = {0};

		ServerFrame_S *rx_frame = NULL;

		recv_len = fifo_get(dl_buf_id,&recv_buf[recv_pos]);

		if(recv_len != 0)
		{
			time_r = GetSysTick1s();

			recv_pos = recv_pos + recv_len;
			recv_len = recv_pos;

			ANALYSIS_LOOP:
			head_pos = MyStrstr(recv_buf, &frame_head, recv_len, 1);
			tail_pos = MyStrstr(recv_buf, &frame_tail, recv_len, 1);

			if(head_pos != 0xFFFF && tail_pos != 0xFFFF)
			{
				if(tail_pos > head_pos)
				{
					frame_len = tail_pos + 1 - head_pos;

					rx_frame = (ServerFrame_S *)pvPortMalloc(sizeof(ServerFrame_S));

					if(rx_frame != NULL)
					{
						rx_frame->connection_mode = connection_mode;
						rx_frame->len = frame_len;

						rx_frame->buf = (u8 *)pvPortMalloc(frame_len * sizeof(u8));

						if(rx_frame->buf != NULL)
						{
							rx_frame->len = EscapeSymbolDelete(&recv_buf[head_pos],frame_len,rx_frame->buf);

							if(xQueueSend(xQueue_ServerFrameRx,(void *)&rx_frame,(TickType_t)10) != pdPASS)
							{
#ifdef DEBUG_LOG
								printf("send xQueue_ServerFrameRx fail.\r\n");
#endif
								DeleteServerFrame(rx_frame);
							}
						}
						else
						{
							DeleteServerFrame(rx_frame);
						}
					}

					if(recv_len > frame_len)	//还有未处理完的数据
					{
						recv_len = recv_len - tail_pos - 1;
						recv_pos = recv_len;

						memcpy(recv_buf,&recv_buf[tail_pos + 1],recv_len);

						goto ANALYSIS_LOOP;
					}
					else						//所有数据均处理完
					{
						recv_pos = 0;
						recv_len = 0;
						frame_len = 0;
					}
				}
			}
		}

		if(recv_pos != 0 || recv_len != 0)
		{
			if(GetSysTick1s() - time_r >= 3)	//接收到数据但是不完整，超时n秒数据作废
			{
				recv_pos = 0;
				recv_len = 0;
				frame_len = 0;
			}
		}
	}
	
	xSemaphoreGive(xMutex_Push_xQueue_ServerFrameRx);
}






































