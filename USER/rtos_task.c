#include "rtos_task.h"
#include "common.h"
#include "server_protocol.h"
#include "task_led.h"
#include "task_4g.h"
#include "task_sensor.h"
#include "task_lwip_init.h"
#include "task_handle_server_frame.h"
#include "task_concentrator.h"
#include "task_store.h"
#include "event_alarm.h"




void AppTaskCreate(void)
{
//	xTaskCreate(vTaskLWIP_INIT,    					/* 指示灯任务  */
//				"vTaskLWIP_INIT",  					/* 任务名称    */
//				configMINIMAL_STACK_SIZE * 2,       /* stack大小,单位word,也就是4字节 */
//				NULL,        						/* 任务参数  */
//				configMAX_PRIORITIES - 7,           /* 任务优先级*/
//				&xHandleTaskLWIP_INIT); 			/* 任务句柄  */

	xTaskCreate(vTask4G,    						/* 指示灯任务  */
				" ",  						/* 任务名称    */
				1024,         						/* stack大小,单位word,也就是4字节 */
				NULL,        						/* 任务参数  */
				configMAX_PRIORITIES - 0,           /* 任务优先级*/
				&xHandleTask4G);

	xTaskCreate(vTaskHANDLE_SERVER_FRAME,    		/* 指示灯任务  */
				" ",  		/* 任务名称    */
				1024,         						/* stack大小,单位word,也就是4字节 */
				NULL,        						/* 任务参数  */
				configMAX_PRIORITIES - 1,           /* 任务优先级*/
				&xHandleTaskHANDLE_SERVER_FRAME); 	/* 任务句柄  */
	
	xTaskCreate(vTaskCONCENTRATOR,    				/* 指示灯任务  */
				" ",  				/* 任务名称    */
				1024,         						/* stack大小,单位word,也就是4字节 */
				NULL,        						/* 任务参数  */
				configMAX_PRIORITIES - 2,           /* 任务优先级*/
				&xHandleTaskCONCENTRATOR); 			/* 任务句柄  */
				
	xTaskCreate(vTaskSTORE,    						/* 指示灯任务  */
				"",  								/* 任务名称    */
				512,         						/* stack大小,单位word,也就是4字节 */
				NULL,        						/* 任务参数  */
				configMAX_PRIORITIES - 7,           /* 任务优先级*/
				&xHandleTaskSTORE); 				/* 任务句柄  */

	xTaskCreate(vTaskSENSOR,    					/* 指示灯任务  */
				"",  								/* 任务名称    */
				128,         						/* stack大小,单位word,也就是4字节 */
				NULL,        						/* 任务参数  */
				configMAX_PRIORITIES - 8,           /* 任务优先级*/
				&xHandleTaskSENSOR); 				/* 任务句柄  */

	xTaskCreate(vTaskLED,    						/* 指示灯任务  */
				"",  								/* 任务名称    */
				512,         						/* stack大小,单位word,也就是4字节 */
				NULL,        						/* 任务参数  */
				configMAX_PRIORITIES - 9,           /* 任务优先级*/
				&xHandleTaskLED); 					/* 任务句柄  */
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

	//消息队列
	xQueue_ServerFrameRx = xQueueCreate(10, sizeof(ServerFrame_S *));
    if(xQueue_ServerFrameRx == NULL)
    {

    }

	xQueue_4gFrameTx = xQueueCreate(10, sizeof(ServerFrame_S *));
    if(xQueue_4gFrameTx == NULL)
    {

    }

	xQueue_WifiFrameTx = xQueueCreate(10, sizeof(ServerFrame_S *));
    if(xQueue_WifiFrameTx == NULL)
    {

    }

	xQueue_EthFrameTx = xQueueCreate(10, sizeof(ServerFrame_S *));
    if(xQueue_EthFrameTx == NULL)
    {

    }

	xQueue_NB_IoTFrameTx = xQueueCreate(10, sizeof(ServerFrame_S *));
    if(xQueue_NB_IoTFrameTx == NULL)
    {

    }
	
	xQueue_ConcentratorFrameStruct = xQueueCreate(10, sizeof(ServerFrame_S *));
    if(xQueue_ConcentratorFrameStruct == NULL)
    {

    }
	
	xQueue_LampControllerFrameStruct = xQueueCreate(10, sizeof(ServerFrameStruct_S *));
    if(xQueue_LampControllerFrameStruct == NULL)
    {

    }
	
	xQueue_RelayFrameStruct = xQueueCreate(10, sizeof(ServerFrameStruct_S *));
    if(xQueue_RelayFrameStruct == NULL)
    {

    }
	
	xQueue_InputCollectorFrameStruct = xQueueCreate(10, sizeof(ServerFrameStruct_S *));
    if(xQueue_InputCollectorFrameStruct == NULL)
    {

    }
	
	xQueue_ElectricMeterFrameStruct = xQueueCreate(10, sizeof(ServerFrameStruct_S *));
    if(xQueue_ElectricMeterFrameStruct == NULL)
    {

    }
	
	xQueue_LumeterFrameStruct = xQueueCreate(10, sizeof(ServerFrameStruct_S *));
    if(xQueue_LumeterFrameStruct == NULL)
    {

    }
	
	xQueue_AlarmReportSend = xQueueCreate(10, sizeof(AlarmReport_S *));
    if(xQueue_AlarmReportSend == NULL)
    {

    }
	
	xQueue_AlarmReportStore = xQueueCreate(10, sizeof(AlarmReport_S *));
    if(xQueue_AlarmReportStore == NULL)
    {

    }
}










































