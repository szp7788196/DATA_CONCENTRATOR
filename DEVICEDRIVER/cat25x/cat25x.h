#ifndef __CAT25X_H
#define __CAT25X_H
#include "sys.h"
#include "common.h"


#define CAT25X_CS			PBout(12)

//÷∏¡Ó±Ì
#define CAT25X_WRITE_EN		0x06
#define CAT25X_WRITE_DIS	0x04
#define CAT25X_READ_SR		0x05
#define CAT25X_WRITE_SR		0x01
#define CAT25X_READ_MMY		0x03
#define CAT25X_WRITE_MMY	0x02

#define CAT25X_PAGE_SIZE         	64
#define CAT25X_PAGE_NUM         	1024

void CAT25X_Init(void);
//void CAT25X_Write_Enable(void);
//void CAT25X_Write_Disable(void);
//void CAT25X_Write_SR(u8 stat);
//u8 CAT25X_Read_SR(void);
void CAT25X_Read(u8* Buff,u16 ReadAddr,u16 Num);
void CAT25X_Write_Page(u8* Buff,u16 ReadAddr,u16 Num);
void CAT25X_Write(u8* Buff,u16 ReadAddr,u16 Num);





























#endif
