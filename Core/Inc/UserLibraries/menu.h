/*
 * menu.h
 *
 *  Created on: Oct 16, 2025
 *      Author: erenegdemir
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

#include "u8g2.h"
#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include "common_def.h"
#include "types.h"


extern uint8_t generate_flag;

extern float Loading_Coefficent;
extern uint32_t total_duration;
extern uint32_t Loading;
extern uint32_t Loading_P;



void menu_send_time(u8g2_t *display, uint8_t x, uint8_t y, int hh, int mm);
void menu_send_date(u8g2_t *display, uint8_t x, uint8_t y, int dd, int mm, int yy);
void menu_send_logo(u8g2_t *display, uint8_t x, uint8_t y);
void menu_send_int_1(u8g2_t *display, uint8_t x, uint8_t y, int val);
void menu_send_int_2(u8g2_t *display, uint8_t x, uint8_t y, int val);
void menu_send_int_3(u8g2_t *display, uint8_t x, uint8_t y, int val);
void menu_send_int_4(u8g2_t *display, uint8_t x, uint8_t y, int val);
void menu_send_fan(u8g2_t *display, uint8_t x, uint8_t y);
void menu_send_flame(u8g2_t *display, uint8_t x, uint8_t y);
void menu_send_setupLogo(u8g2_t *display, uint8_t x, uint8_t y);


void menu_home_run(u8g2_t *display, MenuCtx m, Sens t);


void menu_programs_run(u8g2_t *display, MenuCtx m);

void menu_prg_AdvTempCtrl_run(u8g2_t *display, MenuCtx m, CookingCtx c);

void menu_prg_AdvTempCtrl_RisingTimes_run(u8g2_t *display, MenuCtx *m, CookingCtx c, Sens t);
void menu_prg_AdvTempCtrl_RisingTimes_st1_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_RisingTimes_st2_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_RisingTimes_st3_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_RisingTimes_st4_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_RisingTimes_st5_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_RisingTimes_st6_run(u8g2_t *display, MenuCtx m, CookingCtx c);

void menu_GenerateCookingProcess(u8g2_t *display, MenuCtx *m, CookingCtx *c, Sens t);

void menu_cooking_run(u8g2_t *display, CookingCtx *c, Sens t, setting_t setting);
void menu_cooking_run_TempChange(u8g2_t *display, MenuCtx m, CookingCtx *c, Sens t, setting_t setting);
void menu_cooking_PowerChange_run(u8g2_t *display, MenuCtx m, CookingCtx *c, Sens t);
void menu_cooking_run_Duration(u8g2_t *display, MenuCtx m, CookingCtx *c, Sens t, setting_t setting);


void menu_prg_FastStart_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_FastStart_TrgtTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_FastStart_RiseTime_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_FastStart_FallTime_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_FastStart_Duration_run(u8g2_t *display, MenuCtx m, CookingCtx c);

void menu_prg_AdvTempCtrl_STG1_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG2_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG3_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG4_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG5_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG6_run(u8g2_t *display, MenuCtx m, CookingCtx c);

void menu_prg_AdvTempCtrl_STG1_SetTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG2_SetTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG3_SetTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG4_SetTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG5_SetTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG6_SetTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c);

void menu_prg_AdvTempCtrl_STG1_SetDur_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG2_SetDur_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG3_SetDur_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG4_SetDur_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG5_SetDur_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG6_SetDur_run(u8g2_t *display, MenuCtx m, CookingCtx c);

void menu_prg_AdvTempCtrl_STG1_SetReset_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG2_SetReset_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG3_SetReset_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG4_SetReset_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG5_SetReset_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_STG6_SetReset_run(u8g2_t *display, MenuCtx m, CookingCtx c);


void menu_settings_run(u8g2_t *display, MenuCtx m);

//void menu_settings_TempOffset_run(u8g2_t *display, uint32_t menu_counter, setting_t *s);


void menu_settings_Pi_run(u8g2_t *display, MenuCtx m, setting_t *s);
void menu_settings_PiKp_run(u8g2_t *display, MenuCtx m, setting_t *s);
void menu_settings_PiKi_run(u8g2_t *display, MenuCtx m, setting_t *s);
void menu_settings_PiSlew_run(u8g2_t *display, MenuCtx m, setting_t *s);
void menu_settings_PiMaxPower_run(u8g2_t *display, MenuCtx m, setting_t *s);

void menu_settings_TimeDate_run(u8g2_t *display, MenuCtx m, Sens t);
void menu_settings_TimeDateHour_run(u8g2_t *display, MenuCtx m, Sens t);
void menu_settings_TimeDateMin_run(u8g2_t *display, MenuCtx m, Sens t);
void menu_settings_TimeDateDay_run(u8g2_t *display, MenuCtx m, Sens t);
void menu_settings_TimeDateMonth_run(u8g2_t *display, MenuCtx m, Sens t);
void menu_settings_TimeDateYear_run(u8g2_t *display, MenuCtx m, Sens t);





#endif /* INC_MENU_H_ */
