/*
 * -----------------------------------------------------------------------------
 * Project       : STM32-EMG-sensors
 * File          : ring_buffer.c
 * Author        : nktsb
 * Created on    : 24.03.2024
 * GitHub        : https://github.com/nktsb/STM32-EMG-sensors
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 nktsb
 * All rights reserved.
 * -----------------------------------------------------------------------------
 */

#include "ring_buffer.h"

ring_buffer_st *ringBufferInit(uint16_t size)
{
	ring_buffer_st * buffer = calloc(1, sizeof(ring_buffer_st));

	buffer->input = 0;
	buffer->output = 0;

	buffer->buff = calloc(size, sizeof(uint8_t));
	buffer->size = size;

	return buffer;
}

void ringBufferPutByte(ring_buffer_st *buffer, uint8_t byte)
{
	buffer->buff[buffer->input] = byte;

	buffer->input += 1;
	buffer->input %= buffer->size;
}

void ringBufferPutData(ring_buffer_st *buffer, uint8_t *data, uint16_t data_len)
{
	while(data_len)
	{
		uint16_t round_max_len = (buffer->size - buffer->input);
		uint16_t round_len = (data_len > round_max_len)? round_max_len: data_len;
		
		memcpy((void*)(buffer->buff + buffer->input), (void*)data, round_len);
		data += round_len;
		data_len -= round_len;

		buffer->input += round_len;
		buffer->input %= buffer->size;
	}
}

uint8_t ringBufferGetByte(ring_buffer_st *buffer)
{
	uint8_t res = buffer->buff[buffer->output];

	buffer->output += 1;
	buffer->output %= buffer->size;

	return res;
}

uint16_t ringBufferGetAvail(ring_buffer_st *buffer)
{
	return (buffer->input >= buffer->output)? 
			buffer->input - buffer->output : ((buffer->size - buffer->output) + buffer->input);
}

void ringBufferDeinit(ring_buffer_st *buffer)
{
	buffer->input = 0;
	buffer->output = 0;
	buffer->size = 0;
	free(buffer->buff);
	free(buffer);
}