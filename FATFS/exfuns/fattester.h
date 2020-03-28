#ifndef __FATTESTER_H
#define __FATTESTER_H 			   
#include <stm32f4xx.h>	   
#include "ff.h"

 
FRESULT mf_mount(u8* path,u8 mt);
FRESULT mf_open(u8*path,u8 mode);
FRESULT mf_close(void);
FRESULT mf_read(u32 len);
FRESULT mf_write(u8*dat,u16 len);
FRESULT mf_opendir(u8* path);
FRESULT mf_closedir(void);
FRESULT mf_readdir(void);
FRESULT mf_scan_files(u8 * path);
u32 mf_showfree(u8 *drv);
FRESULT mf_lseek(u32 offset);
u32 mf_tell(void);
u32 mf_size(void);
FRESULT mf_mkdir(u8*pname);
FRESULT mf_fmkfs(u8* path,u8 mode,u16 au);
FRESULT mf_unlink(u8 *pname);
FRESULT mf_rename(u8 *oldname,u8* newname);
void mf_getlabel(u8 *path);
void mf_setlabel(u8 *path); 
void mf_gets(u16 size);
int mf_putc(u8 c);
int mf_puts(u8*c);
 
#endif





























