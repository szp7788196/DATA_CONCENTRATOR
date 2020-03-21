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


//u8 eprombuf[100];
//u8 i = 0;
RCC_ClocksTypeDef RCC_Clocks;
int main(void)
{
	RCC_GetClocksFreq(&RCC_Clocks);
//	IWDG_Init(6,1000);									//看门狗超时8秒
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		//设置系统中断优先级分组号：4。响应优先级不分组，抢占优先级分16个级别
	delay_init(168);									//初始化延时函数
	
	LED_Init();											//LED指示灯初始化
	KC_Init();											//数字量输出初始化
	KR_Init();											//数字量输入初始化
	
	TIM2_Init(100);										//定时器2初始化
	
	USART1_Init(9600);									//维护串口初始化
	USART5_Init(9600);									//RS485串口
	USART6_Init(115200);								//4G模块串口
	
	RX8010S_Init();										//外部RTC初始化
	CAT25X_Init();										//外部EEPROM初始化
	W25QXX_Init();										//外部FLASH初始化
	HT7038_Init();										//电能计量芯片初始化
	EC20_Init();										//4G模块初始化
	

	ReadTotalConfigurationParameters();				//读取所有配置参数

	IWDG_Feed_Thread();									//上电第一次喂看门狗

	AppObjCreate();										//创建消息队列
	AppTaskCreate();									//创建任务

    vTaskStartScheduler();          					//开启任务调度
}














