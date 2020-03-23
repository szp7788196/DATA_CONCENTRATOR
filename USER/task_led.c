//指示灯处理任务
#include "task_led.h"
#include "delay.h"
#include "led.h"
#include "watch_dog.h"
#include "rx8010s.h"
#include "exfuns.h"
#include "ff.h"
#include "ht7038.h"



TaskHandle_t xHandleTaskLED = NULL;

u32 FreeHeapSize = 0;

void vTaskLED(void *pvParameters)
{
	u32 cnt = 0;
	u8 led_state = 0;
//	u8 res = 0;
//	u32 total = 0;
//	u32 free = 0;
//	char readbuf[48]={0};
//	char writebuf[16]="1234567890123456";
	
//	exfuns_init();
//	res=f_mount(fs,"1:",1); 							//挂载FLASH.
//	
//	if(res==0X0D)//FLASH磁盘,FAT文件系统错误,重新格式化FLASH
//	{
//		res=f_mkfs("1:",1,4096);//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
//		if(res==0)
//		{
//			f_setlabel((const TCHAR *)"1:BJLINKTECH");	//设置Flash磁盘的名字为：ALIENTEK
//		}
//		
//		delay_ms(1000);
//	}
//	
//	while(exf_getfree("1:",&total,&free))	//得到SD卡的总容量和剩余容量
//	{
//		delay_ms(100);
//	}
//	
//	res=f_open(&file,"1:test.txt", FA_OPEN_ALWAYS|FA_WRITE|FA_READ);
//	if(res == FR_OK)//文件打开成功
//	{
//		res=f_lseek(&file,f_size(&file));
//		res=f_write(&file,(void*)writebuf,16,&bw);
//		res=f_lseek(&file,f_size(&file));
//		res=f_write(&file,(void*)writebuf,16,&bw);
//		res=f_read(&file,(void*)readbuf,32,&br);
//		res=f_close(&file);
//	}

//	res=f_open(&file,"1:test.txt", FA_OPEN_ALWAYS|FA_READ);
//	if(res == FR_OK)//文件打开成功
//	{
//		res=f_read(&file,(void*)readbuf,32,&br);
//		f_close(&file);
//	}

	while(1)											//循环一次延时约20ms
	{
		FreeHeapSize = xPortGetFreeHeapSize();
		
		if(cnt % 10 == 0)								//每1秒喂一次看门狗
		{
			IWDG_Feed_Thread();
			
			RX8010S_Get_Time();
		}
		
		if(cnt % 5 == 0)								//运行灯以1秒的周期闪烁
		{
			led_state = ~led_state;
		}
		
		if(led_state)
		{
			LED_RUN = 0;
		}
		else
		{
			LED_RUN = 1;
		}
		
		cnt = (cnt + 1) & 0xFFFFFFFF;
		
		delay_ms(100);						//循环一次延时约20ms
	}
}

















