#include "stmflash.h"
#include "delay.h"
#include "usart.h" 

 
 
//��ȡָ����ַ�İ���(16λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(vu32*)faddr; 
}


//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(4λ)��
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	
	for(i = 0; i < NumToRead; i ++)
	{
		pBuffer[i] = STMFLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
		
		ReadAddr += 4;//ƫ��4���ֽ�.	
	}
}

//��FLASH�е�ָ��λ�ö�ȡһ���ֽ�
u8 STMFLASH_ReadByte(u32 faddr)
{
	return *(vu8*)faddr;
}
//���ֽڶ�ȡFLASHָ����ַ
void STMFLASH_ReadBytes(u32 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	u16 i = 0;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadByte(ReadAddr);
		ReadAddr++;
	}
}














