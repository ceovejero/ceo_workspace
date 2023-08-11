/*
 * API_delay.h
 *
 *  Created on: Jul 10, 2023
 *      Author: ceo
 */

#ifndef API_INC_API_DELAY_H_
#define API_INC_API_DELAY_H_

#include "API_header.h"

// typeder movidos a API_header.h
//typedef uint32_t tick_t;
//typedef bool bool_t;

/* == struct for Non-bloquing delay == */
typedef struct{
   tick_t startTime;
   tick_t duration;
   bool_t running;
} delay_t;
/* ============================================= */

/* ========== Non-bloquing delay functions ========== */
void delayInit( delay_t * delay, tick_t duration );
bool_t delayRead( delay_t * delay );
void delayWrite( delay_t * delay, tick_t duration );
/* ========== ============================ ========== */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void ErrorHandler(void);

#endif /* API_INC_API_DELAY_H_ */
