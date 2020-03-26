#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rtos_task.h"
#include "watch_dog.h"
#include "led.h"
#include "kc.h"
#include "kr.h"
#include "rx8010s.h"
#include "cat25x.h"
#include "w25qxx.h" 
#include "ht7038.h"
#include "usart6.h"
#include "ec20.h"
#include "concentrator.h"


//u8 eprombuf[100];
//u8 i = 0;
RCC_ClocksTypeDef RCC_Clocks;
u16 siiize = 0;
int main(void)
{
	SCB->VTOR = FLASH_BASE | 0x40000; 	/* Vector Table Relocation in Internal FLASH. */
//	IWDG_Init(6,1000);									//���Ź���ʱ8��
	
	RCC_GetClocksFreq(&RCC_Clocks);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		//����ϵͳ�ж����ȼ�����ţ�4����Ӧ���ȼ������飬��ռ���ȼ���16������
	delay_init(168);									//��ʼ����ʱ����
	
	LED_Init();											//LEDָʾ�Ƴ�ʼ��
	KC_Init();											//�����������ʼ��
	KR_Init();											//�����������ʼ��
	
	TIM2_Init(100);										//��ʱ��2��ʼ��
	
	USART1_Init(9600);									//ά�����ڳ�ʼ��
	USART5_Init(9600);									//RS485����
	USART6_Init(115200);								//4Gģ�鴮��
	
	RX8010S_Init();										//�ⲿRTC��ʼ��
	CAT25X_Init();										//�ⲿEEPROM��ʼ��
	W25QXX_Init();										//�ⲿFLASH��ʼ��
	HT7038_Init();										//���ܼ���оƬ��ʼ��
	EC20_Init();										//4Gģ���ʼ��
	
	
//	siiize = sizeof(ConcentratorLocationConfig_S);
	
	
	
	ReadTotalConfigurationParameters();				//��ȡ�������ò���

	IWDG_Feed_Thread();									//�ϵ��һ��ι���Ź�

	AppObjCreate();										//������Ϣ����
	AppTaskCreate();									//��������

    vTaskStartScheduler();          					//�����������
}














