#include "sd_card.h"
#include "spi.h"


MSD_CARDINFO SD0_CardInfo;


int MSD0_spi_read_write(uint8_t data)
{
	return (int)SPI2_ReadWriteByte(data);
}

void MSD0_HARD_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOF, ENABLE );//PORTB ±÷” πƒ‹

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOF, &GPIO_InitStructure);

    MSD0_CS = 1;

	if(xSchedulerRunning == 1)
		xSemaphoreTake(xMutex_SPI2, portMAX_DELAY);
	
	MSD0_SPIHighSpeed(0);
	
	if(xSchedulerRunning == 1)
		xSemaphoreGive(xMutex_SPI2);
}

void MSD0_SPIHighSpeed(uint8_t b_high)
{
	if(b_high == 0)
	{
		SPI2_SetSpeed(SPI_BaudRatePrescaler_256);
	}
	else
	{
		SPI2_SetSpeed(SPI_BaudRatePrescaler_8);
	}
}

int MSD0_Init(void)
{
	uint8_t r1;
	uint8_t buff[6] = {0};
	uint16_t retry;

	/* Check , if no card insert */
	
	RE_DETECT:
	if(MSD0_DETECT)
	{
#ifdef DEBUG_LOG
		printf("There is no card detected! \r\n");
#endif
		/* FATFS error flag */
		delay_ms(1000);
		
		goto RE_DETECT;
	}

	/* Power on and delay some times */
	for(retry = 0; retry < 0x100; retry ++)
	{
		MSD0_POWER_ON;
	}
	
	if(xSchedulerRunning == 1)
		xSemaphoreTake(xMutex_SPI2, portMAX_DELAY);

	/* Satrt send 74 clocks at least */
	for(retry = 0; retry < 10; retry ++)
	{
		MSD0_spi_read_write(DUMMY_BYTE);
	}

	/* Start send CMD0 till return 0x01 means in IDLE state */
	for(retry = 0; retry < 0xFFF; retry ++)
	{
		r1 = MSD0_send_command(CMD0, 0, 0x95);
		
		if(r1 == 0x01)
		{
			retry = 0;
			
			break;
		}
	}
	/* Timeout return */
	if(retry == 0xFFF)
	{
#ifdef DEBUG_LOG
		printf("Reset card into IDLE state failed!\r\n");
#endif
		
		if(xSchedulerRunning == 1)
			xSemaphoreGive(xMutex_SPI2);
		
		return 1;
	}

	/* Get the card type, version */
	r1 = MSD0_send_command_hold(CMD8, 0x1AA, 0x87);
	/* r1=0x05 -> V1.0 */
	if(r1 == 0x05)
	{
		SD0_CardInfo.CardType = CARDTYPE_SDV1;

		/* End of CMD8, chip disable and dummy byte */
		MSD0_CS = 1;
		MSD0_spi_read_write(DUMMY_BYTE);

		/* SD1.0/MMC start initialize */
		/* Send CMD55+ACMD41, No-response is a MMC card, otherwise is a SD1.0 card */
		for(retry = 0; retry < 0xFFF; retry ++)
		{
			r1 = MSD0_send_command(CMD55, 0, 0);			/* should be return 0x01 */
			
			if(r1 != 0x01)
			{
#ifdef DEBUG_LOG
				printf("Send CMD55 should return 0x01, response=0x%02x\r\n", r1);
#endif
				if(xSchedulerRunning == 1)
					xSemaphoreGive(xMutex_SPI2);
				
				return r1;
			}

			r1 = MSD0_send_command(ACMD41, 0, 0);			/* should be return 0x00 */
			
			if(r1 == 0x00)
			{
				retry = 0;
				
				break;
			}
		}

		/* MMC card initialize start */
		if(retry == 0xFFF)
		{
			for(retry = 0; retry < 0xFFF; retry ++)
			{
				r1 = MSD0_send_command(CMD1, 0, 0);		/* should be return 0x00 */
				
				if(r1 == 0x00)
				{
					retry = 0;
					
					break;
				}
			}

			/* Timeout return */
			if(retry == 0xFFF)
			{
#ifdef DEBUG_LOG
				printf("Send CMD1 should return 0x00, response=0x%02x\r\n", r1);
#endif
				if(xSchedulerRunning == 1)
					xSemaphoreGive(xMutex_SPI2);
				
				return 2;
			}

			SD0_CardInfo.CardType = CARDTYPE_MMC;
#ifdef DEBUG_LOG
			printf("Card Type                     : MMC\r\n");
#endif
		}
		/* SD1.0 card detected, print information */
#ifdef DEBUG_LOG
		else
		{
			printf("Card Type                     : SD V1\r\n");
		}
#endif

		/* Set spi speed high */
		MSD0_SPIHighSpeed(1);

		/* CRC disable */
		r1 = MSD0_send_command(CMD59, 0, 0x01);
		
		if(r1 != 0x00)
		{
#ifdef DEBUG_LOG
			printf("Send CMD59 should return 0x00, response=0x%02x\r\n", r1);
#endif
			if(xSchedulerRunning == 1)
				xSemaphoreGive(xMutex_SPI2);
			
			return r1;		/* response error, return r1 */
		}

		/* Set the block size */
		r1 = MSD0_send_command(CMD16, MSD_BLOCKSIZE, 0xFF);
		
		if(r1 != 0x00)
		{
#ifdef DEBUG_LOG
			printf("Send CMD16 should return 0x00, response=0x%02x\r\n", r1);
#endif
			if(xSchedulerRunning == 1)
				xSemaphoreGive(xMutex_SPI2);
			
			return r1;		/* response error, return r1 */
		}
	}
	/* r1=0x01 -> V2.x, read OCR register, check version */
	else if(r1 == 0x01)
	{
		/* 4Bytes returned after CMD8 sent	*/
		buff[0] = MSD0_spi_read_write(DUMMY_BYTE);				/* should be 0x00 */
		buff[1] = MSD0_spi_read_write(DUMMY_BYTE);				/* should be 0x00 */
		buff[2] = MSD0_spi_read_write(DUMMY_BYTE);				/* should be 0x01 */
		buff[3] = MSD0_spi_read_write(DUMMY_BYTE);				/* should be 0xAA */

		/* End of CMD8, chip disable and dummy byte */
		MSD0_CS = 1;
		MSD0_spi_read_write(DUMMY_BYTE);

		/* Check voltage range be 2.7-3.6V	*/
		if(buff[2] == 0x01 && buff[3] == 0xAA)
		{
			for(retry = 0; retry < 0xFFF; retry ++)
			{
				r1 = MSD0_send_command(CMD55, 0, 0);			/* should be return 0x01 */
				
				if(r1 != 0x01)
				{
#ifdef DEBUG_LOG
					printf("Send CMD55 should return 0x01, response=0x%02x\r\n", r1);
#endif
					if(xSchedulerRunning == 1)
						xSemaphoreGive(xMutex_SPI2);
					
					return r1;
				}

				r1 = MSD0_send_command(ACMD41, 0x40000000, 0);	/* should be return 0x00 */
				
				if(r1 == 0x00)
				{
					retry = 0;
					
					break;
				}
			}

			/* Timeout return */
			if(retry == 0xFFF)
			{
#ifdef DEBUG_LOG
				printf("Send ACMD41 should return 0x00, response=0x%02x\r\n", r1);
#endif
				if(xSchedulerRunning == 1)
					xSemaphoreGive(xMutex_SPI2);
				
				return 3;
			}

			/* Read OCR by CMD58 */
			r1 = MSD0_send_command_hold(CMD58, 0, 0);
			
			if(r1 != 0x00)
			{
#ifdef DEBUG_LOG
				printf("Send CMD58 should return 0x00, response=0x%02x\r\n", r1);
#endif
				if(xSchedulerRunning == 1)
					xSemaphoreGive(xMutex_SPI2);
				
				return r1;		/* response error, return r1 */
			}

			buff[0] = MSD0_spi_read_write(DUMMY_BYTE);
			buff[1] = MSD0_spi_read_write(DUMMY_BYTE);
			buff[2] = MSD0_spi_read_write(DUMMY_BYTE);
			buff[3] = MSD0_spi_read_write(DUMMY_BYTE);

			/* End of CMD58, chip disable and dummy byte */
			MSD0_CS = 1;
			MSD0_spi_read_write(DUMMY_BYTE);

			/* OCR -> CCS(bit30)  1: SDV2HC	 0: SDV2 */
			if(buff[0] & 0x40)
			{
				SD0_CardInfo.CardType = CARDTYPE_SDV2HC;
#ifdef DEBUG_LOG
				printf("Card Type                     : SD V2HC\r\n");
#endif
			}
			else
			{
				SD0_CardInfo.CardType = CARDTYPE_SDV2;
#ifdef DEBUG_LOG
				printf("Card Type                     : SD V2\r\n");
#endif
			}

			/* Set spi speed high */
			MSD0_SPIHighSpeed(1);
		}
	}
	
	if(xSchedulerRunning == 1)
		xSemaphoreGive(xMutex_SPI2);
	
	return 0;
}

int MSD0_GetCardInfo(PMSD_CARDINFO SD0_CardInfo)
{
	uint8_t r1;
	uint8_t CSD_Tab[16];
	uint8_t CID_Tab[16];

	/* Send CMD9, Read CSD */
	r1 = MSD0_send_command(CMD9, 0, 0xFF);
	
	if(r1 != 0x00)
	{
		return r1;
	}

	if(MSD0_read_buffer(CSD_Tab, 16, RELEASE))
	{
		return 1;
	}

	/* Send CMD10, Read CID */
	r1 = MSD0_send_command(CMD10, 0, 0xFF);
	
	if(r1 != 0x00)
	{
		return r1;
	}

	if(MSD0_read_buffer(CID_Tab, 16, RELEASE))
	{
		return 2;
	}

	/* Byte 0 */
	SD0_CardInfo->CSD.CSDStruct = (CSD_Tab[0] & 0xC0) >> 6;
	SD0_CardInfo->CSD.SysSpecVersion = (CSD_Tab[0] & 0x3C) >> 2;
	SD0_CardInfo->CSD.Reserved1 = CSD_Tab[0] & 0x03;
	/* Byte 1 */
	SD0_CardInfo->CSD.TAAC = CSD_Tab[1] ;
	/* Byte 2 */
	SD0_CardInfo->CSD.NSAC = CSD_Tab[2];
	/* Byte 3 */
	SD0_CardInfo->CSD.MaxBusClkFrec = CSD_Tab[3];
	/* Byte 4 */
	SD0_CardInfo->CSD.CardComdClasses = CSD_Tab[4] << 4;
	/* Byte 5 */
	SD0_CardInfo->CSD.CardComdClasses |= (CSD_Tab[5] & 0xF0) >> 4;
	SD0_CardInfo->CSD.RdBlockLen = CSD_Tab[5] & 0x0F;
	/* Byte 6 */
	SD0_CardInfo->CSD.PartBlockRead = (CSD_Tab[6] & 0x80) >> 7;
	SD0_CardInfo->CSD.WrBlockMisalign = (CSD_Tab[6] & 0x40) >> 6;
	SD0_CardInfo->CSD.RdBlockMisalign = (CSD_Tab[6] & 0x20) >> 5;
	SD0_CardInfo->CSD.DSRImpl = (CSD_Tab[6] & 0x10) >> 4;
	SD0_CardInfo->CSD.Reserved2 = 0; /* Reserved */
	SD0_CardInfo->CSD.DeviceSize = (CSD_Tab[6] & 0x03) << 10;
	/* Byte 7 */
	SD0_CardInfo->CSD.DeviceSize |= (CSD_Tab[7]) << 2;
	/* Byte 8 */
	SD0_CardInfo->CSD.DeviceSize |= (CSD_Tab[8] & 0xC0) >> 6;
	SD0_CardInfo->CSD.MaxRdCurrentVDDMin = (CSD_Tab[8] & 0x38) >> 3;
	SD0_CardInfo->CSD.MaxRdCurrentVDDMax = (CSD_Tab[8] & 0x07);
	/* Byte 9 */
	SD0_CardInfo->CSD.MaxWrCurrentVDDMin = (CSD_Tab[9] & 0xE0) >> 5;
	SD0_CardInfo->CSD.MaxWrCurrentVDDMax = (CSD_Tab[9] & 0x1C) >> 2;
	SD0_CardInfo->CSD.DeviceSizeMul = (CSD_Tab[9] & 0x03) << 1;
	/* Byte 10 */
	SD0_CardInfo->CSD.DeviceSizeMul |= (CSD_Tab[10] & 0x80) >> 7;
	SD0_CardInfo->CSD.EraseGrSize = (CSD_Tab[10] & 0x7C) >> 2;
	SD0_CardInfo->CSD.EraseGrMul = (CSD_Tab[10] & 0x03) << 3;
	/* Byte 11 */
	SD0_CardInfo->CSD.EraseGrMul |= (CSD_Tab[11] & 0xE0) >> 5;
	SD0_CardInfo->CSD.WrProtectGrSize = (CSD_Tab[11] & 0x1F);
	/* Byte 12 */
	SD0_CardInfo->CSD.WrProtectGrEnable = (CSD_Tab[12] & 0x80) >> 7;
	SD0_CardInfo->CSD.ManDeflECC = (CSD_Tab[12] & 0x60) >> 5;
	SD0_CardInfo->CSD.WrSpeedFact = (CSD_Tab[12] & 0x1C) >> 2;
	SD0_CardInfo->CSD.MaxWrBlockLen = (CSD_Tab[12] & 0x03) << 2;
	/* Byte 13 */
	SD0_CardInfo->CSD.MaxWrBlockLen |= (CSD_Tab[13] & 0xc0) >> 6;
	SD0_CardInfo->CSD.WriteBlockPaPartial = (CSD_Tab[13] & 0x20) >> 5;
	SD0_CardInfo->CSD.Reserved3 = 0;
	SD0_CardInfo->CSD.ContentProtectAppli = (CSD_Tab[13] & 0x01);
	/* Byte 14 */
	SD0_CardInfo->CSD.FileFormatGrouop = (CSD_Tab[14] & 0x80) >> 7;
	SD0_CardInfo->CSD.CopyFlag = (CSD_Tab[14] & 0x40) >> 6;
	SD0_CardInfo->CSD.PermWrProtect = (CSD_Tab[14] & 0x20) >> 5;
	SD0_CardInfo->CSD.TempWrProtect = (CSD_Tab[14] & 0x10) >> 4;
	SD0_CardInfo->CSD.FileFormat = (CSD_Tab[14] & 0x0C) >> 2;
	SD0_CardInfo->CSD.ECC = (CSD_Tab[14] & 0x03);
	/* Byte 15 */
	SD0_CardInfo->CSD.CSD_CRC = (CSD_Tab[15] & 0xFE) >> 1;
	SD0_CardInfo->CSD.Reserved4 = 1;

	if(SD0_CardInfo->CardType == CARDTYPE_SDV2HC)
	{
		/* Byte 7 */
		SD0_CardInfo->CSD.DeviceSize = (u16)(CSD_Tab[8]) *256;
		/* Byte 8 */
		SD0_CardInfo->CSD.DeviceSize += CSD_Tab[9] ;
	}

	SD0_CardInfo->Capacity = SD0_CardInfo->CSD.DeviceSize * MSD_BLOCKSIZE * 1024;
	SD0_CardInfo->BlockSize = MSD_BLOCKSIZE;

	/* Byte 0 */
	SD0_CardInfo->CID.ManufacturerID = CID_Tab[0];
	/* Byte 1 */
	SD0_CardInfo->CID.OEM_AppliID = CID_Tab[1] << 8;
	/* Byte 2 */
	SD0_CardInfo->CID.OEM_AppliID |= CID_Tab[2];
	/* Byte 3 */
	SD0_CardInfo->CID.ProdName1 = CID_Tab[3] << 24;
	/* Byte 4 */
	SD0_CardInfo->CID.ProdName1 |= CID_Tab[4] << 16;
	/* Byte 5 */
	SD0_CardInfo->CID.ProdName1 |= CID_Tab[5] << 8;
	/* Byte 6 */
	SD0_CardInfo->CID.ProdName1 |= CID_Tab[6];
	/* Byte 7 */
	SD0_CardInfo->CID.ProdName2 = CID_Tab[7];
	/* Byte 8 */
	SD0_CardInfo->CID.ProdRev = CID_Tab[8];
	/* Byte 9 */
	SD0_CardInfo->CID.ProdSN = CID_Tab[9] << 24;
	/* Byte 10 */
	SD0_CardInfo->CID.ProdSN |= CID_Tab[10] << 16;
	/* Byte 11 */
	SD0_CardInfo->CID.ProdSN |= CID_Tab[11] << 8;
	/* Byte 12 */
	SD0_CardInfo->CID.ProdSN |= CID_Tab[12];
	/* Byte 13 */
	SD0_CardInfo->CID.Reserved1 |= (CID_Tab[13] & 0xF0) >> 4;
	/* Byte 14 */
	SD0_CardInfo->CID.ManufactDate = (CID_Tab[13] & 0x0F) << 8;
	/* Byte 15 */
	SD0_CardInfo->CID.ManufactDate |= CID_Tab[14];
	/* Byte 16 */
	SD0_CardInfo->CID.CID_CRC = (CID_Tab[15] & 0xFE) >> 1;
	SD0_CardInfo->CID.Reserved2 = 1;

	return 0;
}

int MSD0_read_buffer(uint8_t *buff, uint16_t len, uint8_t release)
{
	uint8_t r1;
	register uint16_t retry;

	/* Card enable, Prepare to read	*/
	MSD0_CS = 0;

	/* Wait start-token 0xFE */
	for(retry = 0; retry < 2000; retry ++)
	{
		r1 = MSD0_spi_read_write(DUMMY_BYTE);
		
		if(r1 == 0xFE)
		{
			retry = 0;
			
			break;
		}
	}

	/* Timeout return	*/
	if(retry == 2000)
	{
		MSD0_CS = 1;
		
		return 1;
	}

	/* Start reading */
	for(retry = 0; retry < len; retry ++)
	{
		*(buff + retry) = MSD0_spi_read_write(DUMMY_BYTE);
	}

	/* 2bytes dummy CRC */
	MSD0_spi_read_write(DUMMY_BYTE);
	MSD0_spi_read_write(DUMMY_BYTE);

	/* chip disable and dummy byte */
	if(release)
	{
		MSD0_CS = 1;
		MSD0_spi_read_write(DUMMY_BYTE);
	}

	return 0;
}

int MSD0_ReadSingleBlock(uint32_t sector, uint8_t *buffer)
{
	uint8_t r1;

	/* if ver = SD2.0 HC, sector need <<9 */
	if(SD0_CardInfo.CardType != CARDTYPE_SDV2HC)
	{
		sector = sector << 9;
	}

	/* Send CMD17 : Read single block command */
	r1 = MSD0_send_command(CMD17, sector, 0);

	if(r1 != 0x00)
	{
		return 1;
	}

	/* Start read and return the result */
	r1 = MSD0_read_buffer(buffer, MSD_BLOCKSIZE, RELEASE);

	/* Send stop data transmit command - CMD12 */
	MSD0_send_command(CMD12, 0, 0);

	return r1;
}

int MSD0_ReadMultiBlock(uint32_t sector, uint8_t *buffer, uint32_t NbrOfSector)
{
	uint8_t r1;
	register uint32_t i;

	/* if ver = SD2.0 HC, sector need <<9 */
	if(SD0_CardInfo.CardType != CARDTYPE_SDV2HC)
	{
		sector = sector<<9;
	}

	/* Send CMD18 : Read multi block command */
	r1 = MSD0_send_command(CMD18, sector, 0);
	
	if(r1 != 0x00)
	{
		return 1;
	}

	/* Start read	*/
	for(i = 0; i < NbrOfSector; i ++)
	{
		if(MSD0_read_buffer(buffer + i * MSD_BLOCKSIZE, MSD_BLOCKSIZE, HOLD))
		{
			/* Send stop data transmit command - CMD12	*/
			MSD0_send_command(CMD12, 0, 0);
			/* chip disable and dummy byte */
			MSD0_CS = 1;
			
			return 2;
		}
	}

	/* Send stop data transmit command - CMD12 */
	MSD0_send_command(CMD12, 0, 0);

	/* chip disable and dummy byte */
	MSD0_CS = 1;
	MSD0_spi_read_write(DUMMY_BYTE);

	return 0;
}

int MSD0_WriteSingleBlock(uint32_t sector, uc8 *buffer)
{
	uint8_t r1;
	register uint16_t i;
	uint32_t retry;

	/* if ver = SD2.0 HC, sector need <<9 */
	if(SD0_CardInfo.CardType != CARDTYPE_SDV2HC)
	{
		sector = sector << 9;
	}

	/* Send CMD24 : Write single block command */
	r1 = MSD0_send_command(CMD24, sector, 0);

	if(r1 != 0x00)
	{
		return 1;
	}

	/* Card enable, Prepare to write */
	MSD0_CS = 0;
	MSD0_spi_read_write(DUMMY_BYTE);
	MSD0_spi_read_write(DUMMY_BYTE);
	MSD0_spi_read_write(DUMMY_BYTE);
	/* Start data write token: 0xFE */
	MSD0_spi_read_write(0xFE);

	/* Start single block write the data buffer */
	for(i = 0; i < MSD_BLOCKSIZE; i ++)
	{
		MSD0_spi_read_write(*buffer ++);
	}

	/* 2Bytes dummy CRC */
	MSD0_spi_read_write(DUMMY_BYTE);
	MSD0_spi_read_write(DUMMY_BYTE);

	/* MSD card accept the data */
	r1 = MSD0_spi_read_write(DUMMY_BYTE);
	
	if((r1 & 0x1F) != 0x05)
	{
		MSD0_CS = 1;
		
		return 2;
	}

	/* Wait all the data programm finished */
	retry = 0;
	
	while(MSD0_spi_read_write(DUMMY_BYTE) == 0x00)
	{
		/* Timeout return */
		if(retry ++ == 0x40000)
		{
			MSD0_CS = 1;
			
			return 3;
		}
	}

	/* chip disable and dummy byte */
	MSD0_CS = 1;
	MSD0_spi_read_write(DUMMY_BYTE);

	return 0;
}

int MSD0_WriteMultiBlock(uint32_t sector, uc8 *buffer, uint32_t NbrOfSector)
{
	uint8_t r1;
	register uint16_t i;
	register uint32_t n;
	uint32_t retry;

	/* if ver = SD2.0 HC, sector need <<9 */
	if(SD0_CardInfo.CardType != CARDTYPE_SDV2HC)
	{
		sector = sector << 9;
	}

	/* Send command ACMD23 berfore multi write if is not a MMC card */
	if(SD0_CardInfo.CardType != CARDTYPE_MMC)
	{
		MSD0_send_command(ACMD23, NbrOfSector, 0x00);
	}

	/* Send CMD25 : Write nulti block command	*/
	r1 = MSD0_send_command(CMD25, sector, 0);

	if(r1 != 0x00)
	{
		return 1;
	}

	/* Card enable, Prepare to write */
	MSD0_CS = 0;
	MSD0_spi_read_write(DUMMY_BYTE);
	//MSD0_spi_read_write(DUMMY_BYTE);
	//MSD0_spi_read_write(DUMMY_BYTE);

	for(n = 0; n < NbrOfSector; n ++)
	{
		/* Start multi block write token: 0xFC */
		MSD0_spi_read_write(0xFC);

		for(i = 0; i < MSD_BLOCKSIZE; i ++)
		{
			MSD0_spi_read_write(*buffer ++);
		}

		/* 2Bytes dummy CRC */
		MSD0_spi_read_write(DUMMY_BYTE);
		MSD0_spi_read_write(DUMMY_BYTE);

		/* MSD card accept the data */
		r1 = MSD0_spi_read_write(DUMMY_BYTE);
		
		if((r1 & 0x1F) != 0x05)
		{
			MSD0_CS = 1;
			
			return 2;
		}

		/* Wait all the data programm finished	*/
		retry = 0;
		
		while(MSD0_spi_read_write(DUMMY_BYTE) != 0xFF)
		{
			/* Timeout return */
			if(retry ++ == 0x40000)
			{
				MSD0_CS = 1;
				
				return 3;
			}
		}
	}

	/* Send end of transmit token: 0xFD */
	r1 = MSD0_spi_read_write(0xFD);
	
	if(r1 == 0x00)
	{
		return 4;
	}

	/* Wait all the data programm finished */
	retry = 0;
	
	while(MSD0_spi_read_write(DUMMY_BYTE) != 0xFF)
	{
		/* Timeout return */
		if(retry ++ == 0x40000)
		{
			MSD0_CS = 1;
			
			return 5;
		}
	}

	/* chip disable and dummy byte */
	MSD0_CS = 1;
	MSD0_spi_read_write(DUMMY_BYTE);

	return 0;
}

int MSD0_send_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t r1;
	uint8_t retry;

	/* Dummy byte and chip enable */
	MSD0_spi_read_write(DUMMY_BYTE);
	MSD0_CS = 0;

	/* Command, argument and crc */
	MSD0_spi_read_write(cmd | 0x40);
	MSD0_spi_read_write(arg >> 24);
	MSD0_spi_read_write(arg >> 16);
	MSD0_spi_read_write(arg >> 8);
	MSD0_spi_read_write(arg);
	MSD0_spi_read_write(crc);

	/* Wait response, quit till timeout */
	for(retry = 0; retry < 200; retry ++)
	{
		r1 = MSD0_spi_read_write(DUMMY_BYTE);
		
		if(r1 != 0xFF)
		{
			break;
		}
	}

	/* Chip disable and dummy byte */
	MSD0_CS = 1;
	MSD0_spi_read_write(DUMMY_BYTE);

	return r1;
}

int MSD0_send_command_hold(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	uint8_t r1;
	uint8_t retry;

	/* Dummy byte and chip enable */
	MSD0_spi_read_write(DUMMY_BYTE);
	MSD0_CS = 0;

	/* Command, argument and crc */
	MSD0_spi_read_write(cmd | 0x40);
	MSD0_spi_read_write(arg >> 24);
	MSD0_spi_read_write(arg >> 16);
	MSD0_spi_read_write(arg >> 8);
	MSD0_spi_read_write(arg);
	MSD0_spi_read_write(crc);

	/* Wait response, quit till timeout */
	for(retry = 0; retry < 200; retry ++)
	{
		r1 = MSD0_spi_read_write(DUMMY_BYTE);
		
		if(r1 != 0xFF)
		{
			break;
		}
	}

	return r1;
}














































