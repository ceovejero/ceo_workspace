/*
 * API_debounce.h
 *
 *  Created on: 25 jul. 2023
 *      Author: CEO
 */

#ifndef API_INC_API_DEBOUNCE_H_
#define API_INC_API_DEBOUNCE_H_

#include "API_header.h"


void debounceFSM_init();		// debe cargar el estado inicial
void debounceFSM_update();		// debe leer las entradas, resolver la logica de
															// transicion de estados y actualizar las salidas
void buttonPressed();			// debe invertir el estado del LED1
void buttonReleased();			// debe invertir el estado del LED3

void buttonPressed();	//accion para estado presionado
void buttonReleased();   //accion para estado NO presionado

/* La función readKey debe leer una variable interna del módulo y devolver true o false si la tecla fue presionada.
 * Si devuelve true, debe resetear (poner en false) el estado de la variable.*/
const bool_t readKey();


#endif /* API_INC_API_DEBOUNCE_H_ */
