/*
 * API_LCD_I2C.h
 *
 *  Created on: 7 ago. 2023
 *      Author: CEO
 */

#ifndef API_INC_API_LCD_I2C_H_
#define API_INC_API_LCD_I2C_H_

#include "API_header.h"

void lcd_begin (void);

void lcd_send_cmd (char cmd);

void lcd_init (void);

void lcd_send_data (char data);

void lcd_send_string (char *str);

void lcd_clear (void);

void lcd_put_cur(int row, int col);



#endif /* API_INC_API_LCD_I2C_H_ */
