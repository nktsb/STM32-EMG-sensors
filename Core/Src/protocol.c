/*
 * -----------------------------------------------------------------------------
 * Project       : STM32-EMG-sensors
 * File          : protocol.c
 * Author        : nktsb
 * Created on    : 24.03.2024
 * GitHub        : https://github.com/nktsb/STM32-EMG-sensors
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 nktsb
 * All rights reserved.
 * -----------------------------------------------------------------------------
 */

#include "protocol.h"
#include "usart.h"
#include "main.h"
#include "sensors.h"

static const char* emg_preamble = "EMG";
static const char* accel_preamble = "ACC";
static const char* gyro_preamble = "GYR";

typedef struct frame {
	char emg_send_buff[SENSORS_NUM][6 + FRAME_SIZE * 6 + 2 + 1];
	char accel_send_buff[AXES_NUM][6 + FRAME_SIZE * 6 + 2 + 1];
	char gyro_send_buff[AXES_NUM][6 + FRAME_SIZE * 6 + 2 + 1];

	char *emg_str_data_buff[SENSORS_NUM];
	char *accel_str_data_buff[SENSORS_NUM];
	char *gyro_str_data_buff[SENSORS_NUM];

	uint8_t counter;
	bool frame_not_empty;
} frame_st;

static frame_st frame = {0};

void protocolInit(void)
{
	for(sensor_t ch = 0; ch < SENSORS_NUM; ch++)
	{
		strcpy(frame.emg_send_buff[ch], emg_preamble);
		itoa(ch, &frame.emg_send_buff[ch][strlen(frame.emg_send_buff[ch])], 10);
		strcat(frame.emg_send_buff[ch], ":");

		frame.emg_str_data_buff[ch] = &frame.emg_send_buff[ch][strlen(frame.emg_send_buff[ch])];
	}

	const char *axes[AXES_NUM] = {"X", "Y", "Z"};
	for(axes_t ax = 0; ax < AXES_NUM; ax++)
	{
		strcpy(frame.accel_send_buff[ax], accel_preamble);
		strcpy(frame.gyro_send_buff[ax], gyro_preamble);
		strcat(frame.accel_send_buff[ax], axes[ax]);
		strcat(frame.gyro_send_buff[ax], axes[ax]);
		strcat(frame.accel_send_buff[ax], ":");
		strcat(frame.gyro_send_buff[ax], ":");

		frame.accel_str_data_buff[ax] = &frame.accel_send_buff[ax][strlen(frame.accel_send_buff[ax])];
		frame.gyro_str_data_buff[ax] = &frame.gyro_send_buff[ax][strlen(frame.gyro_send_buff[ax])];
	}
}

void protocolUpdateFrame(uint16_t *emg_adc_data, int16_t *accel_data, int16_t *gyro_data)
{
	for(sensor_t ch = SENSOR_1; ch < SENSORS_NUM; ch++)
	{
		itoa(emg_adc_data[ch], &frame.emg_str_data_buff[ch][strlen(frame.emg_str_data_buff[ch])], 10);
		strcat(frame.emg_str_data_buff[ch], ",");
	}

	for(axes_t ax = AXES_X; ax < AXES_NUM; ax++)
	{
		itoa(accel_data[ax], &frame.accel_str_data_buff[ax][strlen(frame.accel_str_data_buff[ax])], 10);
		strcat(frame.accel_str_data_buff[ax], ",");
		itoa(gyro_data[ax], &frame.gyro_str_data_buff[ax][strlen(frame.gyro_str_data_buff[ax])], 10);
		strcat(frame.gyro_str_data_buff[ax], ",");
	}
	
	if(++frame.counter == FRAME_SIZE)
	{
		frame.frame_not_empty = true;

		for(sensor_t ch = SENSOR_1; ch < SENSORS_NUM; ch++)
		{
			strcpy(&frame.emg_str_data_buff[ch][strlen(frame.emg_str_data_buff[ch]) - 1], "\r\n");
			uartPutToTxBuff((uint8_t*)frame.emg_send_buff[ch], strlen(frame.emg_send_buff[ch]));
		}
		for(axes_t ax = AXES_X; ax < AXES_NUM; ax++)
		{
			strcpy(&frame.accel_str_data_buff[ax][strlen(frame.accel_str_data_buff[ax]) - 1], "\r\n");
			strcpy(&frame.gyro_str_data_buff[ax][strlen(frame.gyro_str_data_buff[ax]) - 1], "\r\n");

			uartPutToTxBuff((uint8_t*)frame.accel_send_buff[ax], strlen(frame.accel_send_buff[ax]));
			uartPutToTxBuff((uint8_t*)frame.gyro_send_buff[ax], strlen(frame.gyro_send_buff[ax]));
		}
	
		protocolClearFrame();
	}
}

void protocolClearFrame(void)
{
	for(sensor_t ch = 0; ch < SENSORS_NUM; ch++)
	{
		memset((void*)frame.emg_str_data_buff[ch], 0, strlen(frame.emg_str_data_buff[ch]));
	}
	for(axes_t ax = 0; ax < AXES_NUM; ax++)
	{
		memset((void*)frame.accel_str_data_buff[ax], 0, strlen(frame.accel_str_data_buff[ax]));
		memset((void*)frame.gyro_str_data_buff[ax], 0, strlen(frame.gyro_str_data_buff[ax]));
	}
	frame.frame_not_empty = false;
	frame.counter = 0;
}

bool isFrameEmpty(void)
{
	return !frame.frame_not_empty;
}

static void protocolParseInput(void)
{
	if(usartGetRxLen() > 0)
	{
		uint8_t data = usartReadByte();
		char *answer = NULL;

		switch(data)
		{
			case 'p': // ping
				answer = "EMG sensors here\r\n";
				uartPutToTxBuff((uint8_t*)answer, strlen(answer));
				break;
			case 'r': // run measure
				sensorRunMeas();
				break;
			case 's': // stop measure
				sensorStopMeas();
				break;
			default:
				break;
		}
	}
}


void protocolTask(void)
{
	protocolParseInput();
}