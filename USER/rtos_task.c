#include "rtos_task.h"
#include "common.h"
#include "server_protocol.h"
#include "history_record.h"
#include "relay_conf.h"
#include "task_led.h"
#include "task_4g.h"
#include "task_electricity_meter.h"
#include "task_lwip_init.h"
#include "task_handle_server_frame.h"
#include "task_concentrator.h"
#include "task_store.h"
#include "task_relay.h"
#include "task_rs485.h"
#include "task_input_collector.h"

/*�������ȼ��б�

vTask4G						0
prvTimerTask				1
tcpip_thread				2
vTaskTCP_CLIENT				3
vTaskHANDLE_SERVER_FRAME	4
vTaskCONCENTRATOR			5
vTaskRELAY					6
vTaskLAMP					7
vTaskINPUT_COLLECTOR		8
vTaskELECTRICITY_METER		9
vTaskLUMETER				10
vTaskRS485					11
vTaskLED					12
LwIP_DHCP_task				13
Eth_Link_IT_task			14
vTaskSTORE					15
vTaskLWIP_INIT				16
prvIdleTask					17

*/



void AppTaskCreate(void)
{
	xTaskCreate(vTaskLWIP_INIT,    					/* ָʾ������  */
				"vTaskLWIP_INIT",  					/* ��������    */
				configMINIMAL_STACK_SIZE * 2,       /* stack��С,��λword,Ҳ����4�ֽ� */
				NULL,        						/* �������  */
				configMAX_PRIORITIES - 16,           /* �������ȼ�*/
				&xHandleTaskLWIP_INIT); 			/* ������  */
	
//	xTaskCreate(vTask4G,    						/* ָʾ������  */
//				"",  								/* ��������    */
//				1024,         						/* stack��С,��λword,Ҳ����4�ֽ� */
//				NULL,        						/* �������  */
//				configMAX_PRIORITIES - 0,           /* �������ȼ�*/
//				&xHandleTask4G);

//	xTaskCreate(vTaskHANDLE_SERVER_FRAME,    		/* ָʾ������  */
//				"",  								/* ��������    */
//				1024,         						/* stack��С,��λword,Ҳ����4�ֽ� */
//				NULL,        						/* �������  */
//				configMAX_PRIORITIES - 4,           /* �������ȼ�*/
//				&xHandleTaskHANDLE_SERVER_FRAME); 	/* ������  */

//	xTaskCreate(vTaskCONCENTRATOR,    				/* ָʾ������  */
//				"",  								/* ��������    */
//				1024,         						/* stack��С,��λword,Ҳ����4�ֽ� */
//				NULL,        						/* �������  */
//				configMAX_PRIORITIES - 5,           /* �������ȼ�*/
//				&xHandleTaskCONCENTRATOR); 			/* ������  */

//	xTaskCreate(vTaskRELAY,    						/* ָʾ������  */
//				"",  								/* ��������    */
//				1024,         						/* stack��С,��λword,Ҳ����4�ֽ� */
//				NULL,        						/* �������  */
//				configMAX_PRIORITIES - 6,           /* �������ȼ�*/
//				&xHandleTaskRELAY); 				/* ������  */
//				
////	xTaskCreate(vTaskLAMP,    						/* ָʾ������  */
////				"",  								/* ��������    */
////				1024,         						/* stack��С,��λword,Ҳ����4�ֽ� */
////				NULL,        						/* �������  */
////				configMAX_PRIORITIES - 7,           /* �������ȼ�*/
////				&xHandleTaskRELAY); 				/* ������  */
//				
//	xTaskCreate(vTaskINPUT_COLLECTOR,    			/* ָʾ������  */
//				"",  								/* ��������    */
//				1024,         						/* stack��С,��λword,Ҳ����4�ֽ� */
//				NULL,        						/* �������  */
//				configMAX_PRIORITIES - 8,           /* �������ȼ�*/
//				&xHandleTaskINPUT_COLLECTOR); 		/* ������  */
//				
//	xTaskCreate(vTaskELECTRICITY_METER,    			/* ָʾ������  */
//				"",  								/* ��������    */
//				1024,         						/* stack��С,��λword,Ҳ����4�ֽ� */
//				NULL,        						/* �������  */
//				configMAX_PRIORITIES - 9,           /* �������ȼ�*/
//				&xHandleTaskELECTRICITY_METER); 	/* ������  */
//				
////	xTaskCreate(vTaskLUMETER,    						/* ָʾ������  */
////				"",  								/* ��������    */
////				1024,         						/* stack��С,��λword,Ҳ����4�ֽ� */
////				NULL,        						/* �������  */
////				configMAX_PRIORITIES - 10,           /* �������ȼ�*/
////				&xHandleTaskRELAY); 				/* ������  */

//	xTaskCreate(vTaskRS485,    						/* ָʾ������  */
//				"",  								/* ��������    */
//				128,         						/* stack��С,��λword,Ҳ����4�ֽ� */
//				NULL,        						/* �������  */
//				configMAX_PRIORITIES - 11,           /* �������ȼ�*/
//				&xHandleTaskRS485);

//	xTaskCreate(vTaskLED,    						/* ָʾ������  */
//				"",  								/* ��������    */
//				512,         						/* stack��С,��λword,Ҳ����4�ֽ� */
//				NULL,        						/* �������  */
//				configMAX_PRIORITIES - 12,           /* �������ȼ�*/
//				&xHandleTaskLED); 					/* ������  */
//				
//	xTaskCreate(vTaskSTORE,    						/* ָʾ������  */
//				"",  								/* ��������    */
//				1024,         						/* stack��С,��λword,Ҳ����4�ֽ� */
//				NULL,        						/* �������  */
//				configMAX_PRIORITIES - 15,           /* �������ȼ�*/
//				&xHandleTaskSTORE); 				/* ������  */
//				
////	xTaskCreate(vTaskLWIP_INIT,    					/* ָʾ������  */
////				"vTaskLWIP_INIT",  					/* ��������    */
////				configMINIMAL_STACK_SIZE * 2,       /* stack��С,��λword,Ҳ����4�ֽ� */
////				NULL,        						/* �������  */
////				configMAX_PRIORITIES - 16,           /* �������ȼ�*/
////				&xHandleTaskLWIP_INIT); 			/* ������  */
}


//������Ϣ����
void AppObjCreate(void)
{
	//������
	xMutex_SPI2 = xSemaphoreCreateMutex();
	if(xMutex_SPI2 == NULL)
    {

    }

	xMutex_RTC = xSemaphoreCreateMutex();
	if(xMutex_RTC == NULL)
    {

    }

	xMutex_Push_xQueue_ServerFrameRx = xSemaphoreCreateMutex();
	if(xMutex_Push_xQueue_ServerFrameRx == NULL)
    {

    }

	xMutex_Push_xQueue_ServerFrameTx = xSemaphoreCreateMutex();
	if(xMutex_Push_xQueue_ServerFrameTx == NULL)
    {

    }

	xMutex_SPI_FLASH = xSemaphoreCreateMutex();
	if(xMutex_SPI_FLASH == NULL)
    {

    }

	xMutex_Push_xQueue_AlarmReportSend = xSemaphoreCreateMutex();
	if(xMutex_Push_xQueue_AlarmReportSend == NULL)
    {

    }

	xMutex_Push_xQueue_AlarmReportStore = xSemaphoreCreateMutex();
	if(xMutex_Push_xQueue_AlarmReportStore == NULL)
    {

    }

	xMutex_TransServerFrameStruct = xSemaphoreCreateMutex();
	if(xMutex_TransServerFrameStruct == NULL)
    {

    }

	xMutex_RelayStrategy = xSemaphoreCreateMutex();
	if(xMutex_RelayStrategy == NULL)
    {

    }

	xMutex_RelayAppointment = xSemaphoreCreateMutex();
	if(xMutex_RelayAppointment == NULL)
    {

    }

	xMutex_Rs485Rs485Frame = xSemaphoreCreateMutex();
	if(xMutex_Rs485Rs485Frame == NULL)
    {

    }

	//��Ϣ����
	xQueue_ServerFrameRx = xQueueCreate(25, sizeof(ServerFrame_S *));
    if(xQueue_ServerFrameRx == NULL)
    {

    }

	xQueue_4gFrameTx = xQueueCreate(25, sizeof(ServerFrame_S *));
    if(xQueue_4gFrameTx == NULL)
    {

    }

	xQueue_WifiFrameTx = xQueueCreate(25, sizeof(ServerFrame_S *));
    if(xQueue_WifiFrameTx == NULL)
    {

    }

	xQueue_EthFrameTx = xQueueCreate(25, sizeof(ServerFrame_S *));
    if(xQueue_EthFrameTx == NULL)
    {

    }

	xQueue_NB_IoTFrameTx = xQueueCreate(25, sizeof(ServerFrame_S *));
    if(xQueue_NB_IoTFrameTx == NULL)
    {

    }

	xQueue_ConcentratorFrameStruct = xQueueCreate(25, sizeof(ServerFrame_S *));
    if(xQueue_ConcentratorFrameStruct == NULL)
    {

    }

	xQueue_LampControllerFrameStruct = xQueueCreate(25, sizeof(ServerFrameStruct_S *));
    if(xQueue_LampControllerFrameStruct == NULL)
    {

    }

	xQueue_RelayFrameStruct = xQueueCreate(25, sizeof(ServerFrameStruct_S *));
    if(xQueue_RelayFrameStruct == NULL)
    {

    }

	xQueue_InputCollectorFrameStruct = xQueueCreate(25, sizeof(ServerFrameStruct_S *));
    if(xQueue_InputCollectorFrameStruct == NULL)
    {

    }

	xQueue_ElectricityMeterFrameStruct = xQueueCreate(25, sizeof(ServerFrameStruct_S *));
    if(xQueue_ElectricityMeterFrameStruct == NULL)
    {

    }

	xQueue_LumeterFrameStruct = xQueueCreate(25, sizeof(ServerFrameStruct_S *));
    if(xQueue_LumeterFrameStruct == NULL)
    {

    }

	xQueue_AlarmReportSend = xQueueCreate(20, sizeof(AlarmReport_S *));
    if(xQueue_AlarmReportSend == NULL)
    {

    }

	xQueue_AlarmReportStore = xQueueCreate(20, sizeof(AlarmReport_S *));
    if(xQueue_AlarmReportStore == NULL)
    {

    }

	xQueue_AlarmReportRead = xQueueCreate(20, sizeof(AlarmReport_S *));
    if(xQueue_AlarmReportRead == NULL)
    {

    }

	xQueue_HistoryRecordRead = xQueueCreate(20, sizeof(AlarmReport_S *));
    if(xQueue_HistoryRecordRead == NULL)
    {

    }

	xQueue_RelayModuleState = xQueueCreate(10, sizeof(RelayModuleState_S *));
    if(xQueue_RelayModuleState == NULL)
    {

    }
	
	xQueue_InputCollectorState = xQueueCreate(10, sizeof(RelayModuleState_S *));
    if(xQueue_InputCollectorState == NULL)
    {

    }
	
	xQueue_ElectricityMeterState = xQueueCreate(10, sizeof(RelayModuleState_S *));
    if(xQueue_ElectricityMeterState == NULL)
    {

    }

	xQueue_Rs485Rs485Frame = xQueueCreate(40, sizeof(Rs485Frame_S *));
    if(xQueue_Rs485Rs485Frame == NULL)
    {

    }

	xQueue_RelayRs485Frame = xQueueCreate(10, sizeof(Rs485Frame_S *));
    if(xQueue_RelayRs485Frame == NULL)
    {

    }

	xQueue_InputCollectorRs485Frame = xQueueCreate(10, sizeof(Rs485Frame_S *));
    if(xQueue_InputCollectorRs485Frame == NULL)
    {

    }

	xQueue_ElectricityMeterRs485Frame = xQueueCreate(10, sizeof(Rs485Frame_S *));
    if(xQueue_ElectricityMeterRs485Frame == NULL)
    {

    }

	xQueue_LumeterRs485Frame = xQueueCreate(10, sizeof(Rs485Frame_S *));
    if(xQueue_LumeterRs485Frame == NULL)
    {

    }
}










































