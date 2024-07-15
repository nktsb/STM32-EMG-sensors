/*
 * -----------------------------------------------------------------------------
 * Project       : STM32-EMG-sensors
 * File          : sensors.h
 * Author        : nktsb
 * Created on    : 24.03.2024
 * GitHub        : https://github.com/nktsb/STM32-EMG-sensors
 * -----------------------------------------------------------------------------
 * Copyright (c) 2024 nktsb
 * All rights reserved.
 * -----------------------------------------------------------------------------
 */

#ifndef __SENSORS_H_
#define __SENSORS_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "usart.h"

#define SAMPLING_FREQUENCY	1000		// Hz
#define FRAME_SIZE			5

typedef enum
{
    SENSOR_1,
    SENSOR_2,
    SENSOR_3,
    SENSOR_4,

    SENSORS_NUM

} sensor_t;

typedef enum
{
    AXES_X,
    AXES_Y,
    AXES_Z,

    AXES_NUM
} axes_t;

void sensorsInit(void);
void sensorsCollectData(void);
void sensorRunMeas(void);
void sensorStopMeas(void);
void sensorsTask(void);

#endif /* __SENSORS_H_ */