/*
 * API_debounce.c
 *
 *  Created on: 13/07/2023
 *      Author: joseluis
 */
// El codigo utiliza el boton includo en la placa.
// Crea un MEF que es transparente para el usuario.
// un tiempo Time_Debounce_UsrBtn que define el filtro del anti-rebote
// para el usuario solo se accede a:
// ButtonMEF_init();	inicializa y configura el button
// ButtonMEF_update();	dentro de bucle infito, lee el boton y actualiza banderas
// UsrBtn_ReadKey();  con el valor de una bandera interna, da respues  de si fue presionado o no el boton




#include "API_debounce.h"
#include "API_delay.h"
#include "stm32f429ZI.h"

// un poco enrredado con el tema de los include.
// que va y donde va, o porque!

typedef enum{
    BUTTON_DOWN,
    BUTTON_FALLING,
    BUTTON_UP,
    BUTTON_RAISING
} buttonMEF_t;



#define     Time_Debounce_UsrBtn		40  // tiempo para eliminar ruido del UserButton


delay_t Timmer_UsrBtn;   	         // variable tipo timer publica para ser vista desde las funciones
static buttonMEF_t EstadoActual;            // MEF-Apuntador
static bool_t  Flanco_press, Flanco_release;// Banderas para mirar el estado de los eventos.
// dudas de cual es la razon del Static


void ButtonMEF_init()
{
	EstadoActual = BUTTON_UP;
	Flanco_press = false;
	Flanco_release = false;
	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO); //inicializa boton BSP en modo GPIO

}

void ButtonMEF_update()
{
	switch (EstadoActual)
	{
		case BUTTON_UP :
		{
			if (BSP_PB_GetState(BUTTON_USER))   //identifica que el boton se presiono, activa delay
			{
				EstadoActual = BUTTON_FALLING;
				SysDelayInit( & Timmer_UsrBtn, Time_Debounce_UsrBtn );
			}
			break;
		}
		case BUTTON_FALLING :
		{
			  if (SysDelayRead(& Timmer_UsrBtn))
			  {
				  if(BSP_PB_GetState(BUTTON_USER))  // al finalizar retardo re-confirma que el boton fue presionado
				  {
					  EstadoActual = BUTTON_DOWN;  // si se mantiene activa varible Flanco_press
					  Flanco_press= true;
				  }
				  else
					  EstadoActual = BUTTON_UP;	// ruido detectado, resetea a esperar pulsacion
			  }
			  break;

		}
		case BUTTON_DOWN :
		{
			if (!BSP_PB_GetState(BUTTON_USER))   // boton esperando que sea soltado
			{
				EstadoActual = BUTTON_RAISING;           // al soltar inicia contador para eliminar falso positivo
				SysDelayInit( & Timmer_UsrBtn, Time_Debounce_UsrBtn );
			}
			break;

		}
		case BUTTON_RAISING :
		{
			  if (SysDelayRead(& Timmer_UsrBtn))
			  {
				  if(!BSP_PB_GetState(BUTTON_USER))   //despues de retardo re-confirma boton suelto
				  {
					  EstadoActual = BUTTON_UP;  // si se mantiene suelto a activa Flanco_release
					  Flanco_release= true;
				  }
				  else
					  EstadoActual = BUTTON_DOWN;	// ruido detectado, resetea a esperar soltado
			  }
			  break;
		}
		default :
			EstadoActual = BUTTON_UP;   // asegura que va a caer en el primer estado que inicia MEF
		break;
	}

}

bool_t UsrBtn_ReadKey()
{
	bool_t resp=false;
	if (Flanco_press)
		resp= true;
	Flanco_press = false;
	return resp;					// y devuelve el valor, al mismo tiempo que lo resetea para siguiente lectura.

}


/* funcion implementada solo para mirar los flancos
bool_t UsrBtn_Pressed()			// para no escribir sobre el puerto en la funcion esta pregunta por el flanco
{
bool_t resp=false;
if (Flanco_press)
	resp= true;
Flanco_press = false;
return resp;					// y devuelve el valor, al mismo tiempo que lo resetea para siguiente lectura.
}
*/


/* funcion implementada solo para mirar los flancos
bool_t UsrBtn_Released()		// para no escribir sobre el puerto en la funcion esta pregunta por el flanco
{
	bool_t resp=false;
	if (Flanco_release)
		resp= true;
	Flanco_release = false;
	return resp;				// y devuelve el valor, al mismo tiempo que lo resetea para siguiente lectura.

}
 */
