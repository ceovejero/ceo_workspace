# Práctica 5 -

## Objetivo
Implementar un módulo de software sencillo para trabajar con la **UART**. 

## Punto 1
Implementar un módulo de software en un archivos fuente `API_uart.c` con su correspondiente archivo de cabecera `API_uart.h` y ubicarlos en el proyecto dentro de  las carpetas `/drivers/API/src` y `/drivers/API/inc`, respectivamente.

En `API_uart.h` se deben ubicar los prototipos de las funciones públicas.

```c
bool_t uartInit();
void uartSendString(uint8_t * pstring);
void uartSendStringSize(uint8_t * pstring, uint16_t size);
void uartReceiveStringSize(uint8_t * pstring, uint16_t size);

```

En `API_uart.c` se deben ubicar los prototipos de las funciones privadas y la implementación de todas las funciones de módulo, privadas y públicas.

### Consideraciones para la implementación:

1.	`uartInit()` debe realizar toda la inicialización de la **UART**.  Adicionalmente, debe imprimir por la terminal serie un mensaje con sus parámetros de configuración.
	La función devuelve:
	- `true`: si la inicialización es exitosa.
	- `false`: si la inicialización no es exitosa.
2.	`uartSendString(uint8_t *pstring)` recibe un puntero a un string que se desea enviar por la **UART** completo (hasta el caracter ‘\0’) y debe utilizar la función de la HAL `HAL_UART_Transmit(...)` para transmitir el string.
3.	`uartSendStringSize(uint8_t * pstring, uint16_t size)` recibe un puntero a un string que se desea enviar por la UART y un entero con la cantidad de caracteres que debe enviar. La función debe utilizar `HAL_UART_Transmit(...)` para transmitir el string.


## Punto 2

Sobre el ejercicio de la `práctica 4`, implementar un programa que utilice el módulo de uart para informar al usuario cuando se producen los flancos ascendente y descendente.
