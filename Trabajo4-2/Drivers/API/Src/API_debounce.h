/*
 * API_debounce.h
 *
 *  Created on: 13/07/2023
 *      Author: joseluis
 */

#ifndef API_SRC_API_DEBOUNCE_H_
#define API_SRC_API_DEBOUNCE_H_

#include "stdbool.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"

// un poco enrredado con el tema de los include.
// que va y donde va, o porque!

typedef bool 		bool_t;
// ya estaba definda, pero me toco volver a definir bool_t, asi esta bien?


void ButtonMEF_init();		// inicializa MEF y resetea Flanco_press y Flanco_release
void ButtonMEF_update();	// lee puerto, actualiza MEF y las salidas son dos banderas Flanco_press y Flanco_release
bool_t UsrBtn_ReadKey();			// la funcion devuelve estado del flag Flanco pressed


#endif /* API_SRC_API_DEBOUNCE_H_ */
