/*
 * Sensor_Handler.h
 *
 *  Created on: Nov 13, 2025
 *      Author: erenegdemir
 */

#ifndef INC_USERLIBRARIES_SENSOR_HANDLER_H_
#define INC_USERLIBRARIES_SENSOR_HANDLER_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "main.h"            // pin tanımları için (yukarıda anlatılan)
#include "common_def.h"
#include "types.h"

#define TEMP_CAL	0.25
#define TRUE		1





void TempSens_IRQ_Handler(Sens *t);
void Sens_Handler(Sens *t, uint32_t cntr, SPI_HandleTypeDef *hspi);

#endif /* INC_USERLIBRARIES_SENSOR_HANDLER_H_ */
