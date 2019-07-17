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
//	IWDG_Init(6,1000);									//看门狗超时8秒
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		//设置系统中断优先级分组号：4。响应优先级不分组，抢占优先级分16个级别
	delay_init(168);									//初始化延时函数
	
	LED_Init();
	
	TIM2_Config(100);									//定时器2初始化
	usart1_config(9600);								//维护串口初始化

	IWDG_Feed_Thread();									//上电第一次喂看门狗

	AppObjCreate();										//创建消息队列
	AppTaskCreate();									//创建任务

    vTaskStartScheduler();          					//开启任务调度
}














