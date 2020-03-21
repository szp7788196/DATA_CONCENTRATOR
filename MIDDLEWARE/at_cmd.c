#include "string.h"
#include "ringbuf.h"
#include "at_cmd.h"
#include "delay.h"
#include <stdio.h>
#include <time.h>

sendmsg callback = NULL;
volatile char *flag_ok = NULL;
RingBuf *result_ptr = NULL;

void register_cmd_handler(sendmsg func,void *result_buf, volatile char *flag)
{
	callback = func;
	
	result_ptr = (struct ringbuf *)result_buf;
	
	flag_ok = flag;
}

char AT_SendCmd(char* cmd, uint8_t *result,uint16_t waittime,uint8_t retry,uint16_t timeout)
{
	char *msg_p = NULL;
	time_t  newtime = 0;
	uint8_t retry_num = 0,retryflag = 0;

	if(callback == NULL || result_ptr == NULL || flag_ok == NULL)
		return 2;

	*flag_ok = 0;

	ringbuf_clear(result_ptr);//清除之前可能残留的信息
#ifdef DEBUG_LOG
	printf("cmd:%s\r\n",cmd);
#endif
	callback((uint8_t*)cmd, strlen((const char *)cmd));

	while(1)
	{
		if(newtime >= timeout)
		{
			if(++ retry_num > retry)
				return 0;

			retryflag = 1;
		}

		if(*flag_ok == 1)
		{
#ifdef DEBUG_LOG
			printf("cmd_rsp:%s",result_ptr->data);
#endif
			*flag_ok = 0;

			msg_p = strstr((char *)result_ptr->data,(char *)result);

			if(msg_p != NULL)
			{
//				ringbuf_clear(result_ptr);
				break;
			}
			else
			{
//				msg_p = strstr((char *)result_ptr->data,"ERROR");
//				ringbuf_clear(result_ptr);

//				if(msg_p != NULL)
//					return 2;
			}
		}
		else
		{
			newtime ++;
		}

		if(retryflag == 1)
		{
			retryflag = 0;
			newtime = 0;

			if(retry_num > 0 && retry_num < retry + 1)
			{
#ifdef DEBUG_LOG
				printf("retry cmd:%s",cmd);
#endif
				callback((uint8_t*)cmd, strlen((const char *)cmd));
			}
		}

		delay_ms(10);
	}

	delay_ms(waittime);

	return 1;
}

char AT_SendData(uint8_t* data, uint16_t len,uint8_t *result,uint16_t waittime,uint8_t retry,uint16_t timeout)
{
	char *msg_p = NULL;
	time_t  newtime = 0;
	uint8_t retry_num = 0,retryflag = 0;

	if(callback == NULL || result_ptr == NULL || flag_ok == NULL)
		return 2;

	*flag_ok = 0;

	ringbuf_clear(result_ptr);//清除之前可能残留的信息
#ifdef DEBUG_LOG
	printf("cmd:%s\r\n",cmd);
#endif
	callback((uint8_t*)data, len);

	while(1)
	{
		if(newtime >= timeout)
		{
			if(++ retry_num > retry)
				return 0;

			retryflag = 1;
		}

		if(*flag_ok == 1)
		{
#ifdef DEBUG_LOG
			printf("cmd_rsp:%s",result_ptr->data);
#endif
			*flag_ok = 0;

			msg_p = strstr((char *)result_ptr->data,(char *)result);

			if(msg_p != NULL)
			{
//				ringbuf_clear(result_ptr);
				break;
			}
			else
			{
//				msg_p = strstr((char *)result_ptr->data,"ERROR");
//				ringbuf_clear(result_ptr);

//				if(msg_p != NULL)
//					return 2;
			}
		}
		else
		{
			newtime ++;
		}

		if(retryflag == 1)
		{
			retryflag = 0;
			newtime = 0;

			if(retry_num > 0 && retry_num < retry + 1)
			{
#ifdef DEBUG_LOG
				printf("retry cmd:%s",cmd);
#endif
				callback((uint8_t*)data, len);
			}
		}

		delay_ms(10);
	}

	delay_ms(waittime);

	return 1;
}

























