#ifndef __WATCH_DOG_H
#define __WATCH_DOG_H

#include "stm32f4xx.h"  


void IWDG_Init(u8 prer,u16 rlr);
void IWDG_Feed_Thread(void);

























#endif

