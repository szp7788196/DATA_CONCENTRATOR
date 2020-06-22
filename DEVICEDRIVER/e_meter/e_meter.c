#include "e_meter.h"
#include "ht7038.h"
#include "task_rs485.h"
#include "cat25x.h"


BulitInMeterParas_S BulitInMeterParas;
BulitInMeterRatio_S BulitInMeterRatio;

void ReadBulitInMeterRatio(void)
{
	u16 crc16_cal = 0;

	CAT25X_Read((u8 *)&BulitInMeterRatio,
	            ELECTRICITY_IN_METER_RATIO_ADD,
	            sizeof(BulitInMeterRatio_S));

	crc16_cal = CRC16((u8 *)&BulitInMeterRatio,ELECTRICITY_IN_METER_RATIO_LEN - 2);

	if(crc16_cal != BulitInMeterRatio.crc16)
	{
		WriteBulitInMeterRatio(1,0);
	}
}

void WriteBulitInMeterRatio(u8 reset,u8 write_enable)
{
	if(reset == 1)
	{
		BulitInMeterRatio.voltage_ratio = 1.0f;
		BulitInMeterRatio.current_ratio = 1.0f;

		BulitInMeterRatio.crc16 = 0;
	}

	if(write_enable == 1)
	{
		BulitInMeterRatio.crc16 = CRC16((u8 *)&BulitInMeterRatio,ELECTRICITY_IN_METER_RATIO_LEN - 2);

		CAT25X_Write((u8 *)&BulitInMeterRatio,
		             ELECTRICITY_IN_METER_RATIO_ADD,
		             ELECTRICITY_IN_METER_RATIO_LEN);
	}
}

void HT7038GetParameters(void)
{
	u8 i = 0;
	u8 got_ratio = 0;
	float temp = 0.0f;
	s32 value = 0;
	float voltage_ratio = 1.0f;
	float current_ratio = 1.0f;

	double active_energy[4] = {0.0f};
	double reactive_energy[4] = {0.0f};
	double apparent_energy[4] = {0.0f};

	for(i = 0; i < ElectricityMeterConfigNum.number; i ++)
	{
		if(ElectricityMeterConfig[i].address == 0 &&
		   ElectricityMeterConfig[i].channel == 0)
		{
			voltage_ratio = ElectricityMeterConfig[i].voltage_ratio;
			current_ratio = ElectricityMeterConfig[i].current_ratio;

			got_ratio = 1;
			break;
		}
	}
	
	if(got_ratio == 0)
	{
		voltage_ratio = BulitInMeterRatio.voltage_ratio;
		current_ratio = BulitInMeterRatio.current_ratio;
	}

	//读取电压
	value = HT7038GetSingleParameter(HT7038_REG_RMS_VOL_A);
	BulitInMeterParas.voltage[1] = (float)value / 8192.0f * voltage_ratio;

	value = HT7038GetSingleParameter(HT7038_REG_RMS_VOL_B);
	BulitInMeterParas.voltage[2] = (float)value / 8192.0f * voltage_ratio;

	value = HT7038GetSingleParameter(HT7038_REG_RMS_VOL_C);
	BulitInMeterParas.voltage[3] = (float)value / 8192.0f * voltage_ratio;

	BulitInMeterParas.voltage[0] = (BulitInMeterParas.voltage[1] +
								    BulitInMeterParas.voltage[2] +
								    BulitInMeterParas.voltage[3]) / 3.0f;

	//读取电流
	value = HT7038GetSingleParameter(HT7038_REG_RMS_CURRENT_A);
	BulitInMeterParas.current[1] = (float)value / 8192.0f / 10.2f * 40.0f / 16.0f * current_ratio;

	value = HT7038GetSingleParameter(HT7038_REG_RMS_CURRENT_B);
	BulitInMeterParas.current[2] = (float)value / 8192.0f / 10.2f * 40.0f / 16.0f * current_ratio;

	value = HT7038GetSingleParameter(HT7038_REG_RMS_CURRENT_C);
	BulitInMeterParas.current[3]= (float)value / 8192.0f / 10.2f * 40.0f / 16.0f * current_ratio;

	value = HT7038GetSingleParameter(HT7038_REG_RMS_CURRENT_ABC);
	BulitInMeterParas.current[0] = (float)value / 8192.0f / 10.2f * 40.0f / 16.0f * current_ratio;

	//读取有功功率
	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_POWER_A);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.active_power[1] = (float)value * HT7038_K;

	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_POWER_B);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.active_power[2] = (float)value * HT7038_K;

	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_POWER_C);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.active_power[3] = (float)value * HT7038_K;

	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_POWER_ABC);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.active_power[0] = (float)value * HT7038_K;

	//读取无功功率
	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_POWER_A);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.reactive_power[1] = (float)value * HT7038_K;

	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_POWER_B);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.reactive_power[2] = (float)value * HT7038_K;

	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_POWER_B);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.reactive_power[3]= (float)value * HT7038_K;

	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_POWER_A);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.reactive_power[0] = (float)value * HT7038_K;

	//读取视在功率
	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_POWER_A);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.apparent_power[1] = (float)value * HT7038_K;

	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_POWER_B);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.apparent_power[2] = (float)value * HT7038_K;

	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_POWER_C);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.apparent_power[3] = (float)value * HT7038_K;

	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_POWER_ABC);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.apparent_power[0] = (float)value * HT7038_K;

	//读取功率因数
	value = HT7038GetSingleParameter(HT7038_REG_PFACTOR_A);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.power_factor[1] = (float)value / 0x800000;

	value = HT7038GetSingleParameter(HT7038_REG_PFACTOR_B);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.power_factor[2] = (float)value / 0x800000;

	value = HT7038GetSingleParameter(HT7038_REG_PFACTOR_C);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.power_factor[3] = (float)value / 0x800000;

	value = HT7038GetSingleParameter(HT7038_REG_PFACTOR_ABC);
	if(value & (1 << 23))
	{
		value = value - 0x01000000;
	}
	BulitInMeterParas.power_factor[0] = (float)value / 0x800000;

	//读取有功电能
	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_ENERGY_A);
	temp = (float)value / 6400.0f / 12.0f;
	BulitInMeterParas.active_energy[1] = (double)(active_energy[0] + temp);

	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_ENERGY_B);
	temp = (float)value / 6400.0f / 12.0f;
	BulitInMeterParas.active_energy[2] = (double)(active_energy[1] + temp);

	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_ENERGY_C);
	temp = (float)value / 6400.0f / 12.0f;
	BulitInMeterParas.active_energy[3] = (double)(active_energy[2] + temp);

	value = HT7038GetSingleParameter(HT7038_REG_ACTIVE_ENERGY_ABC);
	temp = (float)value / 6400.0f / 12.0f;
	BulitInMeterParas.active_energy[0]= (double)(active_energy[3] + temp);

	//读取无功电能
	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_ENERGY_A);
	temp = (float)value / 6400.0f / 12.0f;
	BulitInMeterParas.reactive_energy[1] = (double)(reactive_energy[0] + temp);

	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_ENERGY_B);
	temp = (float)value / 6400.0f / 12.0f;
	BulitInMeterParas.reactive_energy[2] = (double)(reactive_energy[1] + temp);

	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_ENERGY_C);
	temp = (float)value / 6400.0f / 12.0f;
	BulitInMeterParas.reactive_energy[3] = (double)(reactive_energy[2] + temp);

	value = HT7038GetSingleParameter(HT7038_REG_REACTIVE_ENERGY_ABC);
	temp = (float)value / 6400.0f / 12.0f;
	BulitInMeterParas.reactive_energy[0] = (double)(reactive_energy[3] + temp);

	//读取视在电能
	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_ENERGY_A);
	temp = (float)value / 6400.0f / 12.0f;
	BulitInMeterParas.apparent_energy[1] = (double)(apparent_energy[0] + temp);

	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_ENERGY_B);
	temp = (float)value / 6400.0f / 12.0f;
	BulitInMeterParas.apparent_energy[2] = (double)(apparent_energy[1] + temp);

	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_ENERGY_C);
	temp = (float)value / 6400.0f / 12.0f;
	BulitInMeterParas.apparent_energy[3] = (double)(apparent_energy[2] + temp);

	value = HT7038GetSingleParameter(HT7038_REG_APPARENT_ENERGY_ABC);
	temp = (float)value / 6400.0f / 12.0f;
	BulitInMeterParas.apparent_energy[0] = (double)(apparent_energy[3] + temp);

	//读取频率
	value = HT7038GetSingleParameter(HT7038_REG_FREQUENCY);
	BulitInMeterParas.frequency = (float)value / 0x2000;
}

//获取内置电表参数
void ElectricityMeterGetBuiltInParas(ElectricityMeterState_S *state)
{
	u8 i = 0;
	
	for(i = 0; i < 4; i ++)
	{
		state->current_para[i].voltage = BulitInMeterParas.voltage[i];
		state->current_para[i].current = BulitInMeterParas.current[i];
		state->current_para[i].active_power = BulitInMeterParas.active_power[i];
		state->current_para[i].active_energy = BulitInMeterParas.active_energy[i];
		state->current_para[i].reactive_energy = BulitInMeterParas.reactive_energy[i];
		state->current_para[i].power_factor = BulitInMeterParas.power_factor[i];
		state->current_para[i].frequency = BulitInMeterParas.frequency;
	}	
}

//或外置电表或采集模块的参数
void ElectricityMeterGetBuiltOutParas(ElectricityMeterState_S state)
{
	u8 outbuf[10] = {0};
	Rs485Frame_S *frame1 = NULL;
	Rs485Frame_S *frame2 = NULL;

	frame1 = (Rs485Frame_S *)pvPortMalloc(sizeof(Rs485Frame_S));

	if(frame1 != NULL)
	{
		frame1->device_type = ELECTRICITY_METER;

		frame1->len = PackBuiltOutElectricityMeterFrame(state.address,0,outbuf);

		frame1->buf = (u8 *)pvPortMalloc(frame1->len * sizeof(u8));

		if(frame1->buf != NULL)
		{
			memcpy(frame1->buf,outbuf,frame1->len);

			xSemaphoreTake(xMutex_Rs485Rs485Frame, portMAX_DELAY);

			if(xQueueSend(xQueue_Rs485Rs485Frame,(void *)&frame1,(TickType_t)10) != pdPASS)
			{
#ifdef DEBUG_LOG
				printf("send xQueue_Rs485Rs485Frame fail.\r\n");
#endif
				DeleteRs485Frame(frame1);
			}

			xSemaphoreGive(xMutex_Rs485Rs485Frame);
		}
		else
		{
			DeleteRs485Frame(frame1);
		}
	}
	
	if(state.address >= 32 && state.address <= 47)
	{
		frame2 = (Rs485Frame_S *)pvPortMalloc(sizeof(Rs485Frame_S));
		
		if(frame2 != NULL)
		{
			frame2->device_type = ELECTRICITY_METER;

			frame2->len = PackBuiltOutElectricityMeterFrame(state.address,1,outbuf);

			frame2->buf = (u8 *)pvPortMalloc(frame2->len * sizeof(u8));

			if(frame2->buf != NULL)
			{
				memcpy(frame2->buf,outbuf,frame2->len);

				xSemaphoreTake(xMutex_Rs485Rs485Frame, portMAX_DELAY);

				if(xQueueSend(xQueue_Rs485Rs485Frame,(void *)&frame2,(TickType_t)10) != pdPASS)
				{
#ifdef DEBUG_LOG
					printf("send xQueue_Rs485Rs485Frame fail.\r\n");
#endif
					DeleteRs485Frame(frame2);
				}

				xSemaphoreGive(xMutex_Rs485Rs485Frame);
			}
			else
			{
				DeleteRs485Frame(frame2);
			}
		}
	}
}

//合并外部输入量检测模块报文
u16 PackBuiltOutElectricityMeterFrame(u8 address,u8 mode,u8 *outbuf)
{
	u16 len = 0;
	u16 crc16 = 0;
	
	*(outbuf + 0) = address;
	
	if(address >= 32 && address <= 47)				//电表
	{
		*(outbuf + 1) = 0x03;
		
		if(mode == 0)
		{
			*(outbuf + 2) = 0x9C;
			*(outbuf + 3) = 0x40;
			*(outbuf + 4) = 0x00;
			*(outbuf + 5) = 0x3A;
		}
		else if(mode == 1)
		{
			*(outbuf + 2) = 0x9D;
			*(outbuf + 3) = 0x3A;
			*(outbuf + 4) = 0x00;
			*(outbuf + 5) = 0x08;
		}
	}
	else if(address >= 48 && address <= 53)			//六路模块
	{
		*(outbuf + 1) = 0x03;
		*(outbuf + 2) = 0x00;
		*(outbuf + 3) = 0x40;
		*(outbuf + 4) = 0x00;
		*(outbuf + 5) = 0x2A;
	}

	crc16 = CRC16(outbuf,6);
	
	*(outbuf + 6) = (u8)((crc16 >> 8) & 0x00FF);
	*(outbuf + 7) = (u8)(crc16 & 0x00FF);

	len = 8;
	
	return len;
}

void AnalysisBuiltOutElectricityMeterFrame(u8 *buf,u16 len,ElectricityMeterCollectState_S *meter_state)
{
	u8 i = 0;
	u8 *data = NULL;
	u16 data_len = 0;
	u16 crc16_read = 0;
	u16 crc16_cal = 0;
	u8 address = 0;
	u16 value1 = 0; 
	u32 value2 = 0; 
	u8 tmp[8] = {0};
	float voltage_ratio = 1.0f;
	float current_ratio = 1.0f;

	crc16_read = ((((u16)(*(buf + len - 2))) << 8) & 0xFF00) + (((u16)(*(buf + len - 1))) & 0x00FF);
	
	crc16_cal = CRC16(buf,len - 2);
	
	if(crc16_read == crc16_cal && *(buf + 1) == 0x03)
	{
		address = *(buf + 0);
		data_len = *(buf + 2);
		data = buf + 3;
		
		for(i = 0; i < ElectricityMeterConfigNum.number; i ++)
		{
			if(ElectricityMeterConfig[i].address == address &&
			   ElectricityMeterConfig[i].channel == 1)
			{
				voltage_ratio = ElectricityMeterConfig[i].voltage_ratio;
				current_ratio = ElectricityMeterConfig[i].current_ratio;

				break;
			}
		}
		
		if(address >= 32 && address <= 47)
		{
			if(data_len == 116)
			{
				meter_state->address = address;
				meter_state->channel = 1;
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[1].voltage,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[2].voltage,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[3].voltage,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[0].voltage,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[1].line_voltage,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[2].line_voltage,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[3].line_voltage,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[0].line_voltage,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[1].current,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[2].current,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[3].current,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[0].current,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[0].frequency,tmp,4);
				memcpy((void *)&meter_state->collect_para[1].frequency,tmp,4);
				memcpy((void *)&meter_state->collect_para[2].frequency,tmp,4);
				memcpy((void *)&meter_state->collect_para[3].frequency,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[1].active_power,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[2].active_power,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[3].active_power,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[0].active_power,tmp,4);
				
				data = buf + 3 + 100;
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[1].power_factor,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[2].power_factor,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[3].power_factor,tmp,4);
				
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[0].power_factor,tmp,4);
				
//				meter_state->update = 1;
				
			}
			else if(data_len == 16)
			{
				meter_state->address = address;
				meter_state->channel = 1;
				
				tmp[7] = *(data ++);
				tmp[6] = *(data ++);
				tmp[5] = *(data ++);
				tmp[4] = *(data ++);
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[0].active_energy,tmp,8);
				
				tmp[7] = *(data ++);
				tmp[6] = *(data ++);
				tmp[5] = *(data ++);
				tmp[4] = *(data ++);
				tmp[3] = *(data ++);
				tmp[2] = *(data ++);
				tmp[1] = *(data ++);
				tmp[0] = *(data ++);
				memcpy((void *)&meter_state->collect_para[0].reactive_energy,tmp,8);
				
				meter_state->update = 1;
			}
		}
		else if(address >= 48 && address <= 53)
		{
			if(data_len == 84)
			{
				meter_state->address = address;
				meter_state->channel = 1;
				
				for(i = 0; i < 6; i ++)
				{
					value1 = ((((u16)(*(data ++))) << 8) & 0xFF00) + 
					         (((u16)(*(data ++))) & 0x00FF);
					meter_state->collect_para[i + 1].voltage = (((float)value1) / 100.0f) * voltage_ratio;
					
					value1 = ((((u16)(*(data ++))) << 8) & 0xFF00) + 
					         (((u16)(*(data ++))) & 0x00FF);
					meter_state->collect_para[i + 1].current = (((float)value1) / 1000.0f) * current_ratio;
					
					value1 = ((((u16)(*(data ++))) << 8) & 0xFF00) + 
					         (((u16)(*(data ++))) & 0x00FF);
					meter_state->collect_para[i + 1].active_power = (((float)value1) / 1000.0f);
					
					value2 = ((((u32)(*(data ++))) << 24) & 0xFF000000) + 
					         ((((u32)(*(data ++))) << 16) & 0x00FF0000) +
					         ((((u32)(*(data ++))) << 8)  & 0x0000FF00) +
					         (((u32)(*(data ++))) & 0x000000FF);
					meter_state->collect_para[i + 1].active_energy = (((double)value2) / 100.0f);
					
					value1 = ((((u16)(*(data ++))) << 8) & 0xFF00) + 
					         (((u16)(*(data ++))) & 0x00FF);
					meter_state->collect_para[i + 1].power_factor = (((float)value1) / 1000.0f);
					
					value1 = ((((u16)(*(data ++))) << 8) & 0xFF00) + 
					         (((u16)(*(data ++))) & 0x00FF);
					meter_state->collect_para[i + 1].frequency = (((float)value1) / 100.0f);
				}
				
				meter_state->collect_para[0].voltage = (meter_state->collect_para[1].voltage + 
				                                          meter_state->collect_para[2].voltage + 
				                                          meter_state->collect_para[3].voltage + 
				                                          meter_state->collect_para[4].voltage +
				                                          meter_state->collect_para[5].voltage +
				                                          meter_state->collect_para[6].voltage) / 6.0f;
				
				meter_state->collect_para[0].current = (meter_state->collect_para[1].current + 
				                                          meter_state->collect_para[2].current + 
				                                          meter_state->collect_para[3].current + 
				                                          meter_state->collect_para[4].current +
				                                          meter_state->collect_para[5].current +
				                                          meter_state->collect_para[6].current);
				
				meter_state->collect_para[0].active_power = (meter_state->collect_para[1].active_power + 
				                                               meter_state->collect_para[2].active_power + 
				                                               meter_state->collect_para[3].active_power + 
				                                               meter_state->collect_para[4].active_power +
				                                               meter_state->collect_para[5].active_power +
				                                               meter_state->collect_para[6].active_power);
														  
				meter_state->collect_para[0].active_energy = (meter_state->collect_para[1].active_energy + 
				                                                meter_state->collect_para[2].active_energy + 
				                                                meter_state->collect_para[3].active_energy + 
				                                                meter_state->collect_para[4].active_energy +
				                                                meter_state->collect_para[5].active_energy +
				                                                meter_state->collect_para[6].active_energy);
																
				meter_state->collect_para[0].power_factor = (meter_state->collect_para[1].power_factor + 
				                                               meter_state->collect_para[2].power_factor + 
				                                               meter_state->collect_para[3].power_factor + 
				                                               meter_state->collect_para[4].power_factor +
				                                               meter_state->collect_para[5].power_factor +
				                                               meter_state->collect_para[6].power_factor) / 6.0f;
															   
				meter_state->collect_para[0].frequency = meter_state->collect_para[1].frequency;
				
				meter_state->update = 1;
			}
		}
	}
}





























































