/*
 * types.h
 *
 *  Created on: Dec 2, 2025
 *      Author: erenegdemir
 */

#ifndef INC_USERLIBRARIES_TYPES_H_
#define INC_USERLIBRARIES_TYPES_H_
#include <stdint.h>
#include "common_def.h"

typedef struct
{
	float kp;
	float ki;
	float slew;
	float max_pwr;

	int8_t t1_offset;
	int8_t t2_offset;
	int8_t t3_offset;
	int8_t t4_offset;
	int8_t t5_offset;

}setting_t;

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

typedef struct {
	MenuFlag flag;	//menu flag
	uint32_t counter;	//menu_counter
}MenuCtx;

typedef struct
{
	volatile uint32_t counter;
	volatile uint32_t position;
	volatile uint32_t old_position;
	volatile uint32_t temp;
	volatile uint32_t duration;
	volatile uint32_t max_power;
	volatile uint32_t menu;


} encoder;

#endif /* INC_USERLIBRARIES_TYPES_H_ */
