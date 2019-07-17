#include "task_lwip_init.h"
#include "lwip_comm.h"
#include "task_tcp_client.h"
#include "httpd.h"



TaskHandle_t xHandleTaskLWIP_INIT = NULL;

void vTaskLWIP_INIT(void *pvParameters)
{
	/* configure ethernet (GPIOs, clocks, MAC, DMA) */ 
	ETH_BSP_Config();

	/* Initilaize the LwIP stack */
	LwIP_Init();
	
	tcp_client_init();
	
	httpd_init();
	
#ifdef USE_DHCP
  /* Start DHCPClient */
	xTaskCreate(LwIP_DHCP_task,    				/* ָʾ������  */
				"LwIP_DHCP_task",  				/* ��������    */
				configMINIMAL_STACK_SIZE * 2,   /* stack��С,��λword,Ҳ����4�ֽ� */
				NULL,        					/* �������  */
				configMAX_PRIORITIES - 5,       /* �������ȼ�*/
				&xHandleTaskDHCP); 				/* ������  */	
#endif

	vTaskDelete(xHandleTaskLWIP_INIT);
}

































