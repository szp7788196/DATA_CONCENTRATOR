#include "task_lwip_init.h"
#include "task_tcp_client.h"



TaskHandle_t xHandleTaskLWIP_INIT = NULL;

void vTaskLWIP_INIT(void *pvParameters)
{
	/* configure ethernet (GPIOs, clocks, MAC, DMA) */ 
	ETH_BSP_Config();

	/* Initilaize the LwIP stack */
	LwIP_Init();

	
#ifdef USE_DHCP
  /* Start DHCPClient */
	
	xTaskCreate(LwIP_DHCP_task,    				/* 指示灯任务  */
				"LwIP_DHCP_task",  				/* 任务名称    */
				configMINIMAL_STACK_SIZE * 2,   /* stack大小,单位word,也就是4字节 */
				NULL,        					/* 任务参数  */
				4,           					/* 任务优先级*/
				&xHandleTaskLWIP_INIT); 		/* 任务句柄  */	
#endif
	
	tcp_client_init();
  
	while(1)
	{
		vTaskDelete(NULL);
	}
}

































