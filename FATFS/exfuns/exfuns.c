#include "string.h"
#include "exfuns.h"
#include "fattester.h"
#include "usart.h"
#include "common.h"


///////////////////////////////�����ļ���,ʹ��malloc��ʱ��////////////////////////////////////////////
FATFS *fs;//�߼����̹�����.
FATFS *fs1;//�߼����̹�����.
FIL *file;	  		//�ļ�1
UINT br,bw;			//��д����
FILINFO fileinfo;	//�ļ���Ϣ
DIR dir;  			//Ŀ¼

u8 *fatbuf;			//SD�����ݻ�����
///////////////////////////////////////////////////////////////////////////////////////

FRESULT init_mount_flash(void)
{
	FRESULT res = FR_OK;

	exfuns_init();

	res = f_mount(fs,"1:",1); 							//����FLASH.

	if(res == FR_NO_FILESYSTEM)							//FLASH����,FAT�ļ�ϵͳ����,���¸�ʽ��FLASH
	{
		res = f_mkfs("1:",1,4096);						//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����

		if(res == FR_OK)
		{
			f_setlabel((const TCHAR *)"1:BJLINKTECH");	//����Flash���̵�����Ϊ��ALIENTEK

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

//Ϊexfuns�����ڴ�
//����ֵ:0,�ɹ�
//1,ʧ��
u8 exfuns_init(void)
{
	fs = (FATFS*)pvPortMalloc(sizeof(FATFS));	//Ϊ����i�����������ڴ�

	if(!fs)
	{
		return 1;
	}

	file = (FIL*)pvPortMalloc(sizeof(FIL));		//Ϊfile�����ڴ�

	fatbuf = (u8*)pvPortMalloc(4096);				//Ϊfatbuf�����ڴ�

	if(file && fatbuf)
	{
		return 0;  								//������һ��ʧ��,��ʧ��.
	}
	else
	{
		return 1;
	}
}


//�õ�����ʣ������
//drv:���̱��("0:"/"1:")
//total:������	 ����λKB��
//free:ʣ������	 ����λKB��
//����ֵ:0,����.����,�������
u8 exf_getfree(u8 *drv,u32 *total,u32 *free)
{
	FATFS *fs1;
	u8 res;
    u32 fre_clust = 0, fre_sect = 0, tot_sect = 0;
    //�õ�������Ϣ�����д�����
    res = (u32)f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);

    if(res == 0)
	{
	    tot_sect = (fs1->n_fatent-2) * fs1->csize;	//�õ���������
	    fre_sect = fre_clust * fs1->csize;			//�õ�����������

#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
		tot_sect *= fs1->ssize/512;
		fre_sect *= fs1->ssize/512;
#endif
		*total = tot_sect >> 1;	//��λΪKB
		*free = fre_sect >> 1;	//��λΪKB
 	}

	return res;
}











