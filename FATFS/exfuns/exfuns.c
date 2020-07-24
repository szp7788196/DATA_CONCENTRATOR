#include "string.h"
#include "exfuns.h"
#include "fattester.h"
#include "usart.h"
#include "common.h"


///////////////////////////////公共文件区,使用malloc的时候////////////////////////////////////////////
FATFS *fs;//逻辑磁盘工作区.
FATFS *fs1;//逻辑磁盘工作区.
FIL *file;	  		//文件1
UINT br,bw;			//读写变量
FILINFO fileinfo;	//文件信息
DIR dir;  			//目录

u8 *fatbuf;			//SD卡数据缓存区
///////////////////////////////////////////////////////////////////////////////////////

FRESULT init_mount_flash(void)
{
	FRESULT res = FR_OK;

	exfuns_init();

	res = f_mount(fs,"1:",1); 							//挂载FLASH.

	if(res == FR_NO_FILESYSTEM)							//FLASH磁盘,FAT文件系统错误,重新格式化FLASH
	{
		res = f_mkfs("1:",1,4096);						//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇

		if(res == FR_OK)
		{
			f_setlabel((const TCHAR *)"1:BJLINKTECH");	//设置Flash磁盘的名字为：ALIENTEK

			res = mf_opendir("1:");

			if(res == FR_OK)
			{
				res = mf_mkdir("1:CONCEN");
				res = mf_mkdir("1:CONCEN/ALARM");
				res = mf_mkdir("1:CONCEN/EVENT");
				res = mf_mkdir("1:CONCEN/JOURNAL");
				res = mf_mkdir("1:CONCEN/STATE");

				res = mf_mkdir("1:LAMP");
				res = mf_mkdir("1:LAMP/ALARM");
				res = mf_mkdir("1:LAMP/EVENT");
				res = mf_mkdir("1:LAMP/JOURNAL");
				res = mf_mkdir("1:LAMP/STATE");

				res = mf_mkdir("1:RELAY");
				res = mf_mkdir("1:RELAY/ALARM");
				res = mf_mkdir("1:RELAY/EVENT");
				res = mf_mkdir("1:RELAY/JOURNAL");
				res = mf_mkdir("1:RELAY/STATE");

				res = mf_mkdir("1:INPUT");
				res = mf_mkdir("1:INPUT/ALARM");
				res = mf_mkdir("1:INPUT/EVENT");
				res = mf_mkdir("1:INPUT/JOURNAL");
				res = mf_mkdir("1:INPUT/STATE");

				res = mf_mkdir("1:METER");
				res = mf_mkdir("1:METER/ALARM");
				res = mf_mkdir("1:METER/EVENT");
				res = mf_mkdir("1:METER/JOURNAL");
				res = mf_mkdir("1:METER/STATE");

				res = mf_mkdir("1:LUMETER");
				res = mf_mkdir("1:LUMETER/ALARM");
				res = mf_mkdir("1:LUMETER/EVENT");
				res = mf_mkdir("1:LUMETER/JOURNAL");
				res = mf_mkdir("1:LUMETER/STATE");

				res = mf_closedir();
			}
		}
	}

	return res;
}

//为exfuns申请内存
//返回值:0,成功
//1,失败
u8 exfuns_init(void)
{
	fs = (FATFS*)pvPortMalloc(sizeof(FATFS));	//为磁盘i工作区申请内存

	if(!fs)
	{
		return 1;
	}

	file = (FIL*)pvPortMalloc(sizeof(FIL));		//为file申请内存

	fatbuf = (u8*)pvPortMalloc(4096);				//为fatbuf申请内存

	if(file && fatbuf)
	{
		return 0;  								//申请有一个失败,即失败.
	}
	else
	{
		return 1;
	}
}


//得到磁盘剩余容量
//drv:磁盘编号("0:"/"1:")
//total:总容量	 （单位KB）
//free:剩余容量	 （单位KB）
//返回值:0,正常.其他,错误代码
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust = 0, fre_sect = 0, tot_sect = 0;
    //得到磁盘信息及空闲簇数量
    res = (u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);

    if(res == 0)
	{
	    tot_sect = (fs1->n_fatent-2) * fs1->csize;	//得到总扇区数
	    fre_sect = fre_clust * fs1->csize;			//得到空闲扇区数

#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
		tot_sect *= fs1->ssize/512;
		fre_sect *= fs1->ssize/512;
#endif
		*total = tot_sect >> 1;	//单位为KB
		*free = fre_sect >> 1;	//单位为KB
 	}

	return res;
}











