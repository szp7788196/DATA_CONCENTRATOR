//ָʾ�ƴ�������
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

	while(1)											//ѭ��һ����ʱԼ20ms
	{
		key_num = KEY_Scan(0);
		
		if(key_num != 0)
		{
			tcp_client_flag |= LWIP_SEND_DATA;
		}	
		
		if(cnt % 100 == 0)								//ÿ1��ιһ�ο��Ź�
		{
			IWDG_Feed_Thread();
		}
		
		if(cnt % 50 == 0)								//���е���1���������˸
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
		
		delay_ms(10);						//ѭ��һ����ʱԼ20ms
	}
}

















