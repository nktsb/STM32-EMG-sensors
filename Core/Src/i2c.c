/*
 * -----------------------------------------------------------------------------
 * Project       : STM32-EMG-sensors
 * File          : i2c.c
 * Author        : nktsb
 * Created on    : 24.03.2024
 * GitHub        : https://github.com/nktsb/STM32-EMG-sensors
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 nktsb
 * All rights reserved.
 * -----------------------------------------------------------------------------
 */

#include "i2c.h"
#include "sensors.h"
#include "timer.h"
#include "ring_buffer.h"

#define I2C_DEFAULT_TMO							20
#define I2C_REQUEST_WRITE                       0x00
#define I2C_REQUEST_READ                        0x01

typedef struct i2c_buff_st
{
	struct timer timeout;
} i2c_st;

static i2c_st I2C = {0};

void i2cInit(void)
{
	LL_I2C_InitTypeDef I2C_InitStruct = {0};
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

	LL_GPIO_AF_EnableRemap_I2C1();

	GPIO_InitStruct.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;

	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	LL_I2C_DisableOwnAddress2(I2C1);
	LL_I2C_DisableGeneralCall(I2C1);

	I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
	I2C_InitStruct.ClockSpeed = 400000;
	I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_2;
	I2C_InitStruct.OwnAddress1 = 1;
	I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
	I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	LL_I2C_Init(I2C1, &I2C_InitStruct);
	LL_I2C_SetClockSpeedMode(I2C1, LL_I2C_CLOCK_SPEED_FAST_MODE);

	LL_I2C_Enable(I2C1);
}

static void i2cStart(void)
{
	LL_I2C_ClearFlag_STOP(I2C1);
	LL_I2C_DisableBitPOS(I2C1);
	LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
}

static void i2cStop(void)
{
	LL_I2C_GenerateStopCondition(I2C1);
	LL_I2C_ClearFlag_STOP(I2C1);
}

void i2cWriteData(uint8_t slave_addr, uint8_t reg, reg_size_t reg_size, uint8_t *buf, uint16_t data_len)
{
	i2cStart();
	LL_I2C_GenerateStartCondition(I2C1);
	timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
	while(!LL_I2C_IsActiveFlag_SB(I2C1))
	{
		if(timer_expired(&I2C.timeout))
		{
			i2cStop();
			return;
		}
	}

	(void)I2C1->SR1;
	LL_I2C_TransmitData8(I2C1, ((slave_addr << 1) | I2C_REQUEST_WRITE));

	timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
	while(!LL_I2C_IsActiveFlag_ADDR(I2C1))
	{
		if(timer_expired(&I2C.timeout))
		{
			i2cStop();
			return;
		}
	}

	LL_I2C_ClearFlag_ADDR(I2C1);

	if(reg_size == REG_SIZE_16BIT)
	{
		LL_I2C_TransmitData8(I2C1, (uint8_t)(reg >> 8));
		timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
		while(!LL_I2C_IsActiveFlag_TXE(I2C1))
		{
			if(timer_expired(&I2C.timeout))
			{
				i2cStop();
				return;
			}
		}
	}

	LL_I2C_TransmitData8(I2C1, (uint8_t)reg);
	timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
	while(!LL_I2C_IsActiveFlag_TXE(I2C1))
	{
		if(timer_expired(&I2C.timeout))
		{
			i2cStop();
			return;
		}
	}

	for(uint16_t i = 0; i < data_len; i++)
	{
		LL_I2C_TransmitData8(I2C1, buf[i]);
		timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
		while(!LL_I2C_IsActiveFlag_TXE(I2C1))
		{
			if(timer_expired(&I2C.timeout))
			{
				i2cStop();
				return;
			}
		}
	}
	i2cStop();
}

void i2cReadData(uint8_t slave_addr, uint16_t reg, reg_size_t reg_size, uint8_t *buf, uint16_t data_len)
{
	i2cStart();
	LL_I2C_GenerateStartCondition(I2C1);

	timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
	while(!LL_I2C_IsActiveFlag_SB(I2C1))
	{
		if(timer_expired(&I2C.timeout))
		{
			i2cStop();
			return;
		}
	}

	(void) I2C1->SR1;
	LL_I2C_TransmitData8(I2C1, ((slave_addr << 1) | I2C_REQUEST_WRITE));

	timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
	while(!LL_I2C_IsActiveFlag_ADDR(I2C1))
	{
		if(timer_expired(&I2C.timeout))
		{
			i2cStop();
			return;
		}
	}

	LL_I2C_ClearFlag_ADDR(I2C1);

	if(reg_size == REG_SIZE_16BIT)
	{
		LL_I2C_TransmitData8(I2C1, (uint8_t) (reg >> 8));

		timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
		while(!LL_I2C_IsActiveFlag_TXE(I2C1))
		{
			if(timer_expired(&I2C.timeout))
			{
				i2cStop();
				return;
			}
		}
	}

	LL_I2C_TransmitData8(I2C1, (uint8_t) reg);

	timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
	while(!LL_I2C_IsActiveFlag_TXE(I2C1))
	{
		if(timer_expired(&I2C.timeout))
		{
			i2cStop();
			return;
		}
	}

	LL_I2C_GenerateStartCondition(I2C1);

	timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
	while(!LL_I2C_IsActiveFlag_SB(I2C1))
	{
		if(timer_expired(&I2C.timeout))
		{
			i2cStop();
			return;
		}
	}

	(void) I2C1->SR1;
	LL_I2C_TransmitData8(I2C1, ((slave_addr << 1) | I2C_REQUEST_READ));

	timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
	while(!LL_I2C_IsActiveFlag_ADDR(I2C1))
	{
		if(timer_expired(&I2C.timeout))
		{
			i2cStop();
			return;
		}
	}
	LL_I2C_ClearFlag_ADDR(I2C1);

	for(uint8_t i = 0; i < data_len - 1; i++)
	{
		timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
		while(!LL_I2C_IsActiveFlag_RXNE(I2C1))
		{
			if(timer_expired(&I2C.timeout))
			{
				i2cStop();
				return;
			}
		}
		*buf++ = I2C1->DR;
	}

	LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
	i2cStop();

	timer_set(&I2C.timeout, I2C_DEFAULT_TMO);
	while(!LL_I2C_IsActiveFlag_RXNE(I2C1))
	{
		if(timer_expired(&I2C.timeout))
		{
			i2cStop();
			return;
		}
	}
	*buf = I2C1->DR;
}
