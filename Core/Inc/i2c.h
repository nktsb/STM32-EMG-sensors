/*
 * -----------------------------------------------------------------------------
 * Project       : STM32-EMG-sensors
 * File          : i2c.h
 * Author        : nktsb
 * Created on    : 24.03.2024
 * GitHub        : https://github.com/nktsb/STM32-EMG-sensors
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 nktsb
 * All rights reserved.
 * -----------------------------------------------------------------------------
 */

#ifndef __I2C_H_
#define __I2C_H_

#include <stdint.h>
#include "stm32f103xb.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_gpio.h"

typedef enum {
    REG_SIZE_8BIT = 1,
    REG_SIZE_16BIT
} reg_size_t;

void i2cInit(void);
void i2cWriteData(uint8_t slave_addr, uint8_t reg, reg_size_t reg_size, uint8_t *buf, uint16_t data_len);
void i2cReadData(uint8_t slave_addr, uint16_t reg, reg_size_t reg_size, uint8_t *buf, uint16_t data_len);

#endif /* __I2C_H_ */
