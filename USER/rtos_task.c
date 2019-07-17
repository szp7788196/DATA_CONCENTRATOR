//�����������Ϣ����

#include "rtos_task.h"
#include "common.h"
#include "task_led.h"
#include "task_lwip_init.h"



void AppTaskCreate(void)
{
	xTaskCreate(vTaskLWIP_INIT,    					/* ָʾ������  */
				"vTaskLWIP_INIT",  					/* ��������    */
				configMINIMAL_STACK_SIZE * 2,       /* stack��С,��λword,Ҳ����4�ֽ� */
				NULL,        						/* �������  */
				configMAX_PRIORITIES - 7,           /* �������ȼ�*/
				&xHandleTaskLWIP_INIT); 			/* ������  */	
	
	xTaskCreate(vTaskLED,    						/* ָʾ������  */
				"vTaskLED",  						/* ��������    */
				64,         						/* stack��С,��λword,Ҳ����4�ֽ� */
				NULL,        						/* �������  */
				configMAX_PRIORITIES - 6,           /* �������ȼ�*/
				&xHandleTaskLED); 					/* ������  */
}



//������Ϣ����
void AppObjCreate(void)
{
	
}










































