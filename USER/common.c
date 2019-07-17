#include "common.h"


time_t SysTick1s = 0;

//32位CRC校验
u32 CRC32( const u8 *buf, u32 size)
{
     uint32_t i, crc;
     crc = 0xFFFFFFFF;
     for (i = 0; i < size; i++)
      crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
     return crc^0xFFFFFFFF;
}

/*****************************************************
函数：u16 CRC16(u8 *puchMsgg,u8 usDataLen)
功能：CRC校验用函数
参数：puchMsgg是要进行CRC校验的消息，usDataLen是消息中字节数
返回：计算出来的CRC校验码。
*****************************************************/
u16 CRC16(u8 *puchMsgg,u8 usDataLen)
{
    u8 uchCRCHi = 0xFF ; 											//高CRC字节初始化
    u8 uchCRCLo = 0xFF ; 											//低CRC 字节初始化
    u8 uIndex ; 													//CRC循环中的索引
    while (usDataLen--) 											//传输消息缓冲区
    {
		uIndex = uchCRCHi ^ *puchMsgg++; 							//计算CRC
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
    }
    return ((uchCRCHi << 8) | uchCRCLo);
}

//计算校验和
u8 CalCheckSum(u8 *buf, u16 len)
{
	u8 sum = 0;
	u16 i = 0;

	for(i = 0; i < len; i ++)
	{
		sum += *(buf + i);
	}

	return sum;
}












	











