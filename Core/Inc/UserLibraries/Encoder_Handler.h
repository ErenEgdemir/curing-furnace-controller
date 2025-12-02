/*
 * Button_Handler.h
 *
 *  Created on: Nov 12, 2025
 *      Author: erenegdemir
 */

#ifndef INC_USERLIBRARIES_ENCODER_HANDLER_H_
#define INC_USERLIBRARIES_ENCODER_HANDLER_H_

#include <stdint.h>
#include "settings.h"
#include "ds1307_for_stm32_hal.h"
#include "pi_control.h"
#include "common_def.h"
#include "types.h"


extern AppSettings* cfg;
extern setting_t setting;

static inline void display_goto(MenuCtx *m, MenuFlag f)
{
	m->flag = f;
	m->counter = 0;
}

static inline void display_set(MenuCtx *m, MenuFlag f, uint32_t c){
	m->flag = f;
	m->counter = c;
}

static inline void set_value(encoder *enc, uint32_t *var,
		uint32_t min_value, uint32_t max_value) {

	if (enc->position > enc->old_position) {

		if (*var < max_value) {
			(*var)++;
		}

	} else if (enc->position < enc->old_position) {
		if (*var > min_value) {
			(*var)--;
		}

	}

	enc->old_position = enc->position;
}

void button_handler_encoder1(MenuCtx *m, CookingCtx *c, Sens t);
void button_handler_encoder2(MenuCtx *m, CookingCtx *c);
void button_handler_encoder3(MenuCtx *m, setting_t *s, PI_Oven *p1, PI_Oven *p2, PI_Oven *p3);

void tick_handler_encoder1(encoder *e, MenuCtx *m);
void tick_handler_encoder2(encoder *e, MenuCtx *m, CookingCtx c);
void tick_handler_encoder3(encoder *e, MenuCtx *m);


#endif /* INC_USERLIBRARIES_ENCODER_HANDLER_H_ */
