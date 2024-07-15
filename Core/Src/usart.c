/*
 * -----------------------------------------------------------------------------
 * Project       : STM32-EMG-sensors
 * File          : usart.c
 * Author        : nktsb
 * Created on    : 24.03.2024
 * GitHub        : https://github.com/nktsb/STM32-EMG-sensors
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 nktsb
 * All rights reserved.
 * -----------------------------------------------------------------------------
 */

#include "usart.h"
#include "ring_buffer.h"
#include "sensors.h"

#define BAUDRATE 2000000U

ring_buffer_st *uart_rx_buffer = NULL;
ring_buffer_st *uart_tx_buffer = NULL;

void uartInit(void)
{
	uart_rx_buffer = ringBufferInit(256);
	uart_tx_buffer = ringBufferInit(2048);

	LL_USART_InitTypeDef USART_InitStruct = {0};

	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_2;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_FLOATING;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	USART_InitStruct.BaudRate = BAUDRATE;
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART2, &USART_InitStruct);
	LL_USART_ConfigAsyncMode(USART2);

	LL_USART_EnableIT_RXNE(USART2);
	NVIC_EnableIRQ(USART2_IRQn);
	NVIC_SetPriority(USART2_IRQn, 0);

	LL_USART_Enable(USART2);
}

void uartPutToTxBuff(uint8_t *buff, uint16_t size)
{
	ringBufferPutData(uart_tx_buffer, buff, size);
}

static void uartSendOutput(void)
{
	uint16_t output_size = ringBufferGetAvail(uart_tx_buffer);
	for(uint16_t i = 0; i < output_size; i++)
	{
		USART2->DR = ringBufferGetByte(uart_tx_buffer);
		while(!(USART2->SR & USART_SR_TC));
	}
}

uint16_t usartGetRxLen(void)
{
	return ringBufferGetAvail(uart_rx_buffer);
}

uint8_t usartReadByte(void)
{
	return ringBufferGetByte(uart_rx_buffer);
}

void uartTask(void)
{
	uartSendOutput();
}

void USART2_IRQHandler(void)
{
	uint8_t buff = USART2->DR;
	ringBufferPutByte(uart_rx_buffer, buff);
}