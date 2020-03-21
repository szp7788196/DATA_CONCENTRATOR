#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

extern SemaphoreHandle_t xMutex_spi;

void SPI2_Init(void);
void SPI3_Init(void);
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler);
void SPI3_SetSpeed(u8 SPI_BaudRatePrescaler);
u8 SPI2_ReadWriteByte(u8 TxData);
u8 SPI3_ReadWriteByte(u8 TxData);
#endif

