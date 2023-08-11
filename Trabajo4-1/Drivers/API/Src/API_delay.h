/*
 * API_delay.h
 *
 *  Created on: 7/07/2023
 *      Author: joseluis
 */

#include "stdbool.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"


typedef uint32_t 	tick_t;  //AL_GetTick() devuelve un uint32_t, pero se define un tipo tick_t que solo manejara valores relacionados a tiempo.
typedef bool 		bool_t;


typedef struct {
	tick_t		Duration;   //el tipo tick_t es un uint32_t
	tick_t  	StartTime;  // usar un typedef para estas variables facilita la definicion de variables en las funciones
    bool_t      Running;    // el mismo caso para la variable bool.
}delay_t;



void SysDelayInit( delay_t * Time_Var, tick_t Duration); //inicializa el contador pero no arranca
bool_t SysDelayRead ( delay_t * Time_Var);
void SysDelayWrite( delay_t * Time_Var, tick_t Duration);   // funcion para activar/desativar AutoStart;
void SysDelayError_Handler (void);
