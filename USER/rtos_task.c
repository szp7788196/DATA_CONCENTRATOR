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

/*任务优先级列表

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
	xTaskCreate(vTaskLWIP_INIT,    					/* 指示灯任务  */
				"vTaskLWIP_INIT",  					/* 任务名称    */
				configMINIMAL_STACK_SIZE * 2,       /* stack大小,单位word,也就是4字节 */
				NULL,        						/* 任务参数  */
				configMAX_PRIORITIES - 16,           /* 任务优先级*/
				&xHandleTaskLWIP_INIT); 			/* 任务句柄  */
	
//	xTaskCreate(vTask4G,    						/* 指示灯任务  */
//				"",  								/* 任务名称    */
//				1024,         						/* stack大小,单位word,也就是4字节 */
//				NULL,        						/* 任务参数  */
//				configMAX_PRIORITIES - 0,           /* 任务优先级*/
//				&xHandleTask4G);

//	xTaskCreate(vTaskHANDLE_SERVER_FRAME,    		/* 指示灯任务  */
//				"",  								/* 任务名称    */
//				1024,         						/* stack大小,单位word,也就是4字节 */
//				NULL,        						/* 任务参数  */
//				configMAX_PRIORITIES - 4,           /* 任务优先级*/
//				&xHandleTaskHANDLE_SERVER_FRAME); 	/* 任务句柄  */

//	xTaskCreate(vTaskCONCENTRATOR,    				/* 指示灯任务  */
//				"",  								/* 任务名称    */
//				1024,         						/* stack大小,单位word,也就是4字节 */
//				NULL,        						/* 任务参数  */
//				configMAX_PRIORITIES - 5,           /* 任务优先级*/
//				&xHandleTaskCONCENTRATOR); 			/* 任务句柄  */

//	xTaskCreate(vTaskRELAY,    						/* 指示灯任务  */
//				"",  								/* 任务名称    */
//				1024,         						/* stack大小,单位word,也就是4字节 */
//				NULL,        						/* 任务参数  */
//				configMAX_PRIORITIES - 6,           /* 任务优先级*/
//				&xHandleTaskRELAY); 				/* 任务句柄  */
//				
////	xTaskCreate(vTaskLAMP,    						/* 指示灯任务  */
////				"",  								/* 任务名称    */
////				1024,         						/* stack大小,单位word,也就是4字节 */
////				NULL,        						/* 任务参数  */
////				configMAX_PRIORITIES - 7,           /* 任务优先级*/
////				&xHandleTaskRELAY); 				/* 任务句柄  */
//				
//	xTaskCreate(vTaskINPUT_COLLECTOR,    			/* 指示灯任务  */
//				"",  								/* 任务名称    */
//				1024,         						/* stack大小,单位word,也就是4字节 */
//				NULL,        						/* 任务参数  */
//				configMAX_PRIORITIES - 8,           /* 任务优先级*/
//				&xHandleTaskINPUT_COLLECTOR); 		/* 任务句柄  */
//				
//	xTaskCreate(vTaskELECTRICITY_METER,    			/* 指示灯任务  */
//				"",  								/* 任务名称    */
//				1024,         						/* stack大小,单位word,也就是4字节 */
//				NULL,        						/* 任务参数  */
//				configMAX_PRIORITIES - 9,           /* 任务优先级*/
//				&xHandleTaskELECTRICITY_METER); 	/* 任务句柄  */
//				
////	xTaskCreate(vTaskLUMETER,    						/* 指示灯任务  */
////				"",  								/* 任务名称    */
////				1024,         						/* stack大小,单位word,也就是4字节 */
////				NULL,        						/* 任务参数  */
////				configMAX_PRIORITIES - 10,           /* 任务优先级*/
////				&xHandleTaskRELAY); 				/* 任务句柄  */

//	xTaskCreate(vTaskRS485,    						/* 指示灯任务  */
//				"",  								/* 任务名称    */
//				128,         						/* stack大小,单位word,也就是4字节 */
//				NULL,        						/* 任务参数  */
//				configMAX_PRIORITIES - 11,           /* 任务优先级*/
//				&xHandleTaskRS485);

//	xTaskCreate(vTaskLED,    						/* 指示灯任务  */
//				"",  								/* 任务名称    */
//				512,         						/* stack大小,单位word,也就是4字节 */
//				NULL,        						/* 任务参数  */
//				configMAX_PRIORITIES - 12,           /* 任务优先级*/
//				&xHandleTaskLED); 					/* 任务句柄  */
//				
//	xTaskCreate(vTaskSTORE,    						/* 指示灯任务  */
//				"",  								/* 任务名称    */
//				1024,         						/* stack大小,单位word,也就是4字节 */
//				NULL,        						/* 任务参数  */
//				configMAX_PRIORITIES - 15,           /* 任务优先级*/
//				&xHandleTaskSTORE); 				/* 任务句柄  */
//				
////	xTaskCreate(vTaskLWIP_INIT,    					/* 指示灯任务  */
////				"vTaskLWIP_INIT",  					/* 任务名称    */
////				configMINIMAL_STACK_SIZE * 2,       /* stack大小,单位word,也就是4字节 */
////				NULL,        						/* 任务参数  */
////				configMAX_PRIORITIES - 16,           /* 任务优先级*/
////				&xHandleTaskLWIP_INIT); 			/* 任务句柄  */
}


//创建消息队列
void AppObjCreate(void)
{
	//互斥锁
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

	//消息队列
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










































