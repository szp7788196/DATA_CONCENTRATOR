#include "task_input_collector.h"
#include "input_collector_comm.h"
#include "input_collector_event.h"
#include "kr.h"


TaskHandle_t xHandleTaskINPUT_COLLECTOR = NULL;


void vTaskINPUT_COLLECTOR(void *pvParameters)
{
	while(1)
	{
		InputCollectorRecvAndHandleFrameStruct();			//���Ľ���
		
		InputCollectorEventCheckPolling();					//�澯���¼���ѵ
		
		InputCollectorCollectCurrentState();				//�ɼ��������ɼ�ģ��״̬
		
		InputCollectorRecvAndHandleDeviceFrame();			//���ղ����������������ɼ�ģ�鷵�ص�����
		
		delay_ms(100);
	}
}



//�ɼ��������ɼ�ģ��״̬
void InputCollectorCollectCurrentState(void)
{
	u8 i = 0;
	static time_t time_s = 0;

	if(InputCollectorBasicConfig.detect_interval != 0)
	{
		if(GetSysTick10ms() - time_s >= (InputCollectorBasicConfig.detect_interval / 10))
		{
			time_s = GetSysTick10ms();
			
			for(i = 0; i < InputCollectorConfigNum.number; i ++)
			{
				if(InputCollectorState[i].address == 0 && InputCollectorState[i].channel == 0)	//�����ڲ��̵���
				{
					GetAllBuiltInInputCollectorState(&InputCollectorState[i]);
				}
				else if(InputCollectorState[i].channel == 1)									//�����ⲿ�̵���ģ��
				{
					GetBuiltOutInputCollectorState(InputCollectorState[i]);
				}
			}
		}
	}
}

//���ղ����������������ɼ�ģ�鷵�ص�����
void InputCollectorRecvAndHandleDeviceFrame(void)
{
	u8 i = 0;
	Rs485Frame_S *recv_rs485_frame = NULL;
	BaseType_t xResult;
	InputCollectorCollectState_S collect_state;

	xResult = xQueueReceive(xQueue_InputCollectorRs485Frame,(void *)&recv_rs485_frame,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS)
	{
		AnalysisBuiltOutInputCollectorFrame(recv_rs485_frame->buf,recv_rs485_frame->len,&collect_state);

		if(collect_state.update == 1)	//���ü̵���ģ��ɼ�״̬�и���
		{
			collect_state.update = 0;

			for(i = 0; i < InputCollectorConfigNum.number; i ++)
			{
				if(InputCollectorState[i].address == collect_state.address &&
				   InputCollectorState[i].channel == collect_state.channel)
				{
					InputCollectorState[i].d_current_state = collect_state.d_collect_state;
					InputCollectorState[i].a_current_state[0] = collect_state.a_collect_state[0];
					InputCollectorState[i].a_current_state[1] = collect_state.a_collect_state[1];
				}
			}
		}
		
		DeleteRs485Frame(recv_rs485_frame);
	}
}





































