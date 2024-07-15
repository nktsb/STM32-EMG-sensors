#ifndef __EMUL_H_
#define __EMUL_H_

#include <stdint.h>
#include "stm32f103xb.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_gpio.h"

void emulSensorsData(void);
uint16_t *getEmgEmulPtr(void);
int16_t *getAccelEmulPtr(void);
int16_t *getGyroEmulPtr(void);

#endif /* __EMUL_H_ */
