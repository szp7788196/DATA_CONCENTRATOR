#include "task_electricity_meter.h"
#include "delay.h"
#include "ht7038.h"
#include "e_meter.h"
#include "electricity_meter_comm.h"
#include "electricity_meter_event.h"
#include "task_rs485.h"



TaskHandle_t xHandleTaskELECTRICITY_METER = NULL;
unsigned portBASE_TYPE SatckMETER;

void vTaskELECTRICITY_METER(void *pvParameters)
{
	u16 cnt = 0;
	
	delay_ms(1000);
	
	HT7038ConfigClibration();
	
	while(1)
	{
		if(cnt % 20)
		{
			HT7038GetParameters();
		}
		
		ElectricityMeterRecvAndHandleFrameStruct();				//��������
		
		ElectricityMeterEventCheckPolling();					//��ѵ���״̬�仯�͸澯�¼�
		
		ElectricityMeterCollectCurrentState();					//�ɼ��������
		
		ElectricityMeterRecvAndHandleDeviceFrame();				//����485���صĵ������
		
		cnt += 1;
		
		delay_ms(100);
		
		SatckMETER = uxTaskGetStackHighWaterMark(NULL);
	}
}


//�ɼ��������ɼ�ģ��״̬
void ElectricityMeterCollectCurrentState(void)
{
	u8 i = 0;
	static time_t time_s = 2;

	if(ElectricityMeterBasicConfig.detect_interval != 0)
	{
		if(GetSysTick10ms() - time_s >= (ElectricityMeterBasicConfig.detect_interval / 10))
		{
			time_s = GetSysTick10ms();
			
			for(i = 0; i < ElectricityMeterConfigNum.number; i ++)
			{
				if(ElectricityMeterState[i].address == 0 && ElectricityMeterState[i].channel == 0)	//�����ڲ��̵���
				{
					ElectricityMeterGetBuiltInParas(&ElectricityMeterState[i]);
				}
				else if(ElectricityMeterState[i].channel == 1)									//�����ⲿ�̵���ģ��
				{
					ElectricityMeterGetBuiltOutParas(ElectricityMeterState[i]);
				}
			}
		}
	}
}

//���ղ����������������ɼ�ģ�鷵�ص�����
void ElectricityMeterRecvAndHandleDeviceFrame(void)
{
	u8 ret = 0;
	u8 i = 0;
	Rs485Frame_S *recv_rs485_frame = NULL;
	BaseType_t xResult;
	static ElectricityMeterCollectState_S meter_state;

	xResult = xQueueReceive(xQueue_ElectricityMeterRs485Frame,(void *)&recv_rs485_frame,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS)
	{
		ret = AnalysisBuiltOutElectricityMeterFrame(recv_rs485_frame->buf,recv_rs485_frame->len,&meter_state);

		if(ret == 1)
		{
			if(meter_state.update == 1)	//���ü̵���ģ��ɼ�״̬�и���
			{
				meter_state.update = 0;

				for(i = 0; i < ElectricityMeterConfigNum.number; i ++)
				{
					if(ElectricityMeterState[i].address == meter_state.address &&
					   ElectricityMeterState[i].channel == meter_state.channel)
					{
						memcpy(&ElectricityMeterState[i].current_para,
							   meter_state.collect_para,
							   sizeof(ElectricityMeter_Para_S) * MAX_ELECTRICITY_METER_CH_NUM);
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
				trans_trans_frame->device_type = ELECTRICITY_METER;
				
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
						printf("send xQueue_TransTransFrame fail 4.\r\n");
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




































