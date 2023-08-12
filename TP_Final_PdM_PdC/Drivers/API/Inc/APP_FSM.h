/*
 * API_FSM.h
 *
 *  Created on: Aug 11, 2023
 *      Author: CEO
 */

#ifndef API_INC_APP_FSM_H_
#define API_INC_APP_FSM_H_

#include "API_header.h"


void APP_FSM_init();		// debe cargar el estado inicial
void APP_FSM_update();		// debe leer las entradas, resolver la logica de
							// transicion de estados y actualizar las salidas

#endif /* API_INC_APP_FSM_H_ */
