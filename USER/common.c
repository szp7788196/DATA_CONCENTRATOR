#include "common.h"
#include "concentrator.h"

SemaphoreHandle_t  xMutex_SPI2 = NULL;
SemaphoreHandle_t  xMutex_RTC = NULL;
SemaphoreHandle_t  xMutex_Push_xQueue_ServerFrameRx = NULL;
SemaphoreHandle_t  xMutex_Push_xQueue_ServerFrameTx = NULL;






QueueHandle_t xQueue_ServerFrameRx = NULL;
QueueHandle_t xQueue_4gFrameTx = NULL;
QueueHandle_t xQueue_WifiFrameTx = NULL;
QueueHandle_t xQueue_EthFrameTx = NULL;
QueueHandle_t xQueue_NB_IoTFrameTx = NULL;

QueueHandle_t xQueue_ConcentratorFrameStruct = NULL;
QueueHandle_t xQueue_LampControllerFrameStruct = NULL;
QueueHandle_t xQueue_RelayFrameStruct = NULL;
QueueHandle_t xQueue_InputCollectorFrameStruct = NULL;
QueueHandle_t xQueue_ElectricMeterFrameStruct = NULL;
QueueHandle_t xQueue_LumeterFrameStruct = NULL;





time_t SysTick1s = 0;

//获取系统1秒滴答时钟
time_t GetSysTick1s(void)
{
	time_t sec = 0;

	sec = SysTick1s;

	return sec;
}



//32位CRC校验
u32 CRC32(const u8 *buf, u32 size)
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

//在str1中查找str2，失败返回0xFF,成功返回str2首个元素在str1中的位置
u16 MyStrstr(u8 *str1, u8 *str2, u16 str1_len, u16 str2_len)
{
	u16 len = str1_len;
	if(str1_len == 0 || str2_len == 0)
	{
		return 0xFFFF;
	}
	else
	{
		while(str1_len >= str2_len)
		{
			str1_len --;
			if (!memcmp(str1, str2, str2_len))
			{
				return len - str1_len - 1;
			}
			str1 ++;
		}
		return 0xFFFF;
	}
}

unsigned short find_str(unsigned char *s_str, unsigned char *p_str, unsigned short count, unsigned short *seek)
{
	unsigned short _count = 1;
    unsigned short len = 0;
    unsigned char *temp_str = NULL;
    unsigned char *temp_ptr = NULL;
    unsigned char *temp_char = NULL;

	(*seek) = 0;
    if(0 == s_str || 0 == p_str)
        return 0;
    for(temp_str = s_str; *temp_str != '\0'; temp_str++)
    {
        temp_char = temp_str;

        for(temp_ptr = p_str; *temp_ptr != '\0'; temp_ptr++)
        {
            if(*temp_ptr != *temp_char)
            {
                len = 0;
                break;
            }
            temp_char++;
            len++;
        }
        if(*temp_ptr == '\0')
        {
            if(_count == count)
                return len;
            else
            {
                _count++;
                len = 0;
            }
        }
        (*seek) ++;
    }
    return 0;
}

int search_str(unsigned char *source, unsigned char *target)
{
	unsigned short seek = 0;
    unsigned short len;
    len = find_str(source, target, 1, &seek);
    if(len == 0)
        return -1;
    else
        return len;
}

unsigned short get_str1(unsigned char *source, unsigned char *begin, unsigned short count1, unsigned char *end, unsigned short count2, unsigned char *out)
{
	unsigned short i;
    unsigned short len1;
    unsigned short len2;
    unsigned short index1 = 0;
    unsigned short index2 = 0;
    unsigned short length = 0;
    len1 = find_str(source, begin, count1, &index1);
    len2 = find_str(source, end, count2, &index2);
    length = index2 - index1 - len1;
    if((len1 != 0) && (len2 != 0))
    {
        for( i = 0; i < index2 - index1 - len1; i++)
            out[i] = source[index1 + len1 + i];
        out[i] = '\0';
    }
    else
    {
        out[0] = '\0';
    }
    return length;
}

unsigned short get_str2(unsigned char *source, unsigned char *begin, unsigned short count, unsigned short length, unsigned char *out)
{
	unsigned short i = 0;
    unsigned short len1 = 0;
    unsigned short index1 = 0;
    len1 = find_str(source, begin, count, &index1);
    if(len1 != 0)
    {
        for(i = 0; i < length; i++)
            out[i] = source[index1 + len1 + i];
        out[i] = '\0';
    }
    else
    {
        out[0] = '\0';
    }
    return length;
}

unsigned short get_str3(unsigned char *source, unsigned char *out, unsigned short length)
{
	unsigned short i = 0;
    for (i = 0 ; i < length ; i++)
    {
        out[i] = source[i];
    }
    out[i] = '\0';
    return length;
}

//将内存中的数据拷贝到指定指针所指的内存中
u8 GetMemoryForSpecifyPointer(u8 **str,u16 size, u8 *memory)
{
	u8 ret = 0;
	u8 len = 0;
	u8 new_len = 0;

	if(*str == NULL)
	{
		len = size;

		*str = (u8 *)pvPortMalloc(sizeof(u8) * len + 1);
	}

	if(*str != NULL)
	{
		len = strlen((char *)*str);

		new_len = size;

		if(len == new_len)
		{
			memset(*str,0,new_len + 1);

			memcpy(*str,memory,new_len);

			ret = 1;
		}
		else
		{
			vPortFree(*str);
			*str = (u8 *)pvPortMalloc(sizeof(u8) * new_len + 1);

			if(*str != NULL)
			{
				memset(*str,0,new_len + 1);

				memcpy(*str,memory,new_len);

				len = new_len;
				new_len = 0;
				ret = 1;
			}
		}
	}

	return ret;
}








































void ReadTotalConfigurationParameters(void)
{
	ReadConcentratorBasicConfig();			//读取集控器基础配置参数
}





	











