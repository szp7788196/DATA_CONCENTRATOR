#include "relay_comm.h"
#include "common.h"
#include "concentrator_comm.h"


void RelayRecvAndHandleFrameStruct(void)
{
	BaseType_t xResult;
	ServerFrameStruct_S *server_frame_struct = NULL;

	xResult = xQueueReceive(xQueue_RelayFrameStruct,(void *)&server_frame_struct,(TickType_t)pdMS_TO_TICKS(1));

	if(xResult == pdPASS )
	{
		switch(server_frame_struct->msg_id)
		{
			case 0x0000:	//����͸��

			break;

			case 0x0201:	//��·����

			break;

			case 0x0202:	//�������

			break;
			
			case 0x0203:	//ģʽ�л�

			break;
			
			case 0x0270:	//״̬��ѯ

			break;
			
			case 0x0271:	//״̬�ϱ�

			break;
			
			case 0x0272:	//״̬��ʷ��ѯ

			break;
			
			case 0x02A0:	//�澯�ϱ�
				AlarmReportResponse = 0;
			break;

			case 0x02A1:	//�澯���
				AlarmReportResponse = 0;
			break;

			case 0x02A2:	//�¼��ϱ�

			break;
			
			case 0x02A3:	//�澯����

			break;
			
			case 0x02A4:	//�澯���ò�ѯ

			break;
			
			case 0x02A5:	//�澯��ʷ��ѯ
				
			break;
			
			case 0x02D0:	//��������

			break;
			
			case 0x02D1:	//��ѯ��������

			break;
			
			case 0x02D2:	//��������

			break;
			
			case 0x02D3:	//��ѯ��������

			break;
			
			case 0x02D4:	//��������

			break;
			
			case 0x02D5:	//��ѯ��������

			break;

			default:
			break;
		}

		DeleteServerFrameStruct(server_frame_struct);
	}
}



































