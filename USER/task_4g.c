#include "task_4g.h"
#include "ec20.h"
#include "delay.h"
#include "rx8010s.h"
#include <time.h>
#include "common.h"
#include "concentrator_comm.h"
#include "concentrator_conf.h"
#include "server_protocol.h"
#include "task_handle_server_frame.h"



TaskHandle_t xHandleTask4G = NULL;



void vTask4G(void *pvParameters)
{
	u8 ret = 0;
	u16 wait_cnt = 0;
	u8 refresh_state = 0;
	time_t time_s = 0;


	RE_INIT:
	wait_cnt = 0;
	ConnectState = UNKNOW_STATE;

	ec20_soft_init();

	while(1)
	{
		if(FlagReConnectToServer == 2)
		{
			FlagReConnectToServer = 0;
			
			goto RE_INIT;
		}
		
		if(ConnectState != CONNECTED)
		{
			refresh_state = 1;
		}
		else if(GetSysTick1s() - time_s >= 30)
		{
			time_s = GetSysTick1s();

			refresh_state = 1;
		}

		if(refresh_state == 1)
		{
			refresh_state = 0;

			ConnectState = ec20_get_AT_QISTATE();

			ec20_get_AT_CSQ((char *)&EC20Info.csq);

			SyncDataTimeFormEC20Module(43200);		//每隔12小时对时一次
		}

		switch((u8)ConnectState)
		{
			case (u8)UNKNOW_STATE:
				ret = ec20_set_AT_QIACT();

				if(ret == 0)
				{
					goto RE_INIT;
				}
			break;

			case (u8)INITIAL:
				ret = ec20_set_AT_QICLOSE();

				if(ret == 0)
				{
					goto RE_INIT;
				}
			break;

			case (u8)OPENING:
				delay_ms(200);

				if((wait_cnt ++) >= 100)
				{
					goto RE_INIT;
				}
			break;

			case (u8)GOT_IP:
				ret = ec20_set_AT_QIOPEN((char *)ConcentratorBasicConfig.server_ip, 
			                             (char *)ConcentratorBasicConfig.server_port);

				if(ret == 0)
				{
					goto RE_INIT;
				}
			break;

			case (u8)CONNECTED:
				RecvNetFrameAndPushToRxQueue(MODE_4G);
				
				Pull4gTxQueueAndSendFrame();
			break;

			case (u8)CLOSING:
				ret = ec20_set_AT_QICLOSE();

				if(ret == 0)
				{
					goto RE_INIT;
				}
			break;

			default:
			break;
		}

		delay_ms(100);
	}
}

void Pull4gTxQueueAndSendFrame(void)
{
	BaseType_t xResult;
	ServerFrame_S *tx_frame = NULL;

	xResult = xQueueReceive(xQueue_4gFrameTx,(void *)&tx_frame,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS )
	{
		ec20_get_AT_QISEND(tx_frame->buf,tx_frame->len);
		
		vPortFree(tx_frame->buf);
		tx_frame->buf = NULL;

		vPortFree(tx_frame);
		tx_frame = NULL;
	}
}

//从指定的NTP服务器获取时间
u8 SyncDataTimeFormEC20Module(time_t sync_cycle)
{
	u8 ret = 0;
	static u8 got_ok = 0;
	struct tm tm_time;
	static time_t time_s = 0;
	static time_t time_c = 0;
	char buf[32];

	if((GetSysTick1s() - time_c >= sync_cycle) || got_ok == 0)
	{
		time_c = GetSysTick1s();

		memset(buf,0,32);

		if(ec20_get_AT_CCLK(buf))
		{
			tm_time.tm_year = 2000 + (buf[0] - 0x30) * 10 + buf[1] - 0x30 - 1900;
			tm_time.tm_mon = (buf[3] - 0x30) * 10 + buf[4] - 0x30 - 1;
			tm_time.tm_mday = (buf[6] - 0x30) * 10 + buf[7] - 0x30;

			tm_time.tm_hour = (buf[9] - 0x30) * 10 + buf[10] - 0x30;
			tm_time.tm_min = (buf[12] - 0x30) * 10 + buf[13] - 0x30;
			tm_time.tm_sec = (buf[15] - 0x30) * 10 + buf[16] - 0x30;

			time_s = mktime(&tm_time);

			time_s += 28800;

			SyncTimeFromNet(time_s);

			got_ok = 1;

			ret = 1;
		}
	}

	return ret;
}







































