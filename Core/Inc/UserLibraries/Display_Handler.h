/*
 * Display_Handler.h
 *
 *  Created on: Dec 2, 2025
 *      Author: erenegdemir
 */

#ifndef INC_USERLIBRARIES_DISPLAY_HANDLER_H_
#define INC_USERLIBRARIES_DISPLAY_HANDLER_H_

#include "common_def.h"
#include "types.h"
#include "u8g2.h"
#include "menu.h"

void Display_Handler(u8g2_t *d, MenuCtx *m, CookingCtx *c, Sens t, setting_t *s);

#endif /* INC_USERLIBRARIES_DISPLAY_HANDLER_H_ */
