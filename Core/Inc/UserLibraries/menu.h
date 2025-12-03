/**
 * @file    menu.h
 * @brief   UI rendering and menu-state interface for the curing furnace controller.
 *
 * This header declares all high-level menu rendering functions used by the
 * display subsystem. Each function corresponds to a different screen or UI
 * state in the menu-state machine, including:
 *
 *  - Home and program-selection screens
 *  - FastStart configuration screens
 *  - Advanced temperature profile configuration screens (Stage 1–6)
 *  - Cooking runtime screens (temperature, power, duration adjustments)
 *  - System settings screens (PI controller tuning, time/date setup)
 *
 * The functions in this module are responsible only for drawing and updating
 * UI content on the ST7920-based 128×64 LCD via U8g2.
 * No business logic, PI control, or safety logic is implemented here.
 *
 * This file groups UI functions according to their functional category using
 * Doxygen @defgroup tags, providing a clear structure for documentation and
 * maintenance.
 *
 * @date    Dec 3, 2025
 * @author  Eren Egdemir
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

/**
 * @defgroup MENU_DRAW_PRIMITIVES  Menu Drawing Primitives
 * @brief   Low-level drawing helpers for text, icons and static UI elements.
 *
 * This group contains the foundational UI drawing functions used by all menu
 * pages. They provide simple abstractions over U8g2 DrawStr, icon rendering,
 * numeric field drawing, and layout positioning.
 *
 * @{
 */

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

/** @} */


/**
 * @defgroup MENU_MAIN_SCREENS  Main Menu and Core UI Screens
 * @brief   High-level render functions for main, home, cooking, and program menus.
 *
 * These functions correspond to the top-level UI screens presented to the user.
 * Each function fully renders a complete page based on current system context.
 *
 * @{
 */

void menu_home_run(u8g2_t *display, MenuCtx m, Sens t);
void menu_programs_run(u8g2_t *display, MenuCtx m);
void menu_prg_AdvTempCtrl_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_settings_run(u8g2_t *display, MenuCtx m);

/** @} */


/**
 * @defgroup MENU_FASTSTART_SCREENS  FastStart Program Screens
 * @brief   UI pages related to the FastStart cooking mode.
 *
 * This includes target temperature selection, rise/fall time selection, and
 * duration configuration for the FastStart profile.
 *
 * @{
 */

void menu_prg_FastStart_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_FastStart_TrgtTemp_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_FastStart_RiseTime_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_FastStart_FallTime_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_FastStart_Duration_run(u8g2_t *display, MenuCtx m, CookingCtx c);

/** @} */


/**
 * @defgroup MENU_ADVTEMPCTRL_SCREENS  Advanced Temp Control Screens
 * @brief   Stage-based multi-profile cooking configuration UI.
 *
 * This group includes all screens related to stage-based temperature control,
 * including:
 *  - Stage pages (STG1–STG6)
 *  - Stage temperature set pages
 *  - Stage duration set pages
 *  - Stage enable/reset pages
 *  - Rising time configuration screens
 *
 * @{
 */

void menu_prg_AdvTempCtrl_RisingTimes_run(u8g2_t *display, MenuCtx *m, CookingCtx c, Sens t);
void menu_prg_AdvTempCtrl_RisingTimes_st1_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_RisingTimes_st2_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_RisingTimes_st3_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_RisingTimes_st4_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_RisingTimes_st5_run(u8g2_t *display, MenuCtx m, CookingCtx c);
void menu_prg_AdvTempCtrl_RisingTimes_st6_run(u8g2_t *display, MenuCtx m, CookingCtx c);

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

/** @} */

/**
 * @defgroup MENU_COOKING_SCREENS  Cooking runtime and process screens
 * @brief   Screens related to live cooking process, profile generation, and
 *          on-the-fly parameter adjustments.
 *
 * This group contains all menu pages that are shown while a cooking process
 * is running or being prepared. It includes:
 *
 *  - Automatic generation of stage-based temperature profiles
 *  - Live cooking screen with current stage, temperature and timers
 *  - On-the-fly adjustment pages for:
 *      * Target temperature
 *      * Maximum power
 *      * Remaining duration
 *
 * All functions in this group render complete pages using the current
 * CookingCtx, MenuCtx, and sensor readings.
 *
 * @{
 */
void menu_GenerateCookingProcess(u8g2_t *display, MenuCtx *m, CookingCtx *c, Sens t);
void menu_cooking_run(u8g2_t *display, CookingCtx *c, Sens t, setting_t setting);
void menu_cooking_run_TempChange(u8g2_t *display, MenuCtx m, CookingCtx *c, Sens t, setting_t setting);
void menu_cooking_PowerChange_run(u8g2_t *display, MenuCtx m, CookingCtx *c, Sens t);
void menu_cooking_run_Duration(u8g2_t *display, MenuCtx m, CookingCtx *c, Sens t, setting_t setting);

/** @} */

/**
 * @defgroup MENU_SETTINGS_SCREENS  Settings Menu Screens
 * @brief   All UI screens related to system configuration and user settings.
 *
 * This group includes subpages for:
 *  - PI controller tuning (KP, KI, slew rate, max power)
 *  - Time/date configuration (RTC)
 *
 * These screens allow the user to configure core system parameters through
 * the rotary encoder interface.
 *
 * @{
 */
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

/** @} */











#endif /* INC_MENU_H_ */
