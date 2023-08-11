/*
 * API_debounce.c
 *
 *  Created on: 25 jul. 2023
 *      Author: CEO
 */

#include "API_header.h"
#include "API_delay.h"
#include "API_debounce.h"
#include "API_uart.h"

#define DEBOUNCE_PERIOD 40
#define UART_BUTTON_PRESSED "\r\nUser Button Falling edge - Flanco Descendente\r\n"
#define UART_BUTTON_RELEASED "\r\nUser Button Raising edge - Flanco Ascendente\r\n"

 typedef enum{
					BUTTON_UP,
					BUTTON_FALLING,
					BUTTON_DOWN,
					BUTTON_RAISING,
				} debounceState_t;

static delay_t delayDebounceFSM; //Estructura para control de tiempos
static debounceState_t actualState; // Variable de estado (global) interna de Maquina de Estados
static bool_t pressed;

/**
 * @brief  Inicializa la Maquina de Estado - debounce
 * @param  None
 * @retval None
 */
void debounceFSM_init()
{
	actualState = BUTTON_UP;
		BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);
		delayInit(&delayDebounceFSM, DEBOUNCE_PERIOD);  //control de tiempos para debounce
}

/**
 * @brief  Implementacion de la maquia de estado.
 * @param  None
 * @retval None
 */
void debounceFSM_update()
{
	switch (actualState)
		{
			case BUTTON_UP:
					if (BSP_PB_GetState(BUTTON_USER))
						{
						actualState = BUTTON_FALLING; //estado siguiente
							delayRead(&delayDebounceFSM);  //inicia la cuenta de antirrebote
						}
			break;
			case BUTTON_FALLING:
					if (delayRead(&delayDebounceFSM))  // control de tiempo de antirrebote
					{// padado el tiempo de antirebote se controla el estado del pulsador
						if (BSP_PB_GetState(BUTTON_USER))
						{	// si sigue presionado se pasa al sigte estado
							actualState = BUTTON_DOWN;
							pressed = true;	// estado presionado
							buttonPressed();	//accion para estado presionado
						}
						else // si se detecta estado inestable del pulsador
						{
							actualState = BUTTON_UP;   //regreso al estado previo
						}
					}
			break;
			case BUTTON_DOWN:
					if (!BSP_PB_GetState(BUTTON_USER))
						{	// si el pulsador se libera se regresa al estado anterior
						actualState = BUTTON_RAISING;
							delayRead(&delayDebounceFSM);
						}
			break;
			case BUTTON_RAISING:
					if (delayRead(&delayDebounceFSM))   // control de tiempo de antirrebote
					{	// padado el tiempo de antirebote se controla el estado del pulsador
						if(!BSP_PB_GetState(BUTTON_USER))
						{	//  si se libera se pasa al sigte estado
							actualState = BUTTON_UP;
							pressed = false;;   // NO presionado
							buttonReleased();   //accion para estado NO presionado
						}
						else   // si se detecta estado inestable del pulsador
						{
							actualState = BUTTON_DOWN;  //regreso al estado previo
						}
					}
			break;
			default:
					// si se carga algun valor no contemplado, se regresa al estado inicial
					actualState = BUTTON_UP;
			break;
		}
}

/**
* @brief  boton presionado
* @param  None
* @retval true si se presiono el pulsador
*/
const bool_t readKey()
{
	bool_t flag = false;
	if (pressed)
		{
			flag = true;
			pressed = false;
		}
	return flag;
}

/**
* @brief  Accion para boton presionado - invierte el estado del LED1
* @param  None
* @retval None
*/
void buttonPressed()
{
	//uartSendString((uint8_t *) UART_BUTTON_PRESSED);
}


 /**
 * @brief  Accion para boton liberado - invierte el estado del LED3
 * @param  None
 * @retval None
 */
void buttonReleased()
{
	//uartSendString((uint8_t *) UART_BUTTON_RELEASED);
}
