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
			case 0x0000:	//数据透传

			break;

			case 0x0201:	//回路控制

			break;

			case 0x0202:	//分组控制

			break;
			
			case 0x0203:	//模式切换

			break;
			
			case 0x0270:	//状态查询

			break;
			
			case 0x0271:	//状态上报

			break;
			
			case 0x0272:	//状态历史查询

			break;
			
			case 0x02A0:	//告警上报
				AlarmReportResponse = 0;
			break;

			case 0x02A1:	//告警解除
				AlarmReportResponse = 0;
			break;

			case 0x02A2:	//事件上报

			break;
			
			case 0x02A3:	//告警配置

			break;
			
			case 0x02A4:	//告警配置查询

			break;
			
			case 0x02A5:	//告警历史查询
				
			break;
			
			case 0x02D0:	//基础配置

			break;
			
			case 0x02D1:	//查询基础配置

			break;
			
			case 0x02D2:	//场景配置

			break;
			
			case 0x02D3:	//查询场景配置

			break;
			
			case 0x02D4:	//任务配置

			break;
			
			case 0x02D5:	//查询任务配置

			break;

			default:
			break;
		}

		DeleteServerFrameStruct(server_frame_struct);
	}
}



































