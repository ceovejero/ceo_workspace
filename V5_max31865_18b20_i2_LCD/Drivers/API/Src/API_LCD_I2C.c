/*
 * API_LCD_I2C.c
 *
 *  Created on: 7 ago. 2023
 *      Author: CEO
 */

#include "API_header.h"
#include "API_LCD_I2C.h"

I2C_HandleTypeDef i2c1Handle;


#define SLAVE_ADDRESS_LCD 0x4E // change this according to ur setup

#define I2C_DATA_SIZE 4
#define I2C_TIMEOUT 100


/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
void lcd_begin (void)
{
		i2c1Handle.Instance = I2C1;
		i2c1Handle.Init.ClockSpeed = 100000;
		i2c1Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
		i2c1Handle.Init.OwnAddress1 = 0;
		i2c1Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		i2c1Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		i2c1Handle.Init.OwnAddress2 = 0;
		i2c1Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		i2c1Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
		if (HAL_I2C_Init(&i2c1Handle) != HAL_OK)
		{
			assert(!HAL_OK);
		}
}



void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit (&i2c1Handle, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, I2C_DATA_SIZE, I2C_TIMEOUT);
}

void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=1
	data_t[1] = data_u|0x09;  //en=0, rs=1
	data_t[2] = data_l|0x0D;  //en=1, rs=1
	data_t[3] = data_l|0x09;  //en=0, rs=1
	HAL_I2C_Master_Transmit (&i2c1Handle, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, I2C_DATA_SIZE, I2C_TIMEOUT);
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);

}

void lcd_clear (void)
{
	lcd_send_cmd (0x80);
	for (int i=0; i<70; i++)
	{
		lcd_send_data (' ');
	}
}

void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;	// 0xC0 result: (0x40|0x80)
            break;
        case 2:
            col |= 0x94;	// 0x94 result: (0x14|0x80)
            break;
        case 3:
            col |= 0xD4;	// 0xD4 result: (0x54|0x80)
            break;
        default:
        	col |= 0x80;
    }

    lcd_send_cmd (col);
}

void lcd_init (void)
{
	// 4 bit initialisation
	HAL_Delay(50);  // wait for >40ms
	lcd_send_cmd (0x30);
	HAL_Delay(5);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	HAL_Delay(1);  // wait for >100us
	lcd_send_cmd (0x30);
	HAL_Delay(10);
	lcd_send_cmd (0x20);  // 4bit mode
	HAL_Delay(10);

  // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}



