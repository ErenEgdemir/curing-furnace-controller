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
#include "common_def.h"
#include "types.h"

#define PI_DT	0.1f
#define TRUE	1

static inline float temp_drift(float sens, float avg)
{
	float x = avg - sens;

	return x;
}

static inline void check_error(CookingCtx *c, Sens t){

	uint8_t comp_temp = 0;
	uint8_t furnice_temp = 0;

	comp_temp = (t.sens1_temp + t.sens2_temp + t.sens3_temp) / 3;
	furnice_temp = (t.sens4_temp + t.sens5_temp) / 2;

	if(temp_drift(t.sens1_temp, comp_temp) > 5 || temp_drift(t.sens1_temp, comp_temp) < -5){
		c->flag = ABORT;
	}else if(temp_drift(t.sens2_temp, comp_temp) > 5 || temp_drift(t.sens2_temp, comp_temp) < -5){
		c->flag = ABORT;
	}else if(temp_drift(t.sens3_temp, comp_temp) > 5 || temp_drift(t.sens3_temp, comp_temp) < -5){
		c->flag = ABORT;
	}

	if(temp_drift(t.sens4_temp, furnice_temp) > 10 || temp_drift(t.sens4_temp, furnice_temp) < -10){
			c->flag = ABORT;
	}else if(temp_drift(t.sens4_temp, furnice_temp) > 10 || temp_drift(t.sens4_temp, furnice_temp)< -10){
		c->flag = ABORT;
	}

	if(t.sens1_temp > 250 || t.sens2_temp > 250 || t.sens3_temp > 250 || t.sens4_temp > 250 || t.sens5_temp > 250){
		c->flag = ABORT;
	}else if(t.sens1_temp == 0 || t.sens2_temp == 0 || t.sens3_temp == 0 || t.sens4_temp == 0 || t.sens5_temp == 0){
		c->flag = ABORT;c->flag = ABORT;
	}

}


void update_DutyCycle(CookingCtx *c, TIM_HandleTypeDef *htim);

void Cooking_Handler(CookingCtx *c, Sens t, PI_Oven *pi1, PI_Oven *pi2, PI_Oven *pi3);


#endif /* INC_USERLIBRARIES_COOKING_HANDLER_H_ */
