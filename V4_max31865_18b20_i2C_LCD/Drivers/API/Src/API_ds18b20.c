/*
 * API_ds18b20.c
 *
 *  Created on: Aug 10, 2023
 *      Author: CEO
 */


#include "API_header.h"
#include "API_ds18b20.h"
#include "API_usDelay.h"
#include "API_delay.h"

//extern uint8_t check =2;
//extern uint8_t  temp_l;
//extern uint8_t  temp_h;
//extern uint16_t temp;
// float temperature;

#define LOW 0								// Logic State for GPIO_PIN Communication
#define COMM_PIN GPIO_PIN_13				// select GPIO_PIN for communication according to definition in the HAL
#define COMM_PORT GPIOB						// select PORT for communication according to definition in the HAL
#define MASTER_Tx_RESET_PULSE 480			// uSeconds
#define MASTER_WAIT_FOR_PRESENCE 80			// uSeconds
#define MASTER_WAIT_FOR_END_PRESENCE_PULSE 400	// 400 uS

#define MASTER_Rx_TIME 480			// uSeconds

#define SENSOR_PERIOD 800

GPIO_InitTypeDef GPIO_InitStruct;

static delay_t sensorTimed;

static bool_t ds18b20_cmd_flag=0;
//static uint8_t check=2;
static uint8_t  temp_l;
static uint8_t  temp_h;
static uint16_t temp;
static float temperature=0;




float ds18b20_read_temp(void)
{
	uint8_t check = 2;
	check = ds18b20_init ();

	if(ds18b20_cmd_flag == 0)
	{
		delayInit(&sensorTimed, SENSOR_PERIOD);
		ds18b20_write_cmd (0xCC);  // skip ROM
		ds18b20_write_cmd (0x44);  // convert t
		ds18b20_cmd_flag = 1;
	}
	else if(delayRead(&sensorTimed))   //HAL_Delay (800);
			{
				ds18b20_init ();
				ds18b20_write_cmd (0xCC);  // skip ROM
				ds18b20_write_cmd (0xBE);  // Read Scratchpad

				temp_l = ds18b20_read();
				temp_h = ds18b20_read();
				temp = (temp_h<<8)|temp_l;
				temperature = (float)temp/16;

				ds18b20_cmd_flag = 0;
			}

	if(check == 0)
		{
			return (temperature);
		}
		else
		{
			return (temperature);
		}
}



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
	delayUS_DWT (MASTER_Tx_RESET_PULSE);   // 480 uS delay according to datasheet

	gpio_set_input ();    // set the pin as input
	delayUS_DWT (MASTER_WAIT_FOR_PRESENCE);    // 80 uS delay, FOR pRESENCE, according to datasheet

	if (!(HAL_GPIO_ReadPin (COMM_PORT, COMM_PIN)))    // if the pin is low i.e the presence pulse is there
	{
		delayUS_DWT (MASTER_WAIT_FOR_END_PRESENCE_PULSE);  // wait for 400 us
		return 0;
	}

	else
	{
		delayUS_DWT (MASTER_WAIT_FOR_END_PRESENCE_PULSE);
		return 1;
	}
}

void ds18b20_write_cmd (uint8_t data)
{
	gpio_set_output ();   // set as output

	for (int i=0; i<8; i++)
	{

		if ((data & (1<<i))!=0)  // if the bit is high
		{
			// write 1

			gpio_set_output ();  // set as output
			HAL_GPIO_WritePin (COMM_PORT, COMM_PIN, LOW);  // pull the pin LOW
			delayUS_DWT (1);  // wait for  us

			gpio_set_input ();  // set as input
			delayUS_DWT (60);  // wait for 60 us
		}

		else  // if the bit is low
		{
			// write 0

			gpio_set_output ();
			HAL_GPIO_WritePin (COMM_PORT, COMM_PIN, LOW);  // pull the pin LOW
			delayUS_DWT (60);  // wait for 60 us

			gpio_set_input ();
		}
	}
}


uint8_t ds18b20_read (void)
{
	uint8_t value=0;
	gpio_set_input ();

	for (int i=0;i<8;i++)
	{
		gpio_set_output ();   // set as output

		HAL_GPIO_WritePin (COMM_PORT, COMM_PIN, LOW);  // pull the data pin LOW
		delayUS_DWT (2);  // wait for 2 us

		gpio_set_input ();  // set as input
		if (HAL_GPIO_ReadPin (COMM_PORT, COMM_PIN))  // if the pin is HIGH
		{
			value |= 1<<i;  // read = 1
		}
		delayUS_DWT (60);  // wait for 60 us
	}
	return value;
}



