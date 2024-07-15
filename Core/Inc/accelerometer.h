#ifndef __ACCEL_H_
#define __ACCEL_H_

#include <stdint.h>
#include "stm32f103xb.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_gpio.h"

void accelInit(void);
void accelUpdateData(void);
int16_t *getAccelDataPtr(void);
int16_t *getGyroDataPtr(void);

#endif /* __ACCEL_H_ */
