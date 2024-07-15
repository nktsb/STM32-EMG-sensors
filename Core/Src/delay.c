/*
 * -----------------------------------------------------------------------------
 * Project       : STM32-EMG-sensors
 * File          : delay.c
 * Author        : nktsb
 * Created on    : 24.03.2024
 * GitHub        : https://github.com/nktsb/STM32-EMG-sensors
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 nktsb
 * All rights reserved.
 * -----------------------------------------------------------------------------
 */

#include "delay.h"

static struct timer delay_timer = {0};

void delay(clock_time_t time)
{
	timer_set(&delay_timer, time);
	while(!timer_expired(&delay_timer));
}