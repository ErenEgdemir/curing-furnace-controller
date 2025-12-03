/**
 * @file    Display_Handler.h
 * @brief   Central UI rendering and menu-screen dispatch module.
 *
 * This module provides the public interface for the oven controller's display
 * subsystem. It is responsible for routing all UI rendering operations to the
 * correct screen depending on the current menu state (MenuFlag).
 *
 * The Display_Handler() function acts as the “UI dispatcher”:
 *   - Reads the active menu state from MenuCtx (m->flag)
 *   - Calls the appropriate menu_xxx_run() routine to draw:
 *        • Home screen
 *        • Program selection menus
 *        • FastStart configuration screens
 *        • Advanced temperature control (6-stage) configuration screens
 *        • Cooking status screens (live temperature, power, duration)
 *        • Settings screens (PI parameters, max power, date/time)
 *
 * Architectural role:
 *   - The display system is intentionally stateless; it renders based on
 *     externally provided contexts:
 *        • MenuCtx   → active UI state, selected index, transitions
 *        • CookingCtx → current stage, duty cycles, remaining time
 *        • Sens       → live temperature readings
 *        • setting_t  → PI parameters and power limits
 *
 * Rendering backend:
 *   - Uses the u8g2 graphics library for drawing text and UI elements.
 *   - All screen-specific rendering is implemented in menu_xxx_run() functions.
 *   - Display_Handler() simply dispatches and keeps UI logic cleanly separated.
 *
 * Typical usage:
 *   1. Update MenuCtx using encoder handlers.
 *   2. Update CookingCtx via Cooking_Handler().
 *   3. Call Display_Handler() at 10–20 Hz to refresh the screen.
 *
 * Goals:
 *   - Clean separation of UI logic from business logic (cooking, PI control)
 *   - Extensible state-based UI design
 *   - Lightweight rendering loop without blocking operations
 *
 * @note Display_Handler() must be called periodically (e.g., in the main loop).
 * @note This header only declares the UI dispatcher; individual screens are
 *       implemented inside menu_xxx_* modules.
 *
 * @author
 *   Eren Eğdemir
 */

#ifndef INC_USERLIBRARIES_DISPLAY_HANDLER_H_
#define INC_USERLIBRARIES_DISPLAY_HANDLER_H_

#include "common_def.h"
#include "types.h"
#include "u8g2.h"
#include "menu.h"


/**
 * @brief  Main display/UI dispatcher for all menu screens.
 *
 * This function selects and runs the appropriate screen-drawing routine
 * depending on the current menu flag (m->flag). It acts as the central
 * display handler for the entire UI, routing control to individual
 * menu_xxx_run() functions.
 *
 * Behavior:
 *  - Uses m->flag (of type MenuFlag) in a switch-case.
 *  - For each screen/state, calls the corresponding menu_xxx_run() function
 *    with the required context:
 *      • HOME, PROGRAMS, SETTINGS: top-level navigation screens.
 *      • PROGRAMS_FASTSTART and *_TEMP/ *_RISETIME/ *_FALLTIME/ *_DURATION:
 *          FastStart program configuration screens.
 *      • PROGRAMS_ADVTEMPCTRL and all T1..T6 / RISINGTIME_*:
 *          Advanced 6-stage temperature profile configuration screens.
 *      • PROGRAMS_ADVTEMPCTRL_GENERATE:
 *          Summary/generation screen for the full cooking process.
 *      • COOKING, COOKING_TEMPCHANGE, COOKING_PWRCHANGE, COOKING_DURATION:
 *          Active cooking status and inline edit screens.
 *      • SETTINGS_*:
 *          PI parameter, power limit, and time/date configuration screens.
 *
 *  - Some menu_xxx_run() functions take MenuCtx and CookingCtx by value
 *    (*m, *c), others by pointer (m, c) depending on whether they need to
 *    modify the context or only display it.
 *
 * @param[in,out] d  Display context (u8g2 handle). Used by all menu_xxx_run()
 *                   functions to draw text, graphics and widgets.
 * @param[in]     m  Menu context. Its flag field determines which screen is
 *                   drawn. Some screens read additional fields (e.g. counter).
 * @param[in]     c  Cooking context. Provides current stage, remaining time,
 *                   temperatures and duty cycles for cooking-related screens.
 * @param[in]     t  Current sensor/time snapshot. Used by screens that show
 *                   real-time temperatures or time/date information.
 * @param[in]     s  Pointer to settings structure (PI gains, max power, etc.).
 *                   Used by settings and cooking screens that need to show or
 *                   edit configuration values.
 *
 * @note  This function is typically called once per main loop iteration or at
 *        a fixed refresh rate (e.g., 10–20 Hz). It should be kept free of any
 *        blocking delays; all heavy logic should reside outside of drawing
 *        routines.
 */
void Display_Handler(u8g2_t *d, MenuCtx *m, CookingCtx *c, Sens t, setting_t *s);

#endif /* INC_USERLIBRARIES_DISPLAY_HANDLER_H_ */
