/*
 * API_uart.h
 *
 *  Created on: 28 jul. 2023
 *      Author: CEO
 */


#ifndef API_INC_API_UART_H_
#define API_INC_API_UART_H_

#include "API_header.h"

const bool_t uartInit();

void uartSendString(uint8_t * pstring);
void uartSendStringSize(uint8_t * pstring, uint16_t size);
void uartReceiveStringSize(uint8_t * pstring, uint16_t size);



#endif /* API_INC_API_UART_H_ */
