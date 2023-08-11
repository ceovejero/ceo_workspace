/*
 * API_ds18b20.c
 *
 *  Created on: Aug 10, 2023
 *      Author: CEO
 */


#include "API_header.h"
#include "API_ds18b20.h"
#include "API_usDelay.h"

//extern uint8_t check =2;
//extern uint8_t  temp_l;
//extern uint8_t  temp_h;
//extern uint16_t temp;
// float temperature;

#define LOW 0				// Logic State for GPIO_PIN Communication
#define COMM_PIN GPIO_PIN_13	// select GPIO_PIN for communication according to definition in the HAL
#define COMM_PORT GPIOB			// select PORT for communication according to definition in the HAL
#define MASTER_Tx_RESET_PULSE 480    // uSeconds


GPIO_InitTypeDef GPIO_InitStruct;

void gpio_set_input (void)
{
  GPIO_InitStruct.Pin = COMM_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(COMM_PORT, &GPIO_InitStruct);
}


void gpio_set_output (void)
{
  GPIO_InitStruct.Pin = COMM_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(COMM_PORT, &GPIO_InitStruct);
}


uint8_t ds18b20_init (void)
{
	gpio_set_output ();   // set the pin as output
	HAL_GPIO_WritePin (COMM_PORT, COMM_PIN, LOW);  // pull the pin low
	DWT_Delay_us (480);   // delay according to datasheet

	gpio_set_input ();    // set the pin as input
	DWT_Delay_us (80);    // delay according to datasheet

	if (!(HAL_GPIO_ReadPin (COMM_PORT, COMM_PIN)))    // if the pin is low i.e the presence pulse is there
	{
		DWT_Delay_us (400);  // wait for 400 us
		return 0;
	}

	else
	{
		DWT_Delay_us (400);
		return 1;
	}
}

void write (uint8_t data)
{
	gpio_set_output ();   // set as output

	for (int i=0; i<8; i++)
	{

		if ((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1

			gpio_set_output ();  // set as output
			HAL_GPIO_WritePin (COMM_PORT, COMM_PIN, LOW);  // pull the pin LOW
			DWT_Delay_us (1);  // wait for  us

			gpio_set_input ();  // set as input
			DWT_Delay_us (60);  // wait for 60 us
		}

		else  // if the bit is low
		{
			// write 0

			gpio_set_output ();
			HAL_GPIO_WritePin (COMM_PORT, COMM_PIN, LOW);  // pull the pin LOW
			DWT_Delay_us (60);  // wait for 60 us

			gpio_set_input ();
		}
	}
}


uint8_t read (void)
{
	uint8_t value=0;
	gpio_set_input ();

	for (int i=0;i<8;i++)
	{
		gpio_set_output ();   // set as output

		HAL_GPIO_WritePin (COMM_PORT, COMM_PIN, LOW);  // pull the data pin LOW
		DWT_Delay_us (2);  // wait for 2 us

		gpio_set_input ();  // set as input
		if (HAL_GPIO_ReadPin (COMM_PORT, COMM_PIN))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		DWT_Delay_us (60);  // wait for 60 us
	}
	return value;
}
