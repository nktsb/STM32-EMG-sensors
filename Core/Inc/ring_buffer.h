/*
 * -----------------------------------------------------------------------------
 * Project       : STM32-EMG-sensors
 * File          : ring_buffer.h
 * Author        : nktsb
 * Created on    : 24.03.2024
 * GitHub        : https://github.com/nktsb/STM32-EMG-sensors
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 nktsb
 * All rights reserved.
 * -----------------------------------------------------------------------------
 */

#ifndef __RING_BUFFER_H_
#define __RING_BUFFER_H_

#include "main.h"

#define RING_BUFFER_FAST_IS_DATA(_buffer_) 				( (_buffer_)->input != (_buffer_)->output )
#define RING_BUFFER_FAST_CLEAR(_buffer_) 				(_buffer_)->input = 0; (_buffer_)->output = 0;


// only for buffers with data_size == 1 and size == 2^n
#define RING_BUFFER_FAST_PUT_8BIT(_buffer_, _data_) 	*(uint8_t*)((_buffer_)->buff + (_buffer_)->input) = (_data_); \
														(_buffer_)->input++; \
														(_buffer_)->input %= (_buffer_)->size;

#define RING_BUFFER_FAST_GET_8BIT(_buffer_, _dst_)  	(_dst_) = *(uint8_t*)((_buffer_)->buff + (_buffer_)->output); \
														(_buffer_)->output++; \
														(_buffer_)->output %= (_buffer_)->size;
// ...


typedef struct ring_buffer
{
    uint8_t* buff;

    uint16_t input;
    uint16_t output;
    uint16_t size;
} ring_buffer_st;

ring_buffer_st *ringBufferInit(uint16_t size);
void ringBufferPutByte(ring_buffer_st *buffer, uint8_t byte);
void ringBufferPutData(ring_buffer_st *buffer, uint8_t *data, uint16_t data_len);
uint8_t ringBufferGetByte(ring_buffer_st *buffer);
uint16_t ringBufferGetAvail(ring_buffer_st *buffer);
void ringBufferDeinit(ring_buffer_st *buffer);

#endif /* __RING_BUFFER_H_ */