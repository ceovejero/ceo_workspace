/*
 * API_FSM.c
 *
 *  Created on: Aug 11, 2023
 *      Author: CEO
 */


#include "APP_FSM.h"
#include "API_delay.h"
#include "API_debounce.h"
#include "API_uart.h"
#include "API_LCD_I2C.h"
#include "API_usDelay.h"
#include "API_ds18b20.h"


#define INIT_APP_TIME 2000
#define TRUE 1
#define FALSE 0

#define PERIOD_1 100
#define PERIOD_2 800



typedef enum{
	 	 	 	 	INIT_APP_STATE,
					DISPLAY_APP_STATE,
					GRAPH_APP_STATE,

			} appState_t;

static delay_t delayInitApp; //Estructura para control de tiempos
static appState_t actualAppState; // Variable de estado (global) interna de Maquina de Estados
static bool_t displayInit_flag = TRUE;
static float tempC;
static float prev_tempC;
static char lcdString[20]=" ";
static char uartString[50]=" ";
static char initMessageRow1[] = "TP Final";
static char initMessageRow2[] = "PdM - PdC";

//static bool_t periodFlag = false;   //Bandera para conmutar entre DOS valores de Periodos
static delay_t ledTimed;				//Estructura de temporizacion
static bool_t  inicioUart;
static uint8_t prevState = INIT_APP_STATE;


void APP_FSM_init()
{
		actualAppState = INIT_APP_STATE;
		//BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);
		delayInit(&delayInitApp, INIT_APP_TIME);  //control de tiempos para debounce
		BSP_LED_Init(LED2);			// Se nicializa el LED2 onboard a traves de funciones de BSP
		debounceFSM_init();			// Se inicializa la maquina de estados del antirrebote de pulsador
		delayInit(&ledTimed, PERIOD_1);		// Se inicializa la estructura de temporizacion

		  lcd_begin ();
		  lcd_init ();
		  delayUS_DWT_Init();

		  inicioUart = uartInit();			//inicializacion de la uart y captura de inicializacion correcta/incorrecta
		  if (!inicioUart)
		  {
			  ErrorHandler();			// en caso de inicializacion incorrecta se llava a la funcion de reporte de error
		  }


}
void APP_FSM_update()
{
	switch (actualAppState)
			{
				case INIT_APP_STATE:
						if(displayInit_flag)
							{
								 	 //sprintf(lcdString, "TP Final");
									 lcd_clear();
									 lcd_put_cur(0, 0);
									 lcd_send_string (initMessageRow1);

									 //sprintf(lcdString, "PdM - PdC");
									 lcd_put_cur(1, 0);
									 lcd_send_string (initMessageRow2);
									 while(!delayRead(&delayInitApp))
									 {}

								sprintf(uartString, "%s\n\r %s\n\r",initMessageRow1,initMessageRow2);
								uartSendString((uint8_t *) uartString);

								displayInit_flag = FALSE;
							}
						if (delayRead(&delayInitApp))
							{
								actualAppState = DISPLAY_APP_STATE; //estado siguiente
							}
				break;
				case DISPLAY_APP_STATE:
						debounceFSM_update();
						tempC = ds18b20_read_temp();
						if ((prev_tempC != tempC)||(prevState!=DISPLAY_APP_STATE))
						 	 {
								prev_tempC = tempC;
								lcd_clear();
								lcd_put_cur(0, 0);
								lcd_send_string ("Visualizando:");
								lcd_put_cur(1, 1);
								sprintf(lcdString, "Temp: %3.3f C",tempC);
								lcd_send_string (lcdString);
								prevState = DISPLAY_APP_STATE;
						 	 }
						sprintf(uartString, "Temp: %3.2f C\n\r",tempC);
						uartSendString((uint8_t *) uartString);

						if (readKey())  // control de tiempo
							{// padado el tiempo de
								actualAppState = GRAPH_APP_STATE;

							}
							//else // si se detecta estado inestable del pulsador
							//{
							//	actualState = BUTTON_UP;   //regreso al estado previo
							//}
				break;
				case GRAPH_APP_STATE:
						debounceFSM_update();
						tempC = ds18b20_read_temp();
						if ((prev_tempC != tempC)||(prevState!=GRAPH_APP_STATE))
							{
								prev_tempC = tempC;
								lcd_clear();
								lcd_put_cur(0, 0);
								lcd_send_string ("Tx Data 4 Graph:");
								lcd_put_cur(1, 0);
								sprintf(lcdString, "Temp: %3.3f C",tempC);
								lcd_send_string (lcdString);
								prevState = GRAPH_APP_STATE;
							}
						sprintf(uartString, "%3.2f\n",tempC);
						uartSendString((uint8_t *) uartString);

						if (readKey())  // control de tiempo
							{// padado el tiempo de
								actualAppState = DISPLAY_APP_STATE;

							}
				break;
				default:
						// si se carga algun valor no contemplado, se regresa al estado inicial
						actualAppState = INIT_APP_STATE;;
				break;
			}

}

