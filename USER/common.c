#include "common.h"
#include "concentrator_conf.h"
#include "relay_conf.h"
#include "input_collector_conf.h"
#include "electricity_meter_conf.h"
#include "e_meter.h"

SemaphoreHandle_t  xMutex_SPI2 = NULL;
SemaphoreHandle_t  xMutex_RTC = NULL;
SemaphoreHandle_t  xMutex_Push_xQueue_ServerFrameRx = NULL;
SemaphoreHandle_t  xMutex_Push_xQueue_ServerFrameTx = NULL;
SemaphoreHandle_t  xMutex_SPI_FLASH = NULL;
SemaphoreHandle_t  xMutex_Push_xQueue_AlarmReportSend = NULL;
SemaphoreHandle_t  xMutex_Push_xQueue_AlarmReportStore = NULL;
SemaphoreHandle_t  xMutex_TransServerFrameStruct = NULL;
SemaphoreHandle_t  xMutex_RelayStrategy = NULL;
SemaphoreHandle_t  xMutex_RelayAppointment = NULL;
SemaphoreHandle_t  xMutex_Rs485Rs485Frame = NULL;






QueueHandle_t xQueue_ServerFrameRx = NULL;
QueueHandle_t xQueue_4gFrameTx = NULL;
QueueHandle_t xQueue_WifiFrameTx = NULL;
QueueHandle_t xQueue_EthFrameTx = NULL;
QueueHandle_t xQueue_NB_IoTFrameTx = NULL;
QueueHandle_t xQueue_ConcentratorFrameStruct = NULL;
QueueHandle_t xQueue_LampControllerFrameStruct = NULL;
QueueHandle_t xQueue_RelayFrameStruct = NULL;
QueueHandle_t xQueue_InputCollectorFrameStruct = NULL;
QueueHandle_t xQueue_ElectricityMeterFrameStruct = NULL;
QueueHandle_t xQueue_LumeterFrameStruct = NULL;
QueueHandle_t xQueue_AlarmReportSend = NULL;
QueueHandle_t xQueue_AlarmReportStore = NULL;
QueueHandle_t xQueue_AlarmReportRead = NULL;
QueueHandle_t xQueue_HistoryRecordRead = NULL;
QueueHandle_t xQueue_RelayModuleState = NULL;
QueueHandle_t xQueue_InputCollectorState = NULL;
QueueHandle_t xQueue_ElectricityMeterState = NULL;
QueueHandle_t xQueue_Rs485Rs485Frame = NULL;
QueueHandle_t xQueue_RelayRs485Frame = NULL;
QueueHandle_t xQueue_InputCollectorRs485Frame = NULL;
QueueHandle_t xQueue_ElectricityMeterRs485Frame = NULL;
QueueHandle_t xQueue_LumeterRs485Frame = NULL;




time_t SysTick1s = 86400;
time_t SysTick10ms = 0;

//获取系统1秒滴答时钟
time_t GetSysTick1s(void)
{
	return SysTick1s;
}

//获取系统1秒滴答时钟
time_t GetSysTick10ms(void)
{
	return SysTick10ms;
}

//将小写字母转换为大写字母
u8 char_upper(u8 c)
{
	if(c<'A')
		return c;
	
	if(c>='a')
		return c-0x20;
	else 
		return c;
}

u8 myisspace(int x)
{
	if(x == ' ' || x == '\t' || x == '\n' || x == '\f' || x == '\b' || x == '\r')
		return 1;
	else  
		return 0;
}
u8 myisdigit(int x)
{
	if(x <= '9' && x >= '0')         
		return 1; 
	else 
		return 0;
 
}
int myatoi(const char *nptr)
{
	int c;              /* current char */
	int total;         /* current total */
	int sign;           /* if '-', then negative, otherwise positive */

	/* skip whitespace */
	while ( myisspace((int)(unsigned char)*nptr) )
		++nptr;

	c = (int)(unsigned char)*nptr++;
	sign = c;           /* save sign indication */
	if (c == '-' || c == '+')
		c = (int)(unsigned char)*nptr++;    /* skip sign */

	total = 0;

	while (myisdigit(c)) 
	{
		total = 10 * total + (c - '0');     /* accumulate digit */
		c = (int)(unsigned char)*nptr++;    /* get next char */
	}

	if (sign == '-')
		return -total;
	else
		return total;   /* return result, negated if necessary */
}


void myitoa(int num,char *str,int radix)
{
	/* 索引表 */
	char index[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	unsigned unum; /* 中间变量 */
	int i=0,j,k;
	char temp;

	/* 确定unum的值 */
	if(radix == 10 && num < 0) /* 十进制负数 */
	{
		unum = (unsigned) -num;
		str[i ++] = '-';
	}
	else
	{
		unum=(unsigned)num; /* 其它情况 */
	}
	/* 逆序 */
	do
	{
		str[i ++] = index[unum % (unsigned)radix];
		unum /= radix;
	}
	while(unum);

	str[i] = '\0';
	/* 转换 */
	if(str[0]=='-')
	{
		k=1; /* 十进制负数 */
	}
	else
	{
		k=0;
	}
	/* 将原来的“/2”改为“/2.0”，保证当num在16~255之间，radix等于16时，也能得到正确结果 */

	for(j = k; j <= (i - k - 1) / 2.0f; j ++)
	{
		temp = str[j];
		str[j] = str[i - j - 1];
		str[i - j - 1] = temp;
	}
}

//获得整数的位数
u8 GetDatBit(u32 dat)
{
	u8 j = 1;
	u32 i;
	i = dat;
	while(i >= 10)
	{
		j ++;
		i /= 10;
	}
	return j;
}

//用个位数换算出一个整数 1 10 100 1000......
u32 GetADV(u8 len)
{
	u32 count = 1;
	if(len == 1)
	{
		return 1;
	}
	else
	{
		len --;
		while(len --)
		{
			count *= 10;
		}
	}
	return count;
}

//整数转换为字符串
void IntToString(u8 *DString,u32 Dint,u8 zero_num)
{
	u16 i = 0;
	u8 j = GetDatBit(Dint);
	for(i = 0; i < GetDatBit(Dint) + zero_num; i ++)
	{
		DString[i] = Dint / GetADV(j) % 10 + 0x30;
		j --;
	}
}

void TimeToString(u8 *str,u16 year, u8 month, u8 date, u8 hour, u8 minute, u8 second)
{
	if(str == NULL)
	{
		return;
	}

	*(str + 0) = (year / 1000) + 0x30;
	*(str + 1) = (year / 100) % 10 + 0x30;
	*(str + 2) = (year / 10) % 10 + 0x30;
	*(str + 3) = year % 10 + 0x30;

	*(str + 4) = (month / 10) % 10 + 0x30;
	*(str + 5) = month % 10 + 0x30;

	*(str + 6) = (date / 10) % 10 + 0x30;
	*(str + 7) = date % 10 + 0x30;

	*(str + 8) = (hour / 10) % 10 + 0x30;
	*(str + 9) = hour % 10 + 0x30;

	*(str + 10) = (minute / 10) % 10 + 0x30;
	*(str + 11) = minute % 10 + 0x30;

	*(str + 12) = (second / 10) % 10 + 0x30;
	*(str + 13) = second % 10 + 0x30;
}

void Int4BitToString(u8 *str,u16 num)
{
	if(str == NULL)
	{
		return;
	}

	*(str + 0) = (num / 1000) + 0x30;
	*(str + 1) = (num / 100) % 10 + 0x30;
	*(str + 2) = (num / 10) % 10 + 0x30;
	*(str + 3) = num % 10 + 0x30;
}

/*
// C prototype : void HexToStr(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - 存放目标字符串
// [IN] pbSrc - 输入16进制数的起始地址
// [IN] nLen - 16进制数的字节数
// return value:
// remarks : 将16进制数转化为字符串
*/
void HexToStr(char *pbDest, u8 *pbSrc, u16 len)
{
	char ddl,ddh;
	int i;

	for (i = 0; i < len; i ++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pbDest[i * 2] = ddh;
		pbDest[i * 2 + 1] = ddl;
	}

	pbDest[len * 2] = '\0';
}

//小写字母转换为大写字母。
int my_toupper( int ch)
{

	if ((unsigned int)(ch - 'a') < 26)
		ch += 'A' - 'a';

	return ch;
}

/*
// C prototype : void StrToHex(BYTE *pbDest, BYTE *pbSrc, int nLen)
// parameter(s): [OUT] pbDest - 输出缓冲区
// [IN] pbSrc - 字符串
// [IN] nLen - 16进制数的字节数(字符串的长度/2)
// return value:
// remarks : 将字符串转化为16进制数
*/
void StrToHex(u8 *pbDest, char *pbSrc, u16 len)
{
	char h1,h2;
	u8 s1,s2;
	int i;

	for (i = 0; i < len; i ++)
	{
		h1 = pbSrc[2 * i];
		h2 = pbSrc[2 * i + 1];

		s1 = my_toupper(h1) - 0x30;
		if (s1 > 9)
		s1 -= 7;

		s2 = my_toupper(h2) - 0x30;
		if (s2 > 9)
		s2 -= 7;

		pbDest[i] = s1 * 16 + s2;
	}
}

//CRC32
u32 CRC32Extend(const u8 *buf, u32 size, u32 temp,u8 flag)
{
	uint32_t i, crc,crc_e;
	
	crc = temp;
	for (i = 0; i < size; i++)
	{
		crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
	}
	
	if(flag != 0)
	{
		crc_e = crc^0xFFFFFFFF;
	}
	else if(flag == 0)
	{
		crc_e = crc;
	}
	return crc_e;
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
u16 CRC16(u8 *puchMsgg,u16 usDataLen)
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

//闰年判断
u8 leap_year_judge(u16 year)
{
	u16 leap = 0;

	if(year % 400 == 0)
	{
		leap = 1;
	}
    else
    {
        if(year % 4 == 0 && year % 100 != 0)
		{
			leap = 1;
		}
        else
		{
			leap = 0;
		}
	}

	return leap;
}

//闰年判断 返回当前年月日 在一年中的天数
u32 get_days_by_calendar(u16 year,u8 month,u8 date)
{
	u16 i = 0;
	u8 leap = 0;
	u32 days = 0;
	u8 x[13]={0,31,29,31,30,31,30,31,31,30,31,30,31};

	for(i = 2000; i <= year; i ++)
	{
		leap = leap_year_judge(i);

		if(leap == 1)
		{
			days += 366;
		}
		else if(leap == 0)
		{
			days += 365;
		}
	}

	leap = leap_year_judge(year);

	if(leap == 1)
	{
		x[2] = 29;
	}
	else if(leap == 0)
	{
		x[2] = 28;
	}

	for(i = 1; i < month; i ++)
	{
		days += x[i];			//整月的天数
	}

	days += (u16)date;			//日的天数

	return days;
}

u32 get_minutes_by_calendar(u8 month,u8 date,u8 hour,u8 minute)
{
	u8 i = 0;
	u32 minutes = 0;
	u16 days = 0;
	u8 x[13]={0,31,29,31,30,31,30,31,31,30,31,30,31};

	for(i = 1; i < month; i ++)
	{
		days += x[i];			//整月的天数
	}
	
	days += (u16)date;			//日的天数
	
	minutes = 1440 * days + hour * 60 + minute;
	
	return minutes;
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

		*str = (u8 *)mymalloc(sizeof(u8) * len + 1);
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
			myfree(*str);
			*str = (u8 *)mymalloc(sizeof(u8) * new_len + 1);

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
	ReadConcentratorGateWayID();			//读取集控器网关ID
	ReadRunMode();							//读取集控器运行模式
	ReadDefaultSwitchTime();				//读取默认开关灯时间
	ReadConcentratorBasicConfig();			//读取集控器基础配置参数
	ReadConcentratorLocalNetConfig();			//读取集控器本地网络配置
	ReadConcentratorAlarmConfig();			//读取集控器告警配置参数
	ReadConcentratorLocationConfig();		//读取经纬度年表
	ReadFrameWareState();					//读取固件升级状态
	
	ReadRelayModuleConfig();				//读取继电器模块配置
	ReadRelayAlarmConfig();					//读取继电器模块告警参数配置
//	ReadRelayAppointmentGroup();			//读取继电器模块预约控制
//	ReadRelayStrategyGroups();				//读取继电器模块策略组
//	ReadRelayStrategyGroupSwitch();			//读取继电器模块策略组切换配置
	
	ReadInputCollectorBasicConfig();		//读取输入量检测模块基础配置参数
	ReadInputCollectorConfig();				//读取输入量检测模块基础配置参数
	ReadInputCollectorAlarmConfig();		//读取输入量检测模块告警配置参数
	
	ReadBulitInMeterRatio();				//读取内置电表变比
	ReadElectricityMeterBasicConfig();		//读取电表基础配置参数
	ReadElectricityMeterConfig();			//读取电表基础配置参数
	ReadElectricityMeterAlarmConfig();		//读取电表告警配置参数
}

















