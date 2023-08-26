/*
 * Simple microseconds delay routine, utilizing ARM's DWT
 * (Data Watchpoint and Trace Unit) and HAL library.
 *
 * This file is part of DWT_Delay package.
 * DWT_Delay is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * Based on Mastering "Mastering STM32 – Carmine Noviello"
 *
 *
 */

#include "API_header.h"          // change to whatever MCU you use
#include "API_usDelay.h"

/**
 * Initialization routine.
 * You might need to enable access to DWT registers on Cortex-M7
 *   DWT->LAR = 0xC5ACCE55
 */
void delayUS_DWT_Init(void)
{
    if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk))
    {
        // (DEMCR) Debug Exception and Monitor Control Register
    	// (TRCENA = tracing enable) This enables control of power usage unless tracing is required.
    	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;  // reset the counter
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // DWT_CTRL_CYCCNT ENA_Msk equals: enable the counter
    }
}


/**
 * Delay routine.
 * Time is in microseconds (1/1000000th of a second), not to be
 * confused with millisecond (1/1000th).
 *
 * No need to check an overflow. Let it just tick :)
 *
 * @param uint32_t us  Number of microseconds to delay for
 */
void delayUS_DWT(uint32_t us) // microseconds
{
    uint32_t startTick = DWT->CYCCNT,
             delayTicks = us * (SystemCoreClock/1000000);

    while (DWT->CYCCNT - startTick < delayTicks);
}

