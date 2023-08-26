/*
 * MAX31865_lib.c
 *
 *
 *  Cálculo de temperatura de PT100, dependiendo de la resistencia: aquí habrá un enlace a mi cálculo.
 *  MAX31865 datasheet: https://datasheets.maximintegrated.com/en/ds/MAX31865.pdf
 */

#include "MAX31865_lib.h"

/* Private SPI Handler */
//static SPI_HandleTypeDef *hspi;

/* Private GPIO CS Pin Variables */
static GPIO_TypeDef *CS_GPIO_Port;
static uint16_t CS_Pin;

/* SPI Transmission Data */
//static uint8_t SPITransmitData;


/*--------------Características de un sensor PT100 y una resistencia de referencia conectada a MAX31865------------*/
#define MAX31865_PT100_R0 (double)100.0 //Resistencia del sensor PT100, a 0 °С
#define MAX31865_Rref (double)428.5 //Resistencia de referencia conectada a MAX31865
/*--------------Características de un sensor PT100 y una resistencia de referencia conectada a MAX31865------------*/

/*-----------Coeficientes de GOST 6651-2009 para el sensor tipo PT100 (Platinum TS Y CHE, 0.00385°С^-1)---------*/
#define MAX31865_A (double)0.0039083
#define MAX31865_B (double)0.0000005775
/*-----------Coeficientes de GOST 6651-2009 para el sensor tipo PT100 (Platinum TS Y CHE, 0.00385°С^-1)---------*/

/*-----------------------------------------Variables globales---------------------------------------------*/
double MAX31865_PT100_R = 0.0; //Variable global que define la resistencia del sensor PT100
double MAX31865_PT100_T = 0.0; //Variable global que define la temperatura del sensor PT100
bool MAX31865_Sensor_Error = 0; //Variable global para falla del sensor PT100
/*-----------------------------------------Variables globales---------------------------------------------*/

/*-------------------------------------------Para trabajar en spi-----------------------------------------------*/
#define cs_set() CS_GPIO_Port-> BSRR = (uint32_t) CS_Pin << 16u; //CS_to low level
#define cs_reset() CS_GPIO_Port ->BSRR = CS_Pin; //CS to high level -  3.3v
extern SPI_HandleTypeDef hspi1;  //hspi1
//P.S. La velocidad máxima de spi es de 5 MHz.
//También tenga en cuenta que Clock Polarity(CLPOL) = Low. Clock Phase(CPHA) = 2 Edge.
/*-------------------------------------------Para trabajar en spi-----------------------------------------------*/

/*=======================INICIALIZACIÓN DEL MÓDULO MAX31865=========================*/
void MAX31865_Init(uint8_t num_wires) {
	///Función de inicialización del módulo MAX31865
	///No veo mucho sentido en mostrar la configuración completa del módulo, así que hagamos
	///ligera simplificación para el usuario final
	///lo único que el usuario puede configurar es seleccionar el tipo de conexión
	///2, 3 o 4 hilos
	/// \param num_wires - tipo de conexión del sensor 2,3 o 4 hilos
	uint8_t MAX31865_Reinitialization_cnt = 0;
	MAX31865_Sensor_Error = 0;
	uint8_t MAX31865_Configuration_register_write[] = { 0x80, 0x00 };
	if (num_wires == 2 || num_wires == 4) {
		MAX31865_Configuration_register_write[1] = 0xC3; //0xC3
	} else if (num_wires == 3) {
		MAX31865_Configuration_register_write[1] = 0xD3; //0xD3
	}
	//cs_set();
	HAL_SPI_Transmit(&hspi1, MAX31865_Configuration_register_write, 2, 100);
	//cs_reset();
	//Para alcanzar el sensor después de aplicar energía, tk. la primera vez que la inicialización puede no pasar, comencemos un ciclo.
	while (MAX31865_Configuration_info() != 0xD1 || MAX31865_Configuration_info() != 0xC1) {
		MAX31865_Reinitialization_cnt++;

		//cs_set();
		HAL_SPI_Transmit(&hspi1, MAX31865_Configuration_register_write, 2, 100);
		//cs_reset();

		if (MAX31865_Reinitialization_cnt == 100) {
			//printf("Initialization MAX31865 != OK\r\n");
			break;
		}

	}

}
/*=======================INICIALIZACIÓN DEL MÓDULO MAX31865=========================*/

/*====================INFORMACIÓN DE CONFIGURACIÓN DEL MÓDULO MAX31865===================*/
uint8_t MAX31865_Configuration_info(void) {
	///Función de recuperación de información de configuración del módulo MAX31865
	///Devuelve un valor de configuración.
	///No se sorprenda si envía 0xC3 en la inicialización y obtiene 0xC1
	///(Consulte la hoja de datos MAX31865 p.14 - "The fault status clear bit D1, self-clears to 0.")
	uint8_t read_data = 0x00;
	uint8_t MAX31865_Configuration = 0x00;
	//cs_set();
	HAL_SPI_Transmit(&hspi1, &read_data, 1, 100);
	HAL_SPI_Receive(&hspi1, &MAX31865_Configuration, 1, 100);
	//cs_reset();
	return MAX31865_Configuration;
}
/*====================INFORMACIÓN DE CONFIGURACIÓN DEL MÓDULO MAX31865===================*/

/*===================FUNCIÓN PRINCIPAL DE TRABAJAR CON EL MÓDULO MAX31865==================*/
double MAX31865_Get_Temperature(void) {
	///La función principal de trabajar con el módulo MAX31865
	///Se accede a la dirección inicial del registro de memoria del módulo y se leen 7 bytes.
	///La función también incluye el autodiagnóstico del módulo, que le indicará si hay algún problema con el sensor.

	double data; //variable de calculo

	struct rx_data_MAX31865 {
		uint16_t RTD_Resistance_Registers; //Registros de resistencia
		uint16_t High_Fault_Threshold; //Umbral de falla superior
		uint16_t Low_Fault_Threshold; //Umbral de fallo inferior
		uint8_t Fault_Status; //Estado de falla
	};

	struct rx_data_MAX31865 MAX31865_receieve_data;

	uint8_t MAX31865_start_address_of_the_poll = 0x01; //Dirección de registro desde la que empezar a leer datos
	uint8_t MAX31865_rx_buffer[7]; //búfer donde agregaremos los datos entrantes
	//cs_set();
	HAL_SPI_Transmit(&hspi1, &MAX31865_start_address_of_the_poll, 1, 100);
	HAL_SPI_Receive(&hspi1, MAX31865_rx_buffer, 7, 100);
	//cs_reset();
	MAX31865_receieve_data.RTD_Resistance_Registers = ((MAX31865_rx_buffer[0] << 8) | MAX31865_rx_buffer[1]) >> 1; //Datos de registro de resistencia
	MAX31865_receieve_data.High_Fault_Threshold = ((MAX31865_rx_buffer[2] << 8) | MAX31865_rx_buffer[3]) >> 1; //Datos de Umbral de fallo alto
	MAX31865_receieve_data.Low_Fault_Threshold = (MAX31865_rx_buffer[4] << 8) | MAX31865_rx_buffer[5]; //Datos de Umbral de fallo bajo
	MAX31865_receieve_data.Fault_Status = MAX31865_rx_buffer[6]; //Estado de falla
	if (MAX31865_receieve_data.Fault_Status > 0x00) {

		/*--------------Estas son sus acciones para responder a un error del sensor---------------*/
		MAX31865_Sensor_Error = 1;
		//printf("Sensor Error!\r\n");

		/*----Restablecimiento automático de errores----*/
		MAX31865_Init(3);
		MAX31865_Sensor_Error = 0;
		/*----Restablecimiento automático de errores----*/

		//Entonces puede restablecer el error reiniciando el sensor.
		//Restablezca el error, si lo desea. Por lo general, no se reinicia en la máquina, pero se llama al operador para reconocer el error..
		//Antes de la llegada del operador, la instalación tiene un error, todos los nodos de control deben estar apagados.
		/*--------------Estas son sus acciones para responder a un error del sensor---------------*/

	}

	//printf("RTD Resistance Registers = %X\r\n", MAX31865_receieve_data.RTD_Resistance_Registers);
	//printf("Hight Fault Treshold = %X\r\n", MAX31865_receieve_data.High_Fault_Threshold);
	//printf("Low Fault Treshold = %X\r\n", MAX31865_receieve_data.Low_Fault_Threshold);
	//printf("Fault status = %X\r\n", MAX31865_receieve_data.Fault_Status = MAX31865_rx_buffer[6]);
	data = ((double) MAX31865_receieve_data.RTD_Resistance_Registers * MAX31865_Rref ) / (double) 32768.0; // Replace 4000 by 400 for PT100
	//printf("Rrtd = %lf\n", data);
	return MAX31865_Get_Temperature_math(data);
}
/*===================FUNCIÓN PRINCIPAL DE TRABAJAR CON EL MÓDULO MAX31865==================*/

/*===============================================CONVERSIÓN DE RESISTENCIA A TEMPERATURA, SEGÚN GOST 6651-2009===============================================*/
double MAX31865_Get_Temperature_math(double PT100_Resistance) {
	if (PT100_Resistance >= (double) 100.0) {
		double MAX31865_math_Discriminant = (double) 0.00001527480889 - ((double) -0.00000231 * (1 - (PT100_Resistance / MAX31865_PT100_R0 )));
		MAX31865_PT100_T = ((double) -0.0039083 + sqrt(MAX31865_math_Discriminant)) / (double) -0.000001155;
	} else {
		MAX31865_PT100_T = (double) 0.000000000270 * pow(PT100_Resistance, 5) - (double) 0.000000066245 * pow(PT100_Resistance, 4) - (double) 0.000000184636 * pow(PT100_Resistance, 3)
				+ (double) 0.002320232987 * pow(PT100_Resistance, 2) + (double) 2.229927824035 * PT100_Resistance - (double) 242.090854986215;
	}
	return MAX31865_PT100_T;
}
/*===============================================CONVERSIÓN DE RESISTENCIA A TEMPERATURA, SEGÚN GOST 6651-2009===============================================*/
