/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include "diskio.h"			/* FatFs lower layer API */
#include "sd_card.h"
#include "FreeRTOS.h"


DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{
	int Status;
	switch (drv)
	{
		case 0 :
//			if(xSchedulerRunning == 1)
//				xSemaphoreTake(xMutex_SPI2, portMAX_DELAY);
		
			Status = MSD0_Init();
			
//			if(xSchedulerRunning == 1)
//				xSemaphoreGive(xMutex_SPI2);
			
			if(Status==0){
				return RES_OK;
			}else{
				return STA_NOINIT;
			}
		case 1 :
			return RES_OK;
		case 2 :
			return RES_OK;
		case 3 :
			return RES_OK;
		default:
			return STA_NOINIT;
	}
}




DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	switch (drv)
	{
		case 0 :
			return RES_OK;
		case 1 :
			return RES_OK;
		case 2 :
			return RES_OK;
		default:
			return STA_NOINIT;
	}
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..255) */
)
{
	int Status;
	if( !count )
	{
		return RES_PARERR;  /* count2??üμèóú0￡?·??ò·μ??2?êy′í?ó */
	}
	switch (drv)
	{
		case 0:
		    if(count==1)            /* 1??sectorμ??á2ù×÷ */
		    {
				if(xSchedulerRunning == 1)
					xSemaphoreTake(xMutex_SPI2, portMAX_DELAY);
				
				Status =  MSD0_ReadSingleBlock( sector ,buff );
				
				if(xSchedulerRunning == 1)
					xSemaphoreGive(xMutex_SPI2);
				
				if(Status == 0){
					return RES_OK;
				}else{
					return RES_ERROR;
				}
		    }
		    else                    /* ?à??sectorμ??á2ù×÷ */
		    {
				if(xSchedulerRunning == 1)
					xSemaphoreTake(xMutex_SPI2, portMAX_DELAY);
				
				Status = MSD0_ReadMultiBlock( sector , buff ,count);
				
				if(xSchedulerRunning == 1)
					xSemaphoreGive(xMutex_SPI2);
				
				if(Status == 0){
					return RES_OK;
				}else{
					return RES_ERROR;
				}
		    }
		case 1:
		    if(count==1)            /* 1??sectorμ??á2ù×÷ */
		    {
				return RES_OK;
		    }
		    else                    /* ?à??sectorμ??á2ù×÷ */
		    {
				return RES_OK;
		    }

		default:
			return RES_ERROR;
	}
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */

#if _USE_WRITE
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	        /* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..255) */
)
{
	int Status;
	if( !count )
	{
		return RES_PARERR;  /* count2??üμèóú0￡?·??ò·μ??2?êy′í?ó */
	}
	switch (drv)
	{
		case 0:
		    if(count==1)            /* 1??sectorμ?D′2ù×÷ */
		    {
				if(xSchedulerRunning == 1)
					xSemaphoreTake(xMutex_SPI2, portMAX_DELAY);
				
				Status = MSD0_WriteSingleBlock( sector , (uint8_t *)(&buff[0]) );
				
				if(xSchedulerRunning == 1)
					xSemaphoreGive(xMutex_SPI2);
				
				if(Status == 0){
					return RES_OK;
				}else{
					return RES_ERROR;
				}
		    }
		    else                    /* ?à??sectorμ?D′2ù×÷ */
		    {
				if(xSchedulerRunning == 1)
					xSemaphoreTake(xMutex_SPI2, portMAX_DELAY);
				
				Status = MSD0_WriteMultiBlock( sector , (uint8_t *)(&buff[0]) , count );
				
				if(xSchedulerRunning == 1)
					xSemaphoreGive(xMutex_SPI2);
				
				if(Status == 0){
					return RES_OK;
				}else{
					return RES_ERROR;
				}
		    }
		case 1:
		    if(count==1)            /* 1??sectorμ?D′2ù×÷ */
		    {
				return RES_OK;
		    }
		    else                    /* ?à??sectorμ?D′2ù×÷ */
		    {
				return RES_OK;
		    }

		default:return RES_ERROR;
	}
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	if (drv==0)
	{
		MSD0_GetCardInfo(&SD0_CardInfo);
		switch (ctrl)
		{
			case CTRL_SYNC :
				return RES_OK;
		  	case GET_SECTOR_COUNT :
				*(DWORD*)buff = SD0_CardInfo.Capacity/SD0_CardInfo.BlockSize;
		    	return RES_OK;
			case GET_SECTOR_SIZE:
				*(DWORD*)buff = SD0_CardInfo.BlockSize; 
		        return RES_OK;
		  	case GET_BLOCK_SIZE :
				*(WORD*)buff = SD0_CardInfo.BlockSize;
		    	return RES_OK;
		  	case CTRL_POWER :
				break;
		  	case CTRL_LOCK :
				break;
		  	case CTRL_EJECT :
				break;
	      	/* MMC/SDC command */
		  	case MMC_GET_TYPE :
				break;
		  	case MMC_GET_CSD :
				break;
		  	case MMC_GET_CID :
				break;
			case MMC_GET_OCR :
				break;
			case MMC_GET_SDSTAT :
				break;
		}
    }else if(drv==1){
		switch (ctrl)
		{
			case CTRL_SYNC :
				return RES_OK;
		  	case GET_SECTOR_COUNT :
		    	return RES_OK;
		  	case GET_SECTOR_SIZE :
				return RES_OK;
		  	case GET_BLOCK_SIZE :
		    	return RES_OK;
		  	case CTRL_POWER :
				break;
		  	case CTRL_LOCK :
				break;
		  	case CTRL_EJECT :
				break;
	      	/* MMC/SDC command */
		  	case MMC_GET_TYPE :
				break;
		  	case MMC_GET_CSD :
				break;
		  	case MMC_GET_CID :
				break;
		  	case MMC_GET_OCR :
				break;
		  	case MMC_GET_SDSTAT :
				break;
		}
	}
	else{
		return RES_PARERR;
	}
	return RES_PARERR;
}


//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */
DWORD get_fattime (void)
{
	return 0;
}
//动态分配内存
void *ff_memalloc (UINT size)
{
	return (void*)pvPortMalloc(size);
}
//释放内存
void ff_memfree (void* mf)
{
	vPortFree(mf);
}

















