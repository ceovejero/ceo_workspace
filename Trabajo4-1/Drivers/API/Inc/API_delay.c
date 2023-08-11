/*
 * API_delay.c
 *
 *  Created on: 7/07/2023
 *      Author: joseluis
 */

//inicializa el temporizador.

#include "API_delay.h"
#include "stm32f4xx_hal.h"
#include "stm32f429ZI.h"

#define		MaxDuration				60000 //limite de conteo en 1 hora


void SysDelayInit( delay_t * Time_Var, tick_t Duration)
{
    if (Time_Var == NULL)
    	SysDelayError_Handler();
    else
    {
    	if(0 <= Duration && MaxDuration >= Duration ) // asegura que el timer no supere una hora.
        {
    		Time_Var->Duration = Duration;
    		Time_Var->StartTime= HAL_GetTick();		// obtiene el valor para
    		Time_Var->Running  = true;		        // inicializa apagado...
        }
        else
        	SysDelayError_Handler();
    }
	return;
}

//Funcion que pregunta si completo el tiempo
// si completa el tiempo y se pregunta por su estado se recarga y vuelve a iniciar.
// true -> si completa el tiempo   false-> mientras espera
bool_t SysDelayRead ( delay_t * Time_Var)
{
bool Respuesta=false;
	if (Time_Var == NULL)  // asegura que la variable Time_Var no este vacia.
	    SysDelayError_Handler();
	else
	{
		if (Time_Var->Running)  // entra mientras este contando tiempos.
		{
			if(HAL_GetTick() - Time_Var->StartTime >= Time_Var->Duration)
			{
				Time_Var->Running = false;  //apaga despues de cumplir el tiempo.
				Respuesta = true;
			}
			else
			{
				Respuesta = false;
			}
		}
		else   // resetea el contador y lo reinicia, una vez sea consultado.
		{
			Respuesta = false;
			Time_Var->StartTime = HAL_GetTick();
			Time_Var->Running = true;
		}
	}

	return (Respuesta);
}


//Funcion para cambiar el temporizador
void SysDelayWrite( delay_t * Time_Var, tick_t Duration)
{
	if (Time_Var == NULL)  // asegura que la variable Time_Var no este vacia.
	    SysDelayError_Handler();
	else
	{
		if(0 <= Duration && MaxDuration >= Duration ) // asegura que el timer no supere una hora.
			Time_Var->Duration = Duration;
		else
			SysDelayError_Handler();
	}

	return;

}


void SysDelayError_Handler (void)
{
	  __disable_irq();
	  while (1)
	  {
		  BSP_LED_Init(LED1);  // inicializa los puestos fisicos cableados de la board
	  }

}


