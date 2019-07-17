#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rtos_task.h"
#include "watch_dog.h"
#include "led.h"


//u8 eprombuf[100];
//u8 i = 0;
RCC_ClocksTypeDef RCC_Clocks;
int main(void)
{
	RCC_GetClocksFreq(&RCC_Clocks);
//	IWDG_Init(6,1000);									//���Ź���ʱ8��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		//����ϵͳ�ж����ȼ�����ţ�4����Ӧ���ȼ������飬��ռ���ȼ���16������
	delay_init(168);									//��ʼ����ʱ����
	
	LED_Init();
	
	TIM2_Config(100);									//��ʱ��2��ʼ��
	usart1_config(9600);								//ά�����ڳ�ʼ��

	IWDG_Feed_Thread();									//�ϵ��һ��ι���Ź�

	AppObjCreate();										//������Ϣ����
	AppTaskCreate();									//��������

    vTaskStartScheduler();          					//�����������
}














