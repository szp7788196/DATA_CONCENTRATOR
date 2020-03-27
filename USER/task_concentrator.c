#include "task_concentrator.h"
#include "delay.h"
#include <time.h>
#include "common.h"
#include "concentrator.h"
#include "server_protocol.h"
#include "task_handle_server_frame.h"
#include "ff.h"
#include "exfuns.h"
#include "fattester.h"



TaskHandle_t xHandleTaskCONCENTRATOR = NULL;



void vTaskCONCENTRATOR(void *pvParameters)
{
//	u8 res = 0;

//	res = mf_opendir("1:CONCEN");

//	if(res == FR_OK)
//	{
//		res=mf_open("1:CONCEN/test.txt", FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
//		res=mf_lseek(f_size(file));
//		res=mf_write("AB\r\n",4);
//		res=mf_close();

//		res=mf_open("1:CONCEN/test.txt", FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
//		mf_read(100);
//		res=mf_close();
//
//
//
//		mf_scan_files("1:CONCEN");
//
//		mf_unlink("1:CONCEN/test.txt");
//		mf_scan_files("1:CONCEN");
//

//		res = mf_closedir();
//	}

	while(1)
	{
		RecvAndHandleFrameStruct();						//报文解析

		AutoSendFrameToServer();						//发送登录/心跳/告警灯信息

		if(FlagSystemReBoot == 1)						//接收到重启的命令
		{
			FlagSystemReBoot = 0;
			delay_ms(5000);

			__disable_fault_irq();						//重启指令
			NVIC_SystemReset();
		}

		if(FlagReConnectToServer == 1)					//接收到重新连接服务器指令
		{
			delay_ms(5000);

			FlagReConnectToServer = 2;
			LoginResponse = 0;
		}

		if(FrameWareState.state == FIRMWARE_DOWNLOADED)	//固件下载完成,重启系统
		{
			delay_ms(5000);

			__disable_fault_irq();						//重启指令
			NVIC_SystemReset();
		}

		delay_ms(50);
	}
}

























































