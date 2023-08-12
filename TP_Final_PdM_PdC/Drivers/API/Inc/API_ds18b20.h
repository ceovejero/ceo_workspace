/*
 * API_ds18b20.h
 *
 *  Created on: Aug 10, 2023
 *      Author: CEO
 */

#ifndef API_INC_API_DS18B20_H_
#define API_INC_API_DS18B20_H_

#include <API_header.h>


void gpio_set_input (void);

void gpio_set_output (void);

uint8_t ds18b20_init (void);

void ds18b20_write_cmd (uint8_t data);

uint8_t ds18b20_read (void);

float ds18b20_read_temp(void);




#endif /* API_INC_API_DS18B20_H_ */
