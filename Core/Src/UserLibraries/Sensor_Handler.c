/*
 * Sensor_Handler.c
 *
 *  Created on: Nov 13, 2025
 *      Author: erenegdemir
 */

#include "Sensor_Handler.h"
#include "ds1307_for_stm32_hal.h"

void TempSens_IRQ_Handler(Sens *t, SPI_HandleTypeDef *hspi){

	if (hspi->Instance == SPI1) {

		uint16_t t12;

		t12 = ((t->buffer[0] & 0x7FF8u) >> 3);

		if (t12 & 0x800u) {

			t->temp_f = (int16_t) (t12 | 0xF000u) * TEMP_CAL; // 12 bitten 16 bite işaret uzat

		} else {

			t->temp_f = (int16_t) t12 * TEMP_CAL;
		}

		if (HAL_GPIO_ReadPin(MAX_CS1_GPIO_Port, MAX_CS1_Pin)
				== GPIO_PIN_RESET) {

			t->sens1_temp = t->temp_f;

			t->temp_f = 0;

		} else if (HAL_GPIO_ReadPin(MAX_CS2_GPIO_Port, MAX_CS2_Pin)
				== GPIO_PIN_RESET) {

			t->sens2_temp = t->temp_f;

			t->temp_f = 0;

		}else if (HAL_GPIO_ReadPin(MAX_CS3_GPIO_Port, MAX_CS3_Pin)
				== GPIO_PIN_RESET) {

			t->sens3_temp = t->temp_f;
			t->temp_f = 0;

		}else if (HAL_GPIO_ReadPin(MAX_CS4_GPIO_Port, MAX_CS4_Pin)
				== GPIO_PIN_RESET) {


			t->sens4_temp = t->temp_f;
			t->temp_f = 0;


		}else if (HAL_GPIO_ReadPin(MAX_CS5_GPIO_Port, MAX_CS5_Pin)
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
}

void Sens_Handler(Sens *t, uint32_t cntr, SPI_HandleTypeDef *hspi){

	switch (cntr) {
	case 0:
		t->date = DS1307_GetDate();
		HAL_GPIO_WritePin(MAX_CS1_GPIO_Port, MAX_CS1_Pin,
				GPIO_PIN_RESET);
		HAL_SPI_Receive_DMA(hspi, (uint8_t*) t->buffer, 1);
//				HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
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




