#ifndef __USART_H_
#define __USART_H_

#include "main.h"

void uartInit(void);
void uartPutToTxBuff(uint8_t *buff, uint16_t size);
void uartTask(void);
uint16_t usartGetRxLen(void);
uint8_t usartReadByte(void);

#endif /* __USART_H_ */