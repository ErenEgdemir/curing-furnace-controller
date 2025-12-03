/**
 * @file    Sensor_Handler.c
 * @brief   Sensor acquisition, SPI-based MAX6675/MAX31855 reading, and RTC data
 *          retrieval for the composite curing furnace controller.
 *
 * This module provides all sensor-handling logic required by the thermal
 * management subsystem. It performs:
 *
 *  - High-speed temperature acquisition from multiple SPI thermocouple modules
 *    using chip-select multiplexing
 *  - Interrupt-driven temperature decoding (TempSens_IRQ_Handler)
 *  - Staged sensor polling including real-time clock sampling (Sens_Handler)
 *  - Initialization of all temperature sensor chip-select lines (Temp_Sens_Init)
 *
 * **Key functionality:**
 *  - Converts 12-bit thermocouple data into signed 16-bit temperature values
 *    with scaling (TEMP_CAL)
 *  - Identifies which chip-select is active to route temperature into the
 *    correct sensor slot (sens1_temp … sens5_temp)
 *  - Ensures safe reset of all CS lines after each transaction
 *  - Polls DS1307 RTC fields (date, month, year, hour, minute) in a time-sliced
 *    sequence synchronized with the SPI state machine
 *
 * The module is designed to integrate with DMA-driven SPI reception and
 * time-based scheduling managed externally (e.g., in the TIM1/TIM9 ISR layer).
 *
 * @dependencies
 *      - STM32 HAL (SPI, GPIO)
 *      - DS1307 HAL driver
 *      - Global chip-select GPIO definitions
 *      - Sens structure from types.h
 *
 * @note All temperature-related ISRs and handlers here are designed to be fast
 *       and flag-based to minimize CPU load.
 *
 * @date    Dec 3, 2025
 * @author  Eren Egdemir
 */

#include "Sensor_Handler.h"
#include "ds1307_for_stm32_hal.h"

void TempSens_IRQ_Handler(Sens *t) {

	uint16_t t12;

	t12 = ((t->buffer[0] & 0x7FF8u) >> 3);

	if (t12 & 0x800u) {

		t->temp_f = (int16_t) (t12 | 0xF000u) * TEMP_CAL; // 12 bitten 16 bite işaret uzat

	} else {

		t->temp_f = (int16_t) t12 * TEMP_CAL;
	}

	if (HAL_GPIO_ReadPin(MAX_CS1_GPIO_Port, MAX_CS1_Pin) == GPIO_PIN_RESET) {

		t->sens1_temp = t->temp_f;

		t->temp_f = 0;

	} else if (HAL_GPIO_ReadPin(MAX_CS2_GPIO_Port, MAX_CS2_Pin)
			== GPIO_PIN_RESET) {

		t->sens2_temp = t->temp_f;

		t->temp_f = 0;

	} else if (HAL_GPIO_ReadPin(MAX_CS3_GPIO_Port, MAX_CS3_Pin)
			== GPIO_PIN_RESET) {

		t->sens3_temp = t->temp_f;
		t->temp_f = 0;

	} else if (HAL_GPIO_ReadPin(MAX_CS4_GPIO_Port, MAX_CS4_Pin)
			== GPIO_PIN_RESET) {

		t->sens4_temp = t->temp_f;
		t->temp_f = 0;

	} else if (HAL_GPIO_ReadPin(MAX_CS5_GPIO_Port, MAX_CS5_Pin)
			== GPIO_PIN_RESET) {

		t->sens5_temp = t->temp_f;
		t->temp_f = 0;

	}

	HAL_GPIO_WritePin(MAX_CS1_GPIO_Port, MAX_CS1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX_CS2_GPIO_Port, MAX_CS2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX_CS3_GPIO_Port, MAX_CS3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX_CS4_GPIO_Port, MAX_CS4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX_CS5_GPIO_Port, MAX_CS5_Pin, GPIO_PIN_SET);

}

void Sens_Handler(Sens *t, uint32_t cntr, SPI_HandleTypeDef *hspi){

	switch (cntr) {
	case 0:
		t->date = DS1307_GetDate();
		HAL_GPIO_WritePin(MAX_CS1_GPIO_Port, MAX_CS1_Pin,
				GPIO_PIN_RESET);
		HAL_SPI_Receive_DMA(hspi, (uint8_t*) t->buffer, 1);
		cntr++;
		break;
	case 1:
		t->month = DS1307_GetMonth();
		HAL_GPIO_WritePin(MAX_CS2_GPIO_Port, MAX_CS2_Pin,
				GPIO_PIN_RESET);
		HAL_SPI_Receive_DMA(hspi, (uint8_t*) t->buffer, 1);
		cntr++;
		break;
	case 2:
		t->year = DS1307_GetYear();
		HAL_GPIO_WritePin(MAX_CS3_GPIO_Port, MAX_CS3_Pin,
				GPIO_PIN_RESET);
		HAL_SPI_Receive_DMA(hspi, (uint8_t*) t->buffer, 1);
		cntr++;
		break;
	case 3:
		t->hour = DS1307_GetHour();
		HAL_GPIO_WritePin(MAX_CS4_GPIO_Port, MAX_CS4_Pin,
				GPIO_PIN_RESET);
		HAL_SPI_Receive_DMA(hspi, (uint8_t*) t->buffer, 1);
		cntr++;
		break;
	case 4:
		t->minute = DS1307_GetMinute();
		HAL_GPIO_WritePin(MAX_CS5_GPIO_Port, MAX_CS5_Pin,
				GPIO_PIN_RESET);
		HAL_SPI_Receive_DMA(hspi, (uint8_t*) t->buffer, 1);
		cntr = 0;
		break;
	default:
		break;
	}
}

void Temp_Sens_Init(void)
{
	HAL_GPIO_WritePin(MAX_CS1_GPIO_Port, MAX_CS1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX_CS2_GPIO_Port, MAX_CS2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX_CS3_GPIO_Port, MAX_CS3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX_CS4_GPIO_Port, MAX_CS4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(MAX_CS5_GPIO_Port, MAX_CS5_Pin, GPIO_PIN_SET);
}




