#include "task_input_collector.h"
#include "input_collector_comm.h"
#include "input_collector_event.h"
#include "kr.h"


TaskHandle_t xHandleTaskINPUT_COLLECTOR = NULL;
unsigned portBASE_TYPE SatckCOLLECTOR;

void vTaskINPUT_COLLECTOR(void *pvParameters)
{
	static u32 cnt = 0;

	while(1)
	{
		if(cnt % 20)
		{
			GetKrState();
		}

		InputCollectorRecvAndHandleFrameStruct();			//���Ľ���

		InputCollectorEventCheckPolling();					//�澯���¼���ѵ

		InputCollectorCollectCurrentState();				//�ɼ��������ɼ�ģ��״̬

		InputCollectorRecvAndHandleDeviceFrame();			//���ղ����������������ɼ�ģ�鷵�ص�����

		cnt ++;

		delay_ms(100);

		SatckCOLLECTOR = uxTaskGetStackHighWaterMark(NULL);
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
	u8 ret = 0;
	u8 i = 0;
	Rs485Frame_S *recv_rs485_frame = NULL;
	BaseType_t xResult;
	InputCollectorCollectState_S collect_state;

	xResult = xQueueReceive(xQueue_InputCollectorRs485Frame,(void *)&recv_rs485_frame,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS)
	{
		ret = AnalysisBuiltOutInputCollectorFrame(recv_rs485_frame->buf,recv_rs485_frame->len,&collect_state);

		if(ret == 1)
		{
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

						break;
					}
				}
			}
		}
		else
		{
			TransTransmissionFrame_S  *trans_trans_frame = NULL;
			
			trans_trans_frame = (TransTransmissionFrame_S *)pvPortMalloc(sizeof(TransTransmissionFrame_S));
			
			if(trans_trans_frame != NULL)
			{
				trans_trans_frame->device_type = INPUT_COLLECTOR;
				
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

		DeleteRs485Frame(recv_rs485_frame);
	}
}





































