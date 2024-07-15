/*
 * -----------------------------------------------------------------------------
 * Project       : STM32-EMG-sensors
 * File          : sensors.c
 * Author        : nktsb
 * Created on    : 24.03.2024
 * GitHub        : https://github.com/nktsb/STM32-EMG-sensors
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 nktsb
 * All rights reserved.
 * -----------------------------------------------------------------------------
 */

#include "sensors.h"
#include "usart.h"
#include "adc.h"
#include "main.h"
#include "emul.h"
#include "timer.h"
#include "accelerometer.h"
#include "protocol.h"

// #define EMUL_MODE

static uint16_t *emg_data_buff_ptr = NULL;
static int16_t *accel_data_buff_ptr = NULL;
static int16_t *gyro_data_buff_ptr = NULL;
static bool measure_flg = false;

static void sensorsTimInit(void)
{
	LL_TIM_DisableCounter(TIM2);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	LL_TIM_SetPrescaler(TIM2, 64 - 1);
	LL_TIM_SetCounterMode(TIM2, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetAutoReload(TIM2, (1000000 / SAMPLING_FREQUENCY) - 1);

	LL_TIM_DisableARRPreload(TIM2);
	LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM2);
	LL_TIM_EnableIT_UPDATE(TIM2);

	NVIC_SetPriority(TIM2_IRQn, 1);
	NVIC_EnableIRQ(TIM2_IRQn);
	LL_TIM_EnableCounter(TIM2);
}

void sensorsInit(void)
{
#ifdef EMUL_MODE
	emg_data_buff_ptr = getEmgEmulPtr();
	accel_data_buff_ptr = getAccelEmulPtr();
	gyro_data_buff_ptr = getGyroEmulPtr();
#else
	adcInit();
	accelInit();
	emg_data_buff_ptr = getAdcDataPtr();
	accel_data_buff_ptr = getAccelDataPtr();
	gyro_data_buff_ptr = getGyroDataPtr();
#endif /* EMUL_MODE */
	sensorsTimInit();
}

void sensorsTask(void)
{
}

void sensorsCollectData(void)
{
	if(measure_flg && isFrameEmpty())
	{
#ifdef EMUL_MODE
		emulSensorsData();
#else
		adcUpdateData();
		accelUpdateData();
#endif /* EMUL_MODE */

		protocolUpdateFrame(emg_data_buff_ptr, accel_data_buff_ptr, gyro_data_buff_ptr);
	}
}

void sensorRunMeas(void)
{
	measure_flg = true;
	protocolClearFrame();
}

void sensorStopMeas(void)
{
	measure_flg = false;
	protocolClearFrame();
}