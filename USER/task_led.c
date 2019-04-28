//指示灯处理任务
#include "task_led.h"
#include "delay.h"
#include "led.h"
#include "watch_dog.h"
#include "key.h"
#include "task_tcp_client.h"



TaskHandle_t xHandleTaskLED = NULL;

void vTaskLED(void *pvParameters)
{
	u32 cnt = 0;
	u8 led_state = 0;
	u8 key_num = 0;

	while(1)											//循环一次延时约20ms
	{
		key_num = KEY_Scan(0);
		
		if(key_num != 0)
		{
			tcp_client_flag |= LWIP_SEND_DATA;
		}	
		
		if(cnt % 100 == 0)								//每1秒喂一次看门狗
		{
			IWDG_Feed_Thread();
		}
		
		if(cnt % 50 == 0)								//运行灯以1秒的周期闪烁
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
		
		delay_ms(10);						//循环一次延时约20ms
	}
}

















