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


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define		Delay_1				40 //frecuencia base para leds
#define		Delay_2				100 //frecuencia base para leds
#define		Delay_3				500//frecuencia base para leds
#define		Delay_Rebote		100//frecuencia base para leds
#define     Num_Leds		    3  // secuencia de 3 LEDS
#define     Num_Secuencias		5  // en el case :3 Cambiar secuencias.


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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
//  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO); es inicializada en Button_MEF, lo usa "API_debounce"


  delay_t Timmer_Led1, Timmer_Led2;   	// variable de tipo estructura delay_t
  SysDelayInit( & Timmer_Led1 , Delay_1 );	// inicializa el timmer para el led-blink independiente
  SysDelayInit( & Timmer_Led2 , Delay_2 );	// inicializa el timmer para el led de ejercicio

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* USER CODE BEGIN 2 */
   ButtonMEF_init();   //inicializa MEF y tambien el Button_User
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  bool_t salto=false;  // para hacer el salto entre frecuencias del led.

  while (1)
  {
    /* USER CODE END WHILE */

	  //el led que hace Blink de 40ms, durante la ejecucion de la MEF ideantifica que no se genera ninguna
	  //demora o salto de secuencia sobre el LED, independiente del Boton.
	  // buen ejemplo para identificar los retardos no bloqueantes y el anti-rebote
	  // Buen dato relacionado a que modularizando el codigo se hace mas simple en MAIN.

	  ButtonMEF_update();  // pregunta el button y actualiza estado de flancos

	  if (SysDelayRead(& Timmer_Led1))  //adicional a lo solicitado, solo parpadea un led.
		  BSP_LED_Toggle(LED1);


	  if (SysDelayRead(& Timmer_Led2))  //adicional a lo solicitado, solo parpadea un led.
		  BSP_LED_Toggle(LED2);
	  if (UsrBtn_ReadKey())   // pregunta por un evento de presion de boton
	  {
		if(salto== true)
		{
			salto=false;
			SysDelayWrite(&Timmer_Led2,Delay_2);
		}
		else
		{
			salto=true;
			SysDelayWrite(&Timmer_Led2,Delay_3);

		}
	  }

//	  if (UsrBtn_Released())  // pregunta por un evento de soltado de boton
//		  BSP_LED_Toggle(LED3);


  }
  /* USER CODE END 3 */
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
