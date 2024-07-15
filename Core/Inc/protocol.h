#ifndef __PROTOCOL_H_
#define __PROTOCOL_H_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sensors.h"

void protocolInit(void);
void protocolUpdateFrame(uint16_t *emg_adc_data, int16_t *accel_data, int16_t *gyro_data);
void protocolClearFrame(void);
void protocolTask(void);
bool isFrameEmpty(void);

#endif /* __PROTOCOL_H_ */
