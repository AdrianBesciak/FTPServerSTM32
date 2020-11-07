/*
 * logger.c
 *
 *  Created on: Oct 17, 2020
 *      Author: adios
 */

#include "logger.h"

UART_HandleTypeDef *huart;

void LOGGER_Init(UART_HandleTypeDef *huart_got) {
	huart = huart_got;
}

void LOGGER_Log(char * message) {
	HAL_UART_Transmit_IT(huart, message, strlen(message));
}
