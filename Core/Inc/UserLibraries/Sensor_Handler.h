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

#define TEMP_CAL	0.25
#define TRUE		1
#define FALSE 		2

typedef struct
{
	uint16_t buffer[1];
	float temp_f;
	float sens1_temp;
	float sens2_temp;
	float sens3_temp;
	float sens4_temp;
	float sens5_temp;
	uint8_t date;
	uint8_t month;
	uint32_t year;
	uint8_t dow;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t zone_hr;
	uint8_t zone_min;
}Sens;


void TempSens_IRQ_Handler(Sens *t, SPI_HandleTypeDef *hspi);
void Sens_Handler(Sens *t, uint32_t cntr, SPI_HandleTypeDef *hspi);

#endif /* INC_USERLIBRARIES_SENSOR_HANDLER_H_ */
