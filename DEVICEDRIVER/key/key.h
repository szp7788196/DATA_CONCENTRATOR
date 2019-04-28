#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h" 
 

/*下面的方式是通过直接操作库函数方式读取IO*/
#define KEY1 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_6)
#define KEY2 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_7)
#define KEY3 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_8)
#define KEY4 		GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_9)




#define KEY0_PRES 	1
#define KEY1_PRES	2
#define KEY2_PRES	3
#define WKUP_PRES   4

void KEY_Init(void);	//IO初始化
u8 KEY_Scan(u8);  		//按键扫描函数	

#endif
