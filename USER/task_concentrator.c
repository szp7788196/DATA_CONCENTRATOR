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
		RecvAndHandleFrameStruct();						//���Ľ���

		AutoSendFrameToServer();						//���͵�¼/����/�澯����Ϣ

		if(FlagSystemReBoot == 1)						//���յ�����������
		{
			FlagSystemReBoot = 0;
			delay_ms(5000);

			__disable_fault_irq();						//����ָ��
			NVIC_SystemReset();
		}

		if(FlagReConnectToServer == 1)					//���յ��������ӷ�����ָ��
		{
			delay_ms(5000);

			FlagReConnectToServer = 2;
			LoginResponse = 0;
		}

		if(FrameWareState.state == FIRMWARE_DOWNLOADED)	//�̼��������,����ϵͳ
		{
			delay_ms(5000);

			__disable_fault_irq();						//����ָ��
			NVIC_SystemReset();
		}

		delay_ms(50);
	}
}

























































