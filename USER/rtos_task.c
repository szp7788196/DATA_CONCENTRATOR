//创建任务和消息队列

#include "rtos_task.h"
#include "common.h"
#include "task_led.h"
#include "task_lwip_init.h"



void AppTaskCreate(void)
{
	xTaskCreate(vTaskLWIP_INIT,    					/* 指示灯任务  */
				"vTaskLWIP_INIT",  					/* 任务名称    */
				configMINIMAL_STACK_SIZE * 2,       /* stack大小,单位word,也就是4字节 */
				NULL,        						/* 任务参数  */
				configMAX_PRIORITIES - 7,           /* 任务优先级*/
				&xHandleTaskLWIP_INIT); 			/* 任务句柄  */	
	
	xTaskCreate(vTaskLED,    						/* 指示灯任务  */
				"vTaskLED",  						/* 任务名称    */
				64,         						/* stack大小,单位word,也就是4字节 */
				NULL,        						/* 任务参数  */
				configMAX_PRIORITIES - 6,           /* 任务优先级*/
				&xHandleTaskLED); 					/* 任务句柄  */
}



//创建消息队列
void AppObjCreate(void)
{
	
}










































