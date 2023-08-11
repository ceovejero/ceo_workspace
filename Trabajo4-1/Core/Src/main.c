///* USER CODE BEGIN Header */
/**
                    *******************************************************
                    *******************************************************
                    *******************************************************
       ETH_REF_CLK--** PA 1 **                                   ** PB 0 **-- LED1[GREEN]
          ETH_MDIO--** PA 2 **        PLACA NUCLEO-F429ZI        ** PB 7 **-- LED2[BLUE]
        ETH_CRS_DV--** PA 7 **     PINES USADOS POR LA PLACA     ** PB14 **-- LED3[RED]
         ETH_TX_EN--** PB11 **                                   **********
          ETH_TXD0--** PB12 **                                   ** PC13 **-- USER_BUTTON
          ETH_TXD1--** PB13 **                                   *********
           ETH_MDC--** PC 1 **                                   ** PD 8 **-- UART3_RX
          ETH_RXD0--** PC 4 **                                   ** PD 9 **-- UART3_TX
          ETH_RXD1--** PC 5 **                                   **********
                    *******************************************************
                    **********                                   **********
        DS1307_SCL--** PB 8 **     USADOS POR LA APLICACION      ** PA15 **-- SD_SPI1_CS
        DS1307_SDA--** PB 9 **                                   ** PB 3 **-- SD_SPI1_SCK
                    **********                                   ** PB 5 **-- SD_SPI1_MOSI
                    **********                                   ** PA 6 **-- SD_SPI1_MISO
                    **********                                   **********
                    **********                                   **********
                    **********                                   **********
                    *******************************************************
                    *******************************************************
                    *******************************************************.

  */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

// definicion de tipo de datos usado para la MEF
typedef enum{
    BUTTON_DOWN,
    BUTTON_FALLING,
    BUTTON_UP,
    BUTTON_RAISING
} buttonMEF_t;



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define		Delay_1				40 //frecuencia base para leds
#define		Delay_2				500 //frecuencia base para leds
#define		Delay_3				500//frecuencia base para leds
#define		Delay_Rebote		100//frecuencia base para leds
#define     Num_Leds		    3  // secuencia de 3 LEDS
#define     Num_Secuencias		5  // en el case :3 Cambiar secuencias.

#define     Time_Debounce_UsrBtn		40  // tiempo para eliminar ruido del UserButton

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/


/* USER CODE BEGIN PV */

typedef uint32_t tick_t;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void ButtonMEF_init();		// inicializa MEF y resetea Flanco_press y Flanco_release
void ButtonMEF_update();	// lee puerto, actualiza MEF y las salidas son dos banderas Flanco_press y Flanco_release
bool_t UsrBtn_Pressed();			// la funcion devuelve estado del flag Flanco_press
bool_t UsrBtn_Released();		    // la funcion devuelve estado del flag Flanco_release

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

delay_t Timmer_UsrBtn;   	         // variable tipo timer publica para ser vista desde las funciones
buttonMEF_t EstadoActual;            // MEF-Apuntador
bool_t  Flanco_press, Flanco_release;// Banderas para mirar el estado de los eventos.

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  BSP_LED_Init(LED1);  // inicializa los puestos fisicos cableados de la board
  BSP_LED_Init(LED2);  // BSP -> Board Support Package
  BSP_LED_Init(LED3);
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO); //inicializa boton BSP en modo GPIO

  delay_t Timmer_Leds;   	// variable de tipo estructura delay_t
  SysDelayInit( & Timmer_Leds , Delay_1 );	// inicializa el timmer para el led-blink

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
   ButtonMEF_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /* USER CODE END WHILE */

	  //el led que hace Blink de 40ms, durante la ejecucion de la MEF ideantifica que no se genera ninguna
	  //demora o salto de secuencia sobre el LED, independiente del Boton.
	  // buen ejemplo apra identificar los retardos no bloqueantes y el anti-rebote
	  // Buen dato relacionado a que modularizando el codigo se hace mas simple en MAIN.

	  ButtonMEF_update();  // pregunta el button y actualiza estado de flancos

	  if (SysDelayRead(& Timmer_Leds))  //adicional a lo solicitado, solo parpadea un led.
		  BSP_LED_Toggle(LED2);

	  if (UsrBtn_Pressed())   // pregunta por un evento de presion de boton
		  BSP_LED_Toggle(LED1);

	  if (UsrBtn_Released())  // pregunta por un evento de soltado de boton
		  BSP_LED_Toggle(LED3);


  }
  /* USER CODE END 3 */
}



void ButtonMEF_init()
{
	EstadoActual = BUTTON_UP;
	Flanco_press = false;
	Flanco_release = false;
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


bool_t UsrBtn_Pressed()			// para no escribir sobre el puerto en la funcion esta pregunta por el flanco
{
bool_t resp=false;
if (Flanco_press)
	resp= true;
Flanco_press = false;
return resp;					// y devuelve el valor, al mismo tiempo que lo resetea para siguiente lectura.
}


bool_t UsrBtn_Released()		// para no escribir sobre el puerto en la funcion esta pregunta por el flanco
{
	bool_t resp=false;
	if (Flanco_release)
		resp= true;
	Flanco_release = false;
	return resp;				// y devuelve el valor, al mismo tiempo que lo resetea para siguiente lectura.

}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
