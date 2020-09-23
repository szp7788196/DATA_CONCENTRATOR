#include "battery.h"
#include "e_meter.h"


BatteryManagement_S BatteryManagement;



__IO uint16_t AdcValBuf[ADC_POINT_NUM][ADC_CH_NUM];


void ADC1_DMA_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;    
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;    

   /* Enable ADC1, DMA2 and GPIO clocks ****************************************/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /* DMA2 Stream0 channe0 configuration **************************************/
    DMA_DeInit(DMA2_Stream0);
    DMA_InitStructure.DMA_Channel = DMA_Channel_0; 
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&AdcValBuf;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStructure.DMA_BufferSize = ADC_CH_NUM * ADC_POINT_NUM;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;//DMA_FIFOMode_Enable;         
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream0, ENABLE);

    /* Configure ADC1 Channe0 pin as analog input ******************************/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

   /* ADC Common Init **********************************************************/
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//ADC_DMAAccessMode_1;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);   

    /* ADC1 Init ****************************************************************/
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 2;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC1 regular channel configuration ******************************************/
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_480Cycles);
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_480Cycles);  

    /* Enable DMA request after last transfer (Single-ADC mode) */
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

    /* Enable ADC3 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* Enable ADC3 */
    ADC_Cmd(ADC1, ENABLE);

    ADC_SoftwareStartConv(ADC1);
}


u16 ADCGetAverageVal(u8 ch)
{
	u8 i = 0;
	u32 result = 0;

	for(i = 0; i < ADC_POINT_NUM; i ++)
	{
		result += AdcValBuf[i][ch];
	}
	
	result = result / (u32)ADC_POINT_NUM;
	
	return (u16)result;
}

float GetCurrentVal(void)
{
	float result = 0;

	result = 0.0f - (((((float)ADCGetAverageVal(0) / 4096.0f) * 3.26f) - 2.5f) / 0.185f);
	
	return result;
}

float GetVoltageVal(void)
{
	float result = 0;

	result = ADCGetAverageVal(1) * BATTERY_VOLTAGE_SCALE;
	
	return result;
}

void BetteryGetParas(void)
{
	if(BulitInMeterParas.voltage[1] > 70.f)		//输入电压大于70V认为电池在充电
	{
		BatteryManagement.state = BATTERY_CHARGING;
		BatteryManagement.charge_voltage = GetVoltageVal();
		BatteryManagement.charge_current = GetCurrentVal();
		BatteryManagement.discharge_voltage = BatteryManagement.charge_voltage;
		BatteryManagement.discharge_current = 0.0f;
	}
	else
	{
		BatteryManagement.state = BATTERY_DISCHARGING;
		BatteryManagement.charge_voltage = 0.0f;
		BatteryManagement.charge_current = 0.0f;
		BatteryManagement.discharge_voltage = GetVoltageVal();
		BatteryManagement.discharge_current = GetCurrentVal();
	}
}

















































