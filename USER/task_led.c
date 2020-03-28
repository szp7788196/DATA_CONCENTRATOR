//ָʾ�ƴ�������
#include "task_led.h"
#include "delay.h"
#include "led.h"
#include "watch_dog.h"
#include "rx8010s.h"
#include "exfuns.h"
#include "ff.h"
#include "ht7038.h"
#include "event_alarm.h"
#include "common.h"



TaskHandle_t xHandleTaskLED = NULL;

u32 FreeHeapSize = 0;

void vTaskLED(void *pvParameters)
{
	u32 cnt = 0;
	u8 led_state = 0;

	while(1)											//ѭ��һ����ʱԼ20ms
	{
		FreeHeapSize = xPortGetFreeHeapSize();
		
		if(cnt % 10 == 0)								//ÿ1��ιһ�ο��Ź�
		{
			IWDG_Feed_Thread();
			
			RX8010S_Get_Time();
		}
		
		if(cnt % 5 == 0)								//���е���1���������˸
		{
			led_state = ~led_state;
		}
		
		if(led_state)
		{
			LED_RUN = 0;
		}
		else
		{
			LED_RUN = 1;
		}
		
		cnt = (cnt + 1) & 0xFFFFFFFF;
		
		delay_ms(100);									//ѭ��һ����ʱԼ20ms
	}
}

















