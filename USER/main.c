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
#include "plc.h"
#include "battery.h"
#include "concentrator_conf.h"
#include "exfuns.h"
#include "input_collector_conf.h"
#include "electricity_meter_conf.h"
#include "lamp_conf.h"
#include "lamp_event.h"
#include "lumeter_conf.h"


//u8 eprombuf[100];
//u16 i = 0;
RCC_ClocksTypeDef RCC_Clocks;
u16 siiize = 0;

int main(void)
{
//	SCB->VTOR = FLASH_BASE | 0x40000; 	/* Vector Table Relocation in Internal FLASH. */
//	IWDG_Init(6,1000);									//看门狗超时8秒

	RCC_GetClocksFreq(&RCC_Clocks);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		//设置系统中断优先级分组号：4。响应优先级不分组，抢占优先级分16个级别
	delay_init(168);									//初始化延时函数

	LED_Init();											//LED指示灯初始化
	KC_Init();											//数字量输出初始化
	KR_Init();											//数字量输入初始化
	
	ADC1_DMA_Init();									//ADC初始化

	TIM2_Init(100);										//定时器2初始化

	USART1_Init(115200);								//维护串口初始化
	USART2_Init(9600);									//PLC串口
	USART4_Init(115200);								//屏幕串口
	USART5_Init(9600,USART_Parity_No);					//RS485串口
	USART6_Init(115200);								//4G模块串口

	RX8010S_Init();										//外部RTC初始化
	CAT25X_Init();										//外部EEPROM初始化
	W25QXX_Init();										//外部FLASH初始化
	HT7038_Init();										//电能计量芯片初始化
	EC20_Init();										//4G模块初始化
	PLC_Init();											//PLC模块初始化
	
	
	siiize = sizeof(LumeterBasicConfig_S);
	siiize = sizeof(LumeterConfig_S);
	siiize = sizeof(LumeterAlarmConfig_S);


//	init_mount_flash();									//初始化并挂载flash

	ReadTotalConfigurationParameters();					//读取所有配置参数

	IWDG_Feed_Thread();									//上电第一次喂看门狗

	AppObjCreate();										//创建消息队列
	AppTaskCreate();									//创建任务

	vTaskStartScheduler();          					//开启任务调度
}














