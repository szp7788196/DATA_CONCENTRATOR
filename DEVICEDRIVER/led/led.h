#ifndef __LED_H
#define __LED_H
#include "sys.h"



#define LED_RUN		PAout(3)
#define LED_WIFI	PDout(12)
#define LED_ZIGBEE	PDout(14)
#define LED_RS485	PGout(2)
#define LED_4G		PDout(0)
#define LED_ETH		PDout(1)





void LED_Init(void);//≥ı ºªØ		 				    
#endif
