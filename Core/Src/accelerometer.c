/*
 * -----------------------------------------------------------------------------
 * Project       : STM32-EMG-sensors
 * File          : accelerometer.c
 * Author        : nktsb
 * Created on    : 24.03.2024
 * GitHub        : https://github.com/nktsb/STM32-EMG-sensors
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 nktsb
 * All rights reserved.
 * -----------------------------------------------------------------------------
 */


#include "accelerometer.h"
#include "sensors.h"
#include "i2c.h"
#include "delay.h"

#define MPU6050_ADDR 0x68

#define MPU6050_SMPRT_DIV		0X19
#define MPU6050_WHO_AM_I		0X75
#define MPU6050_CONFIG			0X1A
#define MPU6050_GYRO_CONFIG		0X1B
#define MPU6050_ACCEL_CONFIG	0X1C
#define MPU6050_INT_ENABLE		0X38
#define MPU6050_INT_STATUS		0X3A
#define MPU6050_ACCEL_XOUT_H	0X3B
#define MPU6050_ACCEL_XOUT_L	0X3C
#define MPU6050_PWR_MGMT_1		0X6B

enum {
	ACCEL_2G_RANGE = 0,
	ACCEL_4G_RANGE,
	ACCEL_8G_RANGE,
	ACCEL_16G_RANGE,

	ACCEL_RANGES_NUM
};

enum {
	GYRO_250_DEGREES = 0,
	GYRO_500_DEGREES,
	GYRO_1000_DEGREES,
	GYRO_2000_DEGREES,

	GYRO_RANGES_NUM
};

const uint16_t accel_coeffs[ACCEL_RANGES_NUM] = {16384, 8192, 4096, 2048};
const uint16_t gyro_coeffs[GYRO_RANGES_NUM] = {1310, 655, 328, 164}; // x10

static const uint8_t accel_selected_range =  ACCEL_4G_RANGE;
static const uint8_t gyro_selected_range =  GYRO_2000_DEGREES;

static int16_t accel_data_buff[3] = {0};
static int16_t gyro_data_buff[3] = {0};

static void accelWriteReg(uint8_t reg, uint8_t val)
{
	uint8_t buff = val;
	i2cWriteData(MPU6050_ADDR, reg, REG_SIZE_8BIT, &buff, sizeof(buff));
}

static void accelReadData(uint8_t start_reg, uint8_t *buff, uint16_t len)
{
	i2cReadData(MPU6050_ADDR, start_reg, REG_SIZE_8BIT, buff, len);
}

void accelInit(void)
{
	i2cInit();

	accelWriteReg(MPU6050_PWR_MGMT_1, 1 << 7);
	delay(100);
	accelWriteReg(MPU6050_PWR_MGMT_1, 0);
	delay(50);
	accelWriteReg(MPU6050_SMPRT_DIV, 0); // 8kHz if DLPF disabled, 1kHz if DLPF Enabled
	accelWriteReg(MPU6050_CONFIG, 0);

	accelWriteReg(MPU6050_GYRO_CONFIG, (gyro_selected_range << 3));
	accelWriteReg(MPU6050_ACCEL_CONFIG, (accel_selected_range << 3));
}

void accelUpdateData(void)
{
	uint8_t buff[14] = {0};
	accelReadData(MPU6050_ACCEL_XOUT_H, buff, 14);
	for(uint8_t i = 0; i < 3; i++)
	{
		accel_data_buff[i] = (int16_t)(buff[0 + i*2] << 8 | buff[1 + i*2]) * 1000 / accel_coeffs[accel_selected_range];
		gyro_data_buff[i] = (int16_t)(buff[8 + i*2] << 8 | buff[9 + i*2]) * 10 / gyro_coeffs[gyro_selected_range];
	}
}

int16_t *getAccelDataPtr(void)
{
	return accel_data_buff;
}

int16_t *getGyroDataPtr(void)
{
	return gyro_data_buff;
}
