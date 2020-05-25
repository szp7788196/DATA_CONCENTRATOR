//ָʾ�ƴ�������
#include "task_led.h"
#include "delay.h"
#include "led.h"
#include "watch_dog.h"
#include "rx8010s.h"
#include "exfuns.h"
#include "ff.h"
#include "ht7038.h"
#include "history_record.h"
#include "common.h"
#include "sun_rise_set.h"
#include "concentrator_conf.h"



TaskHandle_t xHandleTaskLED = NULL;

u32 FreeHeapSize = 0;

void vTaskLED(void *pvParameters)
{
	u32 cnt = 0;
	u8 led_state = 0;
	u8 date = 0;

	while(1)											//ѭ��һ����ʱԼ100ms
	{
		if(date != calendar.w_date)						//������һ����ճ�����ʱ��
		{
			date = calendar.w_date;
			
			SunRiseSetTime = GetSunTime(calendar.w_year,
			                            calendar.w_month,
			                            calendar.w_date,
			                            ConcentratorLocationConfig.longitude,
			                            ConcentratorLocationConfig.latitude);
		}
		
		FreeHeapSize = xPortGetFreeHeapSize();
		
		if(cnt % 50 == 0)
		{
			printf("FreeHeapSize:%d\r\n",FreeHeapSize);
		}
		
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

















