/*
 * Cooking_Handler.h
 *
 *  Created on: Nov 12, 2025
 *      Author: erenegdemir
 */

#ifndef INC_USERLIBRARIES_COOKING_HANDLER_H_
#define INC_USERLIBRARIES_COOKING_HANDLER_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "pi_control.h"
#include "Sensor_Handler.h"

#define PI_DT	0.1f
#define TRUE	1
#define FALSE	0


typedef enum
{
	PENDING = 0,
	STAGE_1,
	STAGE_2,
	STAGE_3,
	STAGE_4,
	STAGE_5,
	STAGE_6,
	FASTSTART,
	ABORT,
}CookingFlag;


typedef struct {
	uint16_t temperature;
	uint16_t duration;
	uint8_t durationPX;
	uint8_t state;
	uint16_t changePeriod;
	uint8_t changePeriodPX;
	uint8_t fallTime;
	float realTimeTemp;
	float tempForMin[512];
	float tempPerMin;
	float tempPerMinDown;
	uint32_t remTime;
	uint16_t totalDuration;

}stage_t;

typedef struct
{
	CookingFlag flag;
	uint32_t counter;
	stage_t *stage1;
	stage_t *stage2;
	stage_t *stage3;
	stage_t *stage4;
	stage_t *stage5;
	stage_t *stage6;
	stage_t *faststart;
	uint8_t r1_duty;
	uint8_t r2_duty;
	uint8_t r3_duty;
	uint8_t duty_mean;
	uint8_t generated;

}CookingCtx;

void update_DutyCycle(CookingCtx *c, TIM_HandleTypeDef *htim);

void cooking_handler(CookingCtx *c, Sens t);


#endif /* INC_USERLIBRARIES_COOKING_HANDLER_H_ */
