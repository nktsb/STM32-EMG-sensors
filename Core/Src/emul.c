/*
 * -----------------------------------------------------------------------------
 * Project       : STM32-EMG-sensors
 * File          : emul.c
 * Author        : nktsb
 * Created on    : 24.03.2024
 * GitHub        : https://github.com/nktsb/STM32-EMG-sensors
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 nktsb
 * All rights reserved.
 * -----------------------------------------------------------------------------
 */

#include "emul.h"
#include "sensors.h"

#define MAX_EMUL_CNT	4000

#define MIDDLE_VAL		2047
#define MAX_VAL			4095
#define MIN_VAL			0

static uint16_t emg_data_buff[SENSORS_NUM] = {0};
static int16_t accel_data_buff[AXES_NUM] = {0};
static int16_t gyro_data_buff[AXES_NUM] = {0};

static uint16_t triangle(uint16_t x, uint16_t period, uint16_t min_val, uint16_t max_val)
{
	uint16_t half_period = period / 2;
	x = x % period;

	uint16_t res = 0;
	if(x < half_period)
		res = min_val + ((max_val - min_val) * x / half_period);
	else
		res = max_val - ((max_val - min_val) * (x - half_period) / half_period);
	return res;
}

uint16_t *getEmgEmulPtr(void)
{
	return emg_data_buff;
}

int16_t *getAccelEmulPtr(void)
{
	return accel_data_buff;
}

int16_t *getGyroEmulPtr(void)
{
	return gyro_data_buff;
}

void emulSensorsData(void)
{
	static uint16_t emul_counter = 0;

	emg_data_buff[SENSOR_1] = MIDDLE_VAL + ((MAX_VAL - MIN_VAL) / 2) *
			sin((2 * M_PI) * emul_counter * 1 / MAX_EMUL_CNT);
	emg_data_buff[SENSOR_2] = MIDDLE_VAL + ((MAX_VAL - MIN_VAL) / 2) *
			sin((2 * M_PI) * emul_counter * 2 / MAX_EMUL_CNT);
	emg_data_buff[SENSOR_3] = MIDDLE_VAL + ((MAX_VAL - MIN_VAL) / 2) *
			sin((2 * M_PI) * emul_counter * 3 / MAX_EMUL_CNT);
	emg_data_buff[SENSOR_4] = MIDDLE_VAL + ((MAX_VAL - MIN_VAL) / 2) *
			sin((2 * M_PI) * emul_counter * 4 / MAX_EMUL_CNT);
	
	accel_data_buff[AXES_X] = triangle(emul_counter, MAX_EMUL_CNT/50, 600, 800); 
	accel_data_buff[AXES_Y] = triangle(emul_counter, MAX_EMUL_CNT/50, 1947, 2147); 
	accel_data_buff[AXES_Z] = triangle(emul_counter, MAX_EMUL_CNT/50, 3200, 3400); 

	gyro_data_buff[AXES_X] = triangle(emul_counter, MAX_EMUL_CNT/50, -300, -500); 
	gyro_data_buff[AXES_Y] = triangle(emul_counter, MAX_EMUL_CNT/50, -1000, -1200); 
	gyro_data_buff[AXES_Z] = triangle(emul_counter, MAX_EMUL_CNT/50, -1600, -1800); 

	if(++emul_counter >= MAX_EMUL_CNT) emul_counter = 0;
}