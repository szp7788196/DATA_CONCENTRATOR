#include "ec20.h"
#include <stdlib.h>
#include <string.h>
#include "delay.h"
#include "at_cmd.h"
#include "fifo.h"
#include "common.h"
#include "usart6.h"


EC20Info_S EC20Info;
CONNECT_STATE_E EC20ConnectState = UNKNOW_STATE;


void EC20_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	EC20_NETMOD = 0;
	EC20_PWRKEY = 0;
	EC20_RESET = 0;
}

void ec20_soft_init(void)
{	
	RE_INIT:
	ec20_force_pwr_on();
	
	USART6_Init(115200);
	
	if(ec20_wait_pwr_on_ready(15) != 1)
		goto RE_INIT;
	
	if(ec20_get_AT_ATI() != 1)
		goto RE_INIT;
	
	if(ec20_set_AT_ATE0() != 1)
		goto RE_INIT;
	
	if(ec20_get_AT_CPIN() != 1)
		goto RE_INIT;
	
	if(ec20_get_AT_CGREG() != 1)
		goto RE_INIT;
	
	if(ec20_set_AT_QICSGP() != 1)
		goto RE_INIT;
	
	if(ec20_get_AT_CGSN() != 1)
		goto RE_INIT;
	
	if(ec20_get_AT_QCCID() != 1)
		goto RE_INIT;
	
	if(ec20_get_AT_CIMI() != 1)
		goto RE_INIT;
	
	if(ec20_get_AT_CGDCONT() != 1)
		goto RE_INIT;
	
//	if(ec20_get_AT_CNUM() != 1)
//		goto RE_INIT;
}

//EC20强制开机
u8 ec20_force_pwr_on(void)
{
	u8 ret = 1;
	
	ec20_reset();
	
	delay_ms(1000);
	
	RE_PWR_ON:
	ret = ec20_pwr_on();
	
	if(ret == 1)					//开机失败
	{
		delay_ms(1000);
		
		ret = ec20_pwr_off();
		
		if(ret == 0)				//关机成功
		{
			goto RE_PWR_ON;			//重新开机
		}
		else
		{
			ec20_reset();
			
			goto RE_PWR_ON;			//重新开机
		}
	}
	
	return ret;
}

//return 0开机成功，1，开机失败
u8 ec20_pwr_on(void)
{
	unsigned char cnt=0;

	delay_ms(100);

	EC20_PWRKEY = 1;

	delay_ms(600);

	EC20_PWRKEY = 0;

	while((EC20_STATUS != 0) && (cnt <= 80))
	{
		cnt++;

		delay_ms(100);
	}

	return EC20_STATUS;
}

//return 0关机成功，1，关机失败
u8 ec20_pwr_off(void)
{
	unsigned char cnt=0;

	EC20_PWRKEY = 0;

	delay_ms(50);

	EC20_PWRKEY = 1;

	delay_ms(800);

	EC20_PWRKEY = 0;

	while((EC20_STATUS == 0) && (cnt <= 40))
	{
		cnt++;

		delay_ms(1000);
	}

	return !EC20_STATUS;
}

void ec20_reset(void)
{
	EC20_RESET = 1;

	delay_ms(600);

	EC20_RESET = 0;
}

void ec20_send_data(unsigned char *buf,unsigned short len)
{
	USART6_Write(buf, len);
}

//等待开机成功
unsigned char ec20_wait_pwr_on_ready(u16 time_out_s)
{
	u16 cnt = time_out_s * 10;
	
	while((search_str((unsigned char *)result_ptr->data, "RDY") == -1) && cnt != 0)
	{
		delay_ms(100);
		
		cnt --;
	}
	
	if(cnt != 0)
	{
		cnt = 1;
	}
	
	return cnt;
}

//AT指令测试
unsigned char ec20_get_AT_ATI(void)
{
	unsigned char ret = 0;

    ret = AT_SendCmd("ATI\r\n", "OK", 100,0,TIMEOUT_1S);
	
	ringbuf_clear(result_ptr);

    return ret;
}

//关闭回显
unsigned char ec20_set_AT_ATE0(void)
{
	unsigned char ret = 0;

    ret = AT_SendCmd("ATE0\r\n", "OK", 100,0,TIMEOUT_1S);
	
	ringbuf_clear(result_ptr);

    return ret;
}

//查询SIM卡状态
unsigned char ec20_get_AT_CPIN(void)
{
	unsigned char ret = 0;

    ret = AT_SendCmd("AT+CPIN?\r\n", "+CPIN: READY", 100,0,TIMEOUT_7S);
	
	ringbuf_clear(result_ptr);

    return ret;
}

//获取网络注册状态
unsigned char ec20_get_AT_CGREG(void)
{
	unsigned char ret = 0;

    ret = AT_SendCmd("AT+CGREG?\r\n", "+CGREG: 0,1", 100,30,TIMEOUT_1S);
	
	ringbuf_clear(result_ptr);

	if(ret != 1)
	{
		ret = AT_SendCmd("AT+CGREG?\r\n", "+CGREG: 0,5", 100,30,TIMEOUT_1S);
		
		ringbuf_clear(result_ptr);
	}

    return ret;
}

//配置TCP/IP上下文参数
unsigned char ec20_set_AT_QICSGP(void)
{
	unsigned char ret = 0;

    ret = AT_SendCmd("AT+QICSGP=1\r\n", "OK", 100,0,TIMEOUT_7S);
	
	ringbuf_clear(result_ptr);

    return ret;
}

//获取IMEI号
unsigned char ec20_get_AT_CGSN(void)
{
	unsigned char ret = 0;
	char buf[32];

    if(AT_SendCmd("AT+CGSN\r\n", "OK", 100,0,TIMEOUT_1S) == 1)
    {
		memset(buf,0,32);

		get_str1((u8 *)result_ptr->data, "\r\n", 1, "\r\n", 2, (unsigned char *)buf);

		if(strlen(buf) == 15)
		{
			GetMemoryForSpecifyPointer((u8 **)&EC20Info.imei,15,(u8 *)buf);

			ret = 1;
		}
    }
	
	ringbuf_clear(result_ptr);

    return ret;
}

//获取ICCID
unsigned char ec20_get_AT_QCCID(void)
{
	unsigned char ret = 0;
	char buf[32];

    if(AT_SendCmd("AT+QCCID\r\n", "OK", 100,0,TIMEOUT_1S) == 1)
    {
		memset(buf,0,32);

		get_str1((u8 *)result_ptr->data, "QCCID: ", 1, "\r\n", 2, (unsigned char *)buf);

		if(strlen(buf) == 20)
		{
			GetMemoryForSpecifyPointer((u8 **)&EC20Info.iccid,20,(u8 *)buf);

			ret = 1;
		}
    }
	
	ringbuf_clear(result_ptr);

    return ret;
}

//获取IMSI
unsigned char ec20_get_AT_CIMI(void)
{
	unsigned char ret = 0;
	char buf[32];

    if(AT_SendCmd("AT+CIMI\r\n", "OK", 100,0,TIMEOUT_1S) == 1)
    {
		memset(buf,0,32);

		get_str1((u8 *)result_ptr->data, "\r\n", 1, "\r\n", 2, (unsigned char *)buf);

		if(strlen(buf) == 15)
		{
			GetMemoryForSpecifyPointer((u8 **)&EC20Info.imsi,15,(u8 *)buf);

			ret = 1;
		}
    }
	
	ringbuf_clear(result_ptr);

    return ret;
}

//获取APN
unsigned char ec20_get_AT_CGDCONT(void)
{
	unsigned char ret = 0;
	char buf[33];

    if(AT_SendCmd("AT+CGDCONT?\r\n", "+CGDCONT", 100,10,TIMEOUT_1S) == 1)
    {
		memset(buf,0,33);

		get_str1((u8 *)result_ptr->data, "\"", 3, "\"", 4, (unsigned char *)buf);

		if(strlen(buf) <= 32)
		{
			GetMemoryForSpecifyPointer((u8 **)&EC20Info.apn,strlen(buf),(u8 *)buf);

			ret = 1;
		}
    }
	
	ringbuf_clear(result_ptr);

    return ret;
}

//获取CNUM手机号
unsigned char ec20_get_AT_CNUM(void)
{
	unsigned char ret = 0;
	char buf[32];

    if(AT_SendCmd("AT+CNUM\r\n", "+CNUM: ", 100,10,TIMEOUT_1S) == 1)
    {
		memset(buf,0,32);

		get_str1((u8 *)result_ptr->data, "\"", 1, "\"", 2, (unsigned char *)buf);

		if(strlen(buf) == 14)
		{
			GetMemoryForSpecifyPointer((u8 **)&EC20Info.cnum,15,(u8 *)buf);

			ret = 1;
		}
    }
	
	ringbuf_clear(result_ptr);

    return ret;
}

//激活PDP上下文
unsigned char ec20_set_AT_QIACT(void)
{
	unsigned char ret = 0;
	unsigned char cnt = 0;

	RE_SEND:
    ret = AT_SendCmd("AT+QIACT=1\r\n", "OK", 100,0,TIMEOUT_160S);
	
	ringbuf_clear(result_ptr);

	if(ret != 1)
	{
		cnt ++;

		if(cnt == 3)
		{
			return 0;
		}

		ret = AT_SendCmd("AT+QIDEACT=1\r\n", "OK", 100,0,TIMEOUT_50S);
		
		ringbuf_clear(result_ptr);

		if(ret == 1)
		{
			goto RE_SEND;
		}
	}

    return ret;
}

//停用PDP上下文
unsigned char ec20_get_AT_QIACT(void)
{
	unsigned char ret = 0;

    ret = AT_SendCmd("AT+QIACT?\r\n", "OK", 100,0,TIMEOUT_50S);

	if(ret == 1)
	{
		if(search_str((unsigned char *)result_ptr->data, "+QIACT: ") != -1)
		{
			ret = 1;
		}
		else
		{
			ret = 0;
		}
	}
	
	ringbuf_clear(result_ptr);

    return ret;
}



//建立socket连接
unsigned char ec20_set_AT_QIOPEN(char *addr, char *port)
{
	unsigned char ret = 0;
	unsigned char cnt = 0;
	unsigned short _port = 0;
	char cmd_tx_buf[64];

	_port = atoi(port);

	RE_CONNECT:
	memset(cmd_tx_buf,0,64);

	sprintf(cmd_tx_buf,"AT+QIOPEN=1,0,\"TCP\",\"%s\",%d,0,1\r\n",addr,_port);

    ret = AT_SendCmd(cmd_tx_buf, "+QIOPEN: ", 100,0,TIMEOUT_160S);

	if(ret == 1)
	{
		if(search_str((unsigned char *)result_ptr->data, "+QIOPEN: 0,0") != -1)
		{
			ret = 1;
		}
		else
		{
			RE_CLOSE:

			cnt ++;

			if(cnt == 3)
			{
				return 0;
			}

			ret = AT_SendCmd("AT+QICLOSE=0\r\n", "OK", 100,0,TIMEOUT_15S);

			if(ret == 1)
			{
				goto RE_CONNECT;
			}
			else
			{
				ret = 0;
			}
		}
	}
	else
	{
		goto RE_CLOSE;
	}
	
	ringbuf_clear(result_ptr);

    return ret;
}

//断开socket
unsigned char ec20_set_AT_QICLOSE(void)
{
	unsigned char ret = 0;

    ret = AT_SendCmd("AT+QICLOSE=0\r\n", "OK", 100,0,TIMEOUT_15S);
	
	ringbuf_clear(result_ptr);

    return ret;
}

CONNECT_STATE_E ec20_get_AT_QISTATE(void)
{
	u8 res = 0;
	CONNECT_STATE_E ret = UNKNOW_STATE;
	char buf[32];

    if(AT_SendCmd("AT+QISTATE=1,0\r\n", "OK", 100,3,TIMEOUT_1S) == 1)
    {
		if(search_str((unsigned char *)result_ptr->data, "+QISTATE:") != -1)
		{
			memset(buf,0,32);

			get_str1((u8 *)result_ptr->data, ",", 5, ",", 6, (unsigned char *)buf);

			ret = (CONNECT_STATE_E)atoi(buf);

			if(ret != UNKNOW_STATE && ret > CLOSING)
			{
				ret = UNKNOW_STATE;
			}
		}
    }
	else
	{
		ret = UNKNOW_STATE;
	}
	
	if(ret == UNKNOW_STATE)
	{
		res = ec20_get_AT_QIACT();
		
		if(res == 1)
		{
			ret = GOT_IP;
		}
	}
	
	ringbuf_clear(result_ptr);

    return ret;
}

//获取信号强度
unsigned char ec20_get_AT_CSQ(char *csq)
{
	u8 ret = 0;
	char buf[10];

	ret = AT_SendCmd("AT+CSQ\r\n", "OK", 100,0,TIMEOUT_1S);

	if(ret == 1)
	{
		memset(buf,0,10);

		get_str1((u8 *)result_ptr->data, " ", 1, ",", 1, (unsigned char *)buf);

		*csq = atoi(buf);

		if(*csq == 0 || *csq >= 99)
		{
			*csq = 0;
			ret = 0;
		}
		
		*csq = 113 - (*csq * 2);
		
		*csq = 0 - *csq;
	}
	
	ringbuf_clear(result_ptr);

	return ret;
}

//发送数据
unsigned char ec20_get_AT_QISEND(unsigned char *buf,unsigned short len)
{
	u8 ret = 0;

	char cmd_tx_buf[20];

	memset(cmd_tx_buf,0,20);

	sprintf(cmd_tx_buf,"AT+QISEND=0,%d\r\n",len);

    ret = AT_SendCmd(cmd_tx_buf, "> ", 100,0,TIMEOUT_1S);

	if(ret == 1)
	{
		ec20_send_data(buf,len);

		ret = AT_SendData(buf,len,"SEND OK",100,0,TIMEOUT_1S);
	}
	
	ringbuf_clear(result_ptr);

	return ret;
}

//从模块获取时间
unsigned char ec20_get_AT_CCLK(char *buf)
{
	unsigned char ret = 0;

    if(AT_SendCmd("AT+CCLK?\r\n", "OK", 100,0,TIMEOUT_1S) == 1)
    {
        if(search_str((unsigned char *)result_ptr->data, "+CCLK:") != -1)
		{
			get_str1((unsigned char *)result_ptr->data, "\"", 1, "\"", 2, (unsigned char *)buf);

			ret = 1;
		}
    }

    return ret;
}































