/*
 * logger.h
 *
 *  Created on: Oct 17, 2020
 *      Author: adios
 */

#ifndef INC_LOGGER_H_
#define INC_LOGGER_H_

#include "main.h"
#include "stm32f7xx_hal.h"
#include <string.h>
#include <stdio.h>

void LOGGER_Init(UART_HandleTypeDef *huart_got);
void LOGGER_Log(char * message);

#endif /* INC_LOGGER_H_ */
