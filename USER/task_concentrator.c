#include "task_concentrator.h"
#include "delay.h"
#include <time.h>
#include "common.h"
#include "concentrator_comm.h"
#include "server_protocol.h"
#include "task_handle_server_frame.h"
#include "ff.h"
#include "exfuns.h"
#include "fattester.h"
#include "history_record.h"
#include "rx8010s.h"
#include "concentrator_event.h"



TaskHandle_t xHandleTaskCONCENTRATOR = NULL;
unsigned portBASE_TYPE SatckCONCENTRATOR;

void vTaskCONCENTRATOR(void *pvParameters)
{
	while(1)
	{
		ConcentratorRecvAndHandleFrameStruct();			//���Ľ���

		AutoSendFrameToServer();						//���͵�¼/����/�澯����Ϣ

		ConcentratorEventCheckPolling();				//�澯���¼���ѵ

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

		delay_ms(100);

		SatckCONCENTRATOR = uxTaskGetStackHighWaterMark(NULL);
	}
}

























































