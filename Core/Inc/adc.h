#ifndef __ADC_H_
#define __ADC_H_

#include <stdint.h>
#include "stm32f103xb.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_gpio.h"

uint16_t *getAdcDataPtr(void);
void adcUpdateData(void);
void adcInit(void);

#endif /* __ADC_H_ */
