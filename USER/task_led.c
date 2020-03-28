//指示灯处理任务
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

	while(1)											//循环一次延时约20ms
	{
		FreeHeapSize = xPortGetFreeHeapSize();
		
		if(cnt % 10 == 0)								//每1秒喂一次看门狗
		{
			IWDG_Feed_Thread();
			
			RX8010S_Get_Time();
		}
		
		if(cnt % 5 == 0)								//运行灯以1秒的周期闪烁
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
		
		delay_ms(100);									//循环一次延时约20ms
	}
}

















