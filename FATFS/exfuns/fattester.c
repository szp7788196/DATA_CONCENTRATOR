#include "fattester.h"	 
#include "usart.h"
#include "exfuns.h"	  
#include "ff.h"
#include "string.h"
#include "common.h"

    
//Ϊ����ע�Ṥ����	 
//path:����·��������"0:"��"1:"
//mt:0��������ע�ᣨ�Ժ�ע�ᣩ��1������ע��
//����ֵ:ִ�н��
FRESULT mf_mount(u8* path,u8 mt)
{		   
	return f_mount(fs,(const TCHAR*)path,mt); 
}

//��·���µ��ļ�
//path:·��+�ļ���
//mode:��ģʽ
//����ֵ:ִ�н��
FRESULT mf_open(u8*path,u8 mode)
{
	FRESULT res = FR_OK;

	res = f_open(file,(const TCHAR*)path,mode);//���ļ���

	return res;
}

//�ر��ļ�
//����ֵ:ִ�н��
FRESULT mf_close(void)
{
	FRESULT res = FR_OK;
	
	res = f_close(file);

	return res;
}

//��������
//len:�����ĳ���
//����ֵ:ִ�н��
u32 mf_read(u32 len)
{
	u32 i,t;
	FRESULT res = FR_OK;
	u32 tlen = 0;

#ifdef DEBUG_LOG
	printf("\r\nRead file data is:\r\n");
#endif
	
	for(i = 0; i < len / 512; i ++)
	{
		res = f_read(file,fatbuf,512,&br);

		if(res)
		{
#ifdef DEBUG_LOG
			printf("Read Error:%d\r\n",res);
#endif
			break;
		}
		else
		{
			tlen += br;

			for(t = 0; t < br; t ++)
			{
#ifdef DEBUG_LOG
				printf("%c",fatbuf[t]);
#endif
			}
		}
	}

	if(len % 512)
	{
		res = f_read(file,fatbuf,len % 512,&br);

		if(res)	//�����ݳ�����
		{
#ifdef DEBUG_LOG
			printf("\r\nRead Error:%d\r\n",res); 
#endif			
		}
		else
		{
			tlen += br;
			
			for(t = 0; t < br; t ++)
			{
#ifdef DEBUG_LOG
				printf("%c",fatbuf[t]); 
#endif
			}
		}	 
	}
	
	if(tlen)
	{
#ifdef DEBUG_LOG
		printf("\r\nReaded data len:%d\r\n",tlen);//���������ݳ���
#endif
	}
	
#ifdef DEBUG_LOG
	printf("Read data over\r\n");
#endif
	
	return tlen;
}
//д������
//dat:���ݻ�����
//len:д�볤��
//����ֵ:ִ�н��
FRESULT mf_write(u8*dat,u16 len)
{			    
	FRESULT res = FR_OK;	   					   

#ifdef DEBUG_LOG
	printf("\r\nBegin Write file...\r\n");
	printf("Write data len:%d\r\n",len);
#endif
	
	res = f_write(file,dat,len,&bw);

#ifdef DEBUG_LOG
	if(res)
	{
		printf("Write Error:%d\r\n",res);   
	}
	else
	{
		printf("Writed data len:%d\r\n",bw);
	}		

	printf("Write data over.\r\n");
#endif	

	return res;
}

//��Ŀ¼
//path:·��
//����ֵ:ִ�н��
FRESULT mf_opendir(u8* path)
{
	return f_opendir(&dir,(const TCHAR*)path);	
}

//�ر�Ŀ¼ 
//����ֵ:ִ�н��
FRESULT mf_closedir(void)
{
	return f_closedir(&dir);	
}

//���ȡ�ļ���
//����ֵ:ִ�н��
FRESULT mf_readdir(void)
{
	FRESULT res = FR_OK;
	char *fn;			 
		  
	res = f_readdir(&dir,&fileinfo);//��ȡһ���ļ�����Ϣ
	
	if(res != FR_OK || fileinfo.fname[0] == 0)
	{
		return res;//������.
	}

	fn = fileinfo.fname;;

#ifdef DEBUG_LOG
	printf("\r\n DIR info:\r\n");
	printf("dir.id:%d\r\n",dir.id);
	printf("dir.index:%d\r\n",dir.index);
	printf("dir.sclust:%d\r\n",dir.sclust);
	printf("dir.clust:%d\r\n",dir.clust);
	printf("dir.sect:%d\r\n",dir.sect);	  

	printf("\r\n");
	printf("File Name is:%s\r\n",fn);
	printf("File Size is:%d\r\n",fileinfo.fsize);
	printf("File data is:%d\r\n",fileinfo.fdate);
	printf("File time is:%d\r\n",fileinfo.ftime);
	printf("File Attr is:%d\r\n",fileinfo.fattrib);
	printf("\r\n");
#endif

	return res;
}			 

//�����ļ�
//path:·��
//����ֵ:ִ�н��
FRESULT mf_scan_files(u8 * path)
{
	FRESULT res = FR_OK;	  
    char *fn;   /* This function is assuming non-Unicode cfg. */
	  
    res = f_opendir(&dir,(const TCHAR*)path); //��һ��Ŀ¼
	
    if (res == FR_OK) 
	{	
#ifdef DEBUG_LOG
		printf("\r\n"); 
#endif
		
		while(1)
		{
	        res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
			
	        if (res != FR_OK || fileinfo.fname[0] == 0) 
			{
				break;  //������/��ĩβ��,�˳�
			}
			   
        	fn = fileinfo.fname;
			
#ifdef DEBUG_LOG
			printf("%s/", path);//��ӡ·��	
			printf("%s\r\n", fn);//��ӡ�ļ���	
#endif			
		} 
    }	  

    return res;	  
}

//��ʾʣ������
//drv:�̷�
//����ֵ:ʣ������(�ֽ�)
u32 mf_showfree(u8 *drv)
{
	FATFS *fs1;
	FRESULT res = FR_OK;
    u32 fre_clust = 0, fre_sect = 0, tot_sect = 0;
	
    //�õ�������Ϣ�����д�����
    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
    if(res==0)
	{											   
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//�õ���������
	    fre_sect = fre_clust * fs1->csize;			//�õ�����������
		
#if _MAX_SS!=512
		tot_sect *= fs1->ssize / 512;
		fre_sect *= fs1->ssize / 512;
#endif
		
#ifdef DEBUG_LOG
		if(tot_sect < 20480)//������С��10M
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n����������:%d KB\r\n"
		           "���ÿռ�:%d KB\r\n",
		           tot_sect>>1,fre_sect>>1);
		}
		else
		{
		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
		    printf("\r\n����������:%d MB\r\n"
		           "���ÿռ�:%d MB\r\n",
		           tot_sect>>11,fre_sect>>11);
		}
#endif
	}

	return fre_sect;
}	

//�ļ���дָ��ƫ��
//offset:����׵�ַ��ƫ����
//����ֵ:ִ�н��.
FRESULT mf_lseek(u32 offset)
{
	return f_lseek(file,offset);
}

//��ȡ�ļ���ǰ��дָ���λ��.
//����ֵ:λ��
u32 mf_tell(void)
{
	return f_tell(file);
}

//��ȡ�ļ���С
//����ֵ:�ļ���С
u32 mf_size(void)
{
	return f_size(file);
} 

//����Ŀ¼
//pname:Ŀ¼·��+����
//����ֵ:ִ�н��
FRESULT mf_mkdir(u8*pname)
{
	return f_mkdir((const TCHAR *)pname);
}

//��ʽ��
//path:����·��������"0:"��"1:"
//mode:ģʽ
//au:�ش�С
//����ֵ:ִ�н��
FRESULT mf_fmkfs(u8* path,u8 mode,u16 au)
{
	return f_mkfs((const TCHAR*)path,mode,au);//��ʽ��,drv:�̷�;mode:ģʽ;au:�ش�С
} 

//ɾ���ļ�/Ŀ¼
//pname:�ļ�/Ŀ¼·��+����
//����ֵ:ִ�н��
FRESULT mf_unlink(u8 *pname)
{
	return  f_unlink((const TCHAR *)pname);
}

//�޸��ļ�/Ŀ¼����(���Ŀ¼��ͬ,�������ƶ��ļ�Ŷ!)
//oldname:֮ǰ������
//newname:������
//����ֵ:ִ�н��
FRESULT mf_rename(u8 *oldname,u8* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}

//��ȡ�̷����������֣�
//path:����·��������"0:"��"1:"  
void mf_getlabel(u8 *path)
{
	u8 buf[20];
	u32 sn = 0;
	u8 res;

	res = f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);

#ifdef DEBUG_LOG
	if(res == FR_OK)
	{
		printf("\r\n����%s ���̷�Ϊ:%s\r\n",path,buf);
		printf("����%s �����к�:%X\r\n\r\n",path,sn); 
	}
	else 
	{
		printf("\r\n��ȡʧ�ܣ�������:%X\r\n",res);
	}
#endif
}
//�����̷����������֣����11���ַ�������֧�����ֺʹ�д��ĸ����Լ����ֵ�
//path:���̺�+���֣�����"0:ALIENTEK"��"1:OPENEDV"  
void mf_setlabel(u8 *path)
{
	u8 res;
	
	res = f_setlabel ((const TCHAR *)path);
	
#ifdef DEBUG_LOG
	if(res == FR_OK)
	{
		printf("\r\n�����̷����óɹ�:%s\r\n",path);
	}
	else 
	{
		printf("\r\n�����̷�����ʧ�ܣ�������:%X\r\n",res);
	}
#endif
} 

//���ļ������ȡһ���ַ���
//size:Ҫ��ȡ�ĳ���
void mf_gets(u16 size)
{
 	TCHAR* rbuf;
	
	rbuf = f_gets((TCHAR*)fatbuf,size,file);
	
	if(*rbuf == 0)
	{
		return;//û�����ݶ���
	}
	else
	{
#ifdef DEBUG_LOG
		printf("\r\nThe String Readed Is:%s\r\n",rbuf);  	
#endif		
	}			    	
}

//��Ҫ_USE_STRFUNC>=1
//дһ���ַ����ļ�
//c:Ҫд����ַ�
//����ֵ:ִ�н��
int mf_putc(u8 c)
{
	return f_putc((TCHAR)c,file);
}

//д�ַ������ļ�
//c:Ҫд����ַ���
//����ֵ:д����ַ�������
int mf_puts(u8*c)
{
	return f_puts((TCHAR*)c,file);
}













