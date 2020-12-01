#include "stmflash.h"
#include "delay.h"
#include "usart.h" 

 
 
//读取指定地址的半字(16位数据) 
//faddr:读地址 
//返回值:对应数据.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}


//从指定地址开始读出指定长度的数据
//ReadAddr:起始地址
//pBuffer:数据指针
//NumToRead:字(4位)数
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	
	for(i = 0; i < NumToRead; i ++)
	{
		pBuffer[i] = STMFLASH_ReadWord(ReadAddr);//读取4个字节.
		
		ReadAddr += 4;//偏移4个字节.	
	}
}

//在FLASH中的指定位置读取一个字节
u8 STMFLASH_ReadByte(u32 faddr)
{
	return *(vu8*)faddr;
}
//按字节读取FLASH指定地址
void STMFLASH_ReadBytes(u32 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	u16 i = 0;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadByte(ReadAddr);
		ReadAddr++;
	}
}














