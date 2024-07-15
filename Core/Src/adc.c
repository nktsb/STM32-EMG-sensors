/*
 * -----------------------------------------------------------------------------
 * Project       : STM32-EMG-sensors
 * File          : adc.c
 * Author        : nktsb
 * Created on    : 24.03.2024
 * GitHub        : https://github.com/nktsb/STM32-EMG-sensors
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 nktsb
 * All rights reserved.
 * -----------------------------------------------------------------------------
 */

#include "adc.h"
#include "delay.h"
#include "sensors.h"

void adcInit(void)
{
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

	/**ADC1 GPIO Configuration
	PA0   ------> ADC1_IN0
	PA1   ------> ADC1_IN1
	PA4   ------> ADC1_IN4
	PB0   ------> ADC1_IN8
	*/

	LL_ADC_Disable(ADC1);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);

	LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_CONTINUOUS);

	LL_ADC_INJ_SetTriggerSource(ADC1, LL_ADC_INJ_TRIG_SOFTWARE);
	LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);
	LL_ADC_INJ_SetSequencerLength(ADC1, LL_ADC_INJ_SEQ_SCAN_ENABLE_4RANKS);
	LL_ADC_SetSequencersScanMode(ADC1, LL_ADC_SEQ_SCAN_ENABLE);
	LL_ADC_INJ_SetTrigAuto(ADC1, LL_ADC_INJ_TRIG_FROM_GRP_REGULAR);

	LL_ADC_INJ_SetSequencerRanks(ADC1, LL_ADC_INJ_RANK_1, LL_ADC_CHANNEL_0);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_7CYCLES_5);
	LL_ADC_INJ_SetOffset(ADC1, LL_ADC_INJ_RANK_1, 0);

	LL_ADC_INJ_SetSequencerRanks(ADC1, LL_ADC_INJ_RANK_2, LL_ADC_CHANNEL_1);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_7CYCLES_5);
	LL_ADC_INJ_SetOffset(ADC1, LL_ADC_INJ_RANK_2, 0);

	LL_ADC_INJ_SetSequencerRanks(ADC1, LL_ADC_INJ_RANK_3, LL_ADC_CHANNEL_4);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_7CYCLES_5);
	LL_ADC_INJ_SetOffset(ADC1, LL_ADC_INJ_RANK_3, 0);

	LL_ADC_INJ_SetSequencerRanks(ADC1, LL_ADC_INJ_RANK_4, LL_ADC_CHANNEL_8);
	LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_8, LL_ADC_SAMPLINGTIME_7CYCLES_5);
	LL_ADC_INJ_SetOffset(ADC1, LL_ADC_INJ_RANK_4, 0);

	LL_ADC_Enable(ADC1);

	delay(5);
	LL_ADC_StartCalibration(ADC1);
	while(LL_ADC_IsCalibrationOnGoing(ADC1));
}

static uint16_t adc_buff[4] = {0};

uint16_t *getAdcDataPtr(void)
{
	return adc_buff;
}

void adcUpdateData(void)
{
	LL_ADC_INJ_StartConversionSWStart(ADC1);
	while(!LL_ADC_IsActiveFlag_JEOS(ADC1));
	adc_buff[0] = LL_ADC_INJ_ReadConversionData12(ADC1, LL_ADC_INJ_RANK_1);
	adc_buff[1] = LL_ADC_INJ_ReadConversionData12(ADC1, LL_ADC_INJ_RANK_2);
	adc_buff[2] = LL_ADC_INJ_ReadConversionData12(ADC1, LL_ADC_INJ_RANK_3);
	adc_buff[3] = LL_ADC_INJ_ReadConversionData12(ADC1, LL_ADC_INJ_RANK_4);
	LL_ADC_ClearFlag_JEOS(ADC1);
}
