#include "key.h"
#include "delay.h" 


//按键初始化函数
void KEY_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
} 
//按键处理函数

u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	
	if(mode)
		key_up=1;  //支持连按	
	
	if(key_up && (KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0))
	{
		delay_ms(10);//去抖动
		
		key_up=0;
		
		if(KEY1==0)
			return 1;
		
		else if(KEY2==0)
			return 2;
		
		else if(KEY3==0)
			return 3;
		
		else if(KEY4==0)
			return 4;
		
	}
	else if(KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1)
		key_up=1;
    
 	return 0;// 无按键按下
}




















