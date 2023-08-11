/*
 * API_uart.c
 *
 *  Created on: 28 jul. 2023
 *      Author: CEO
 */

#include "API_header.h"
#include "API_uart.h"

#define UART_INIT_OK "\r\nInicializacion correcta\r\n"
#define UART_INIT_FAIL "Fallo de inicializacion\r\n"
#define INIT_MSG "Comunic UART 9600, 8N1\r\n HWC_OFF, Tx-Rx, OS_16\r\n"
#define SIZE_ONE 1

// from Carmine Noviello pag 230
static UART_HandleTypeDef uartHandle;			// declaracion de la estructura de configuracion de UART
static uint32_t uartDelay = HAL_MAX_DELAY;		// definicion del timeout de uart en su valor maximo
static bool_t uartInitStatus = false;
//static char pstring[100]={0};


/**
 * @brief  Inicializa la interfaz de comunicaion UART
 * @param  None
 * @retval Booleano true = inicializacion OK
 */
const bool_t uartInit()			// inicializacion de UART
{
	// estructura de inicializacion de UART
	/*
	 * Instancia= USART3
	 * BaudRate= 9600
	 * WordLengh= 8 bit (7 bit  datos + 1 bit paridad)
	 * StopBit= 1 stop bit
	 * Parity= No Paridad
	 * Mode= TX_RX
	 * WardWareControl= HW flow OFF
	 * OverSampling= 16
	 * */
	uartHandle.Instance = USART3;
	uartHandle.Init.BaudRate = 9600;
	uartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	uartHandle.Init.StopBits = UART_STOPBITS_1;
	uartHandle.Init.Parity = UART_PARITY_NONE;
	uartHandle.Init.Mode = UART_MODE_TX_RX;
	uartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

	if (HAL_UART_Init(&uartHandle) != HAL_OK)			// Inicializacio nde UART
		{
			uartSendString((uint8_t *) UART_INIT_FAIL);		// en caso de error de inicializacion se envia mensaje de error
			uartInitStatus = false;
		}
	else
		{
			uartSendString((uint8_t *) UART_INIT_OK);		// en caso de inicializacion Correcta se envia mensaje OK
			uartSendString((uint8_t *) INIT_MSG);
			uartInitStatus = true;
		}

	return uartInitStatus;
}


/**
 * @brief  Envia una cadena de caracteres por UART
 * @param  puntero a cadena
 * @retval None
 */
void uartSendString(uint8_t * pstring)
{
	assert(pstring!=0);
	uint16_t len = 0;
	while (*(pstring+len) != '\0')
	{
		HAL_UART_Transmit(&uartHandle, (pstring+len), (uint16_t)SIZE_ONE, uartDelay);
		len++;
	}

}

/**
 * @brief  Envia una cadena de caracteres por UART
 * @param  puntero a cadena
 * @param  tamanio de cadena
 * @retval None
 */
void uartSendStringSize(uint8_t * pstring, uint16_t size)
{
	HAL_UART_Transmit(&uartHandle, pstring, size, uartDelay);

	for (uint16_t len = 0; len <= size; len++)
	{
		HAL_UART_Transmit(&uartHandle, (pstring+len), (uint16_t)SIZE_ONE, uartDelay);
		len++;
	}

}


/**
 * @brief  Recibe una cadena de caracteres por UART
 * @param  puntero a varaible tipo cadena para almacenar los datos
 * @param  tamanio de cadena
 * @retval None
 */
void uartReceiveStringSize(uint8_t * pstring, uint16_t size)
{
	HAL_UART_Receive(&uartHandle, pstring, size, uartDelay);
}
