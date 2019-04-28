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
	
	xTaskCreate(LwIP_DHCP_task,    				/* ָʾ������  */
				"LwIP_DHCP_task",  				/* ��������    */
				configMINIMAL_STACK_SIZE * 2,   /* stack��С,��λword,Ҳ����4�ֽ� */
				NULL,        					/* �������  */
				4,           					/* �������ȼ�*/
				&xHandleTaskLWIP_INIT); 		/* ������  */	
#endif
	
	tcp_client_init();
  
	while(1)
	{
		vTaskDelete(NULL);
	}
}

































