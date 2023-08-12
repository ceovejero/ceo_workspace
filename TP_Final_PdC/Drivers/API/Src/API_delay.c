/*
 * API_delay.c
 *
 *  Created on: Jul 10, 2023
 *      Author: ceo
 */
#include "API_header.h"
#include "API_delay.h"

/* ========== Non-bloquing delay functions ========== */

/**
 * @brief  Inicializa la estructura de temporizacion
 * @param  puntero a estructura
 * @param  durecion de la temporizacion
 * @retval None
 */
void delayInit(delay_t *delay, tick_t duration)
{
	if(delay == NULL || duration <= 0)   //control - Si el puntero de parametro es nulo o duracion menor/igual a cero
	{
		ErrorHandler(); //Funcion de error
	}

	delay->duration = duration;
	delay->running = false;
}

/**
 * @brief  devuelve fin  de temporizacion
 * @param  puntero a estructura
 * @retval Booleano
 */

bool_t delayRead(delay_t *delay)
{
	if(delay == NULL)  //control - Si el puntero de parametro es nulo
		{
			ErrorHandler();  //Funcion de error
		}
	if (delay->running == false)
	{
		delay->running = true;
		delay->startTime = HAL_GetTick();
		return false;
	}
	else
	{
		if ((HAL_GetTick() - delay->startTime) < delay->duration)
		{
			return false;
		}
		else
		{
			delay->running = false;
			return true;
		}
	}
}

/**
 * @brief  Escribe nuevo valor de  temporizacion
 * @param  puntero a estructura
 * @param  valor de temporizacion
 * @retval void
 */
void delayWrite(delay_t *delay, tick_t duration)
{
	if(delay == NULL || duration <= 0)   //control - Si el puntero de parametro es nulo o duracion menor/igual a cero
		{
			ErrorHandler(); //Funcion de error
		}
	delay->duration = duration;
}

/**
 * @brief  Funcion de Error
 * @param  Ninguno
 * @retval void
 */
void ErrorHandler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  /* Si un error ocurre el USER LED RED permanece encendido*/
	HAL_GPIO_WritePin(GPIOB, LED3_PIN, GPIO_PIN_SET);
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

/* ============================================================ */
