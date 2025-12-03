/**
 * @file    Encoder_Handler.h
 * @brief   High-level rotary encoder interface for UI navigation and value editing.
 *
 * This module provides an abstraction layer for handling multiple rotary encoders
 * used in the system's user interface. It contains:
 *
 *  - **Encoder initialization**
 *      Starts TIM2, TIM3, and TIM4 in hardware quadrature mode with interrupts enabled.
 *
 *  - **Utility helpers**
 *      - set_value()        : Bounded increment/decrement based on encoder movement.
 *      - display_goto()     : Switches to a new menu state and resets its counter.
 *      - display_set()      : Switches to a new menu state with a specified counter value.
 *
 *  - **Tick handlers (rotation)**
 *      Used to update counters and values when the encoder is rotated.
 *      - tick_handler_encoder1()
 *      - tick_handler_encoder2()
 *      - tick_handler_encoder3()
 *
 *  - **Button handlers (press events)**
 *      Used to confirm selections or transition between UI states.
 *      - button_handler_encoder1()
 *      - button_handler_encoder2()
 *      - button_handler_encoder3()
 *
 * # Purpose
 * The encoder system serves as the main human-machine interface (HMI) mechanism for:
 *  - Navigating multi-level menus,
 *  - Editing advanced temperature programs,
 *  - Modifying real-time cooking parameters,
 *  - Adjusting PI controller settings,
 *  - Editing time/date values,
 *  - Generating and confirming cooking profiles.
 *
 * # Design Notes
 *  - All functions are lightweight and safe for use inside interrupts or
 *    high-frequency scheduler loops.
 *  - No dynamic memory allocation or blocking calls are used.
 *  - The module does not directly interact with heater hardware; it only updates
 *    UI-related state and configuration.
 *
 * # Dependencies
 *  - STM32 HAL TIM Encoder driver
 *  - MenuCtx, CookingCtx, and other shared context structures
 *  - Common type definitions
 *
 * @date    12 Nov 2025
 * @author  eren
 */

#ifndef INC_USERLIBRARIES_ENCODER_HANDLER_H_
#define INC_USERLIBRARIES_ENCODER_HANDLER_H_

#include <stdint.h>
#include "settings.h"
#include "ds1307_for_stm32_hal.h"
#include "pi_control.h"
#include "common_def.h"
#include "types.h"
#include "stm32f4xx_hal_tim.h"

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;



extern AppSettings* cfg;
extern setting_t setting;

/**
 * @brief Switches the menu context to a new state and resets its counter.
 *
 * This helper function updates the menu context by assigning a new menu flag
 * (@p f) and resetting the menu counter to zero. It is typically used when
 * navigating to a new screen where no prior selection or cursor index should be
 * retained.
 *
 * Behavior:
 *  - m->flag    ← f
 *  - m->counter ← 0
 *
 * Typical Use Cases:
 *  - Moving to a new menu page
 *  - Resetting cursor/index when entering a submenu
 *  - Ensuring a clean state transition without preserving old values
 *
 * @param[in,out] m   Pointer to the current menu context.
 *                    Both `flag` and `counter` fields are modified.
 *
 * @param[in]     f   New menu flag representing the target menu page/state.
 *
 * @note Implemented as `static inline` for speed, since it is commonly used in
 *       UI navigation handlers and is performance-critical.
 */
static inline void display_goto(MenuCtx *m, MenuFlag f)
{
	m->flag = f;
	m->counter = 0;
}

/**
 * @brief Sets the active menu state and its associated counter value.
 *
 * This helper function updates the menu context by assigning a new menu flag
 * (@p f) and initializing the menu counter (@p c). It is typically used during
 * menu navigation to switch screens or reset selections when entering a new
 * state.
 *
 * Behavior:
 *  - m->flag    ← f
 *  - m->counter ← c
 *
 * Typical Use Cases:
 *  - Navigating between menu pages
 *  - Resetting selection indices when entering a submenu
 *  - Jumping programmatically to a specific menu state
 *
 * @param[in,out] m   Pointer to the current menu context structure.
 *                    Both `flag` and `counter` fields are modified.
 *
 * @param[in]     f   Target menu state to transition into (enum MenuFlag).
 *
 * @param[in]     c   Initial counter value for the new menu state.
 *
 * @note Function is declared `static inline` to allow efficient use inside
 *       frequently called UI and navigation handlers.
 */
static inline void display_set(MenuCtx *m, MenuFlag f, uint32_t c){
	m->flag = f;
	m->counter = c;
}

/**
 * @brief Updates a variable based on encoder movement within given bounds.
 *
 * This inline helper function adjusts a target variable (@p var) according to
 * the movement of a rotary encoder. It compares the encoder's current position
 * with its previous position and increments or decrements @p var within the
 * allowed range.
 *
 * Behavior:
 *  - If encoder rotated forward  → *var is incremented (up to @p max_value).
 *  - If encoder rotated backward → *var is decremented (down to @p min_value).
 *  - If there is no movement     → *var remains unchanged.
 *  - After processing, enc->old_position is updated to the current position.
 *
 * Typical Use Cases:
 *  - Adjusting menu counters
 *  - Configuring temperature, duration, power limits
 *  - Smooth and bounded numeric input via rotary encoder
 *
 * @param[in,out] enc         Pointer to encoder instance.
 *                            Uses enc->position and updates enc->old_position.
 *
 * @param[in,out] var         Pointer to the variable being controlled by encoder.
 *                            This value will be incremented or decremented.
 *
 * @param[in]     min_value   Minimum allowed value for *var.
 *
 * @param[in]     max_value   Maximum allowed value for *var.
 *
 * @note Function is declared `static inline` to allow high-frequency calling
 *       inside main loops or timer interrupts with minimal overhead.
 *
 * @note The caller must update enc->position externally (e.g., in encoder ISR
 *       or polling logic).
 */
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

/**
 * @brief Initializes and starts all rotary encoder timers in interrupt mode.
 *
 * This function enables hardware-based quadrature decoding for all three
 * rotary encoders connected to TIM2, TIM3, and TIM4. Each timer is configured
 * in encoder interface mode and started with interrupt generation enabled.
 *
 * Behavior:
 *  - TIM2, TIM3, and TIM4 are started in encoder mode with IT enabled.
 *  - Each timer will trigger interrupts on encoder count changes.
 *  - The corresponding IRQ handlers must process direction/count events.
 *
 * Purpose:
 *  - Allows non-blocking, hardware-accurate encoder reading.
 *  - Offloads quadrature decoding to the timer peripheral.
 *  - Ensures real-time responsiveness for menu navigation and parameter tuning.
 *
 * @note Requires encoder timers (TIM2, TIM3, TIM4) to be properly configured
 *       in CubeMX with:
 *          - Encoder Mode (TI1 + TI2)
 *          - Correct GPIO alternate functions
 *          - Enabled interrupts
 *
 * @note The global timer handles (htim2, htim3, htim4) must be declared extern.
 *
 * @retval None
 */
void Encoder_Init(void);

/**
 * @brief  Handles encoder 1 push-button actions for the main UI state machine.
 *
 * This function interprets the encoder 1 push-button press according to the
 * current menu flag (@ref MenuCtx::flag) and the selected item index
 * (@ref MenuCtx::counter). It performs high-level navigation and parameter
 * editing for:
 *
 *  - HOME / PROGRAMS:
 *      * From HOME: jumps to @ref PROGRAMS or @ref SETTINGS depending on the
 *        selected item.
 *      * From PROGRAMS: navigates to FASTSTART, advanced temperature control
 *        menus or back to HOME.
 *
 *  - PROGRAMS_FASTSTART and related screens:
 *      * Edits the fast-start profile parameters (target temperature, rise time,
 *        fall time, duration).
 *      * Activates the fast-start mode by enabling @ref CookingCtx::faststart
 *        and disabling all stage-based profiles.
 *
 *  - PROGRAMS_ADVTEMPCTRL and related stage screens (T1..T6):
 *      * Configures up to six advanced temperature stages:
 *          - Rising times (@ref PROGRAMS_ADVTEMPCTRL_RISINGTIME_Sx)
 *          - Target temperatures and durations (@ref PROGRAMS_ADVTEMPCTRL_Tx_TEMP,
 *            @ref PROGRAMS_ADVTEMPCTRL_Tx_DUR)
 *          - Stage enable / reset operations (@ref PROGRAMS_ADVTEMPCTRL_Tx_SETRESET)
 *      * Clears and re-initializes individual stage structures when requested.
 *      * After configuration, allows generating a complete cooking profile and,
 *        if already generated, starting cooking (switching to @ref COOKING).
 *
 *  - SETTINGS and PI settings:
 *      * Navigates to PI parameter settings and time/date settings.
 *      * Updates global PI gains (Kp, Ki), slew rate and maximum power.
 *      * Saves settings to non-volatile memory via Settings_Save() and performs
 *        a system reset when requested.
 *
 *  - Time & date settings (SETTINGS_TIMEDATE):
 *      * Uses the provided @p t snapshot (Sens) to prefill the time/date fields.
 *      * Writes new RTC values via DS1307_SetHour/Minute/Date/Month/Year calls.
 *
 *  - COOKING and COOKING_DURATION:
 *      * In @ref COOKING: opens the duration edit screen for the currently
 *        active stage (STAGE_1..STAGE_6 or FASTSTART) using the remaining time.
 *      * In @ref COOKING_DURATION: writes back the new duration for the active
 *        stage into @ref CookingCtx and returns to the profile generate screen.
 *
 * @param[in,out] m  Pointer to the active menu context. Reads and updates
 *                   @ref MenuCtx::flag and @ref MenuCtx::counter through
 *                   display_goto() / display_set().
 * @param[in,out] c  Pointer to the cooking context, containing all stage
 *                   structures and the fast-start profile. This function
 *                   modifies stage temperatures, durations, rising times,
 *                   enable flags and generated/active flags.
 * @param[in]     t  Snapshot of the current time/date (Sens) used to initialize
 *                   the time/date setting screens when entering SETTINGS_TIMEDATE.
 *
 * @note  This function is typically called when encoder 1 push-button is
 *        pressed. It should remain non-blocking and must not perform any
 *        long-running operations (no delays, no heavy I/O).
 */
void button_handler_encoder1(MenuCtx *m, CookingCtx *c, Sens t);
/**
 * @brief  Handles encoder 2 push-button actions for cooking temperature changes.
 *
 * This function is responsible for editing the target temperature of the
 * currently active cooking stage (STAGE_1..STAGE_6 or FASTSTART) and
 * switching between the COOKING and COOKING_TEMPCHANGE screens.
 *
 * Behavior:
 *  - When m->flag == COOKING:
 *      - Opens the COOKING_TEMPCHANGE screen using display_set().
 *      - Initializes the menu counter with the current target temperature of
 *        the active stage (c->stageX->temperature or c->faststart->temperature).
 *
 *  - When m->flag == COOKING_TEMPCHANGE:
 *      - Writes the new temperature from m->counter into the active stage:
 *          - c->stage1->temperature, c->stage2->temperature, ... or
 *            c->faststart->temperature depending on c->flag.
 *      - Sets m->flag back to PROGRAMS_ADVTEMPCTRL_GENERATE to return to the
 *        profile generation / summary screen.
 *      - Resets m->counter to 0.
 *
 * @param[in,out] m  Menu context. Reads and modifies m->flag and m->counter,
 *                   and uses display_set() to navigate between COOKING and
 *                   COOKING_TEMPCHANGE screens.
 * @param[in,out] c  Cooking context. Updates the temperature field of the
 *                   currently active stage (stage1..stage6) or faststart,
 *                   based on c->flag.
 *
 * @note  Typically called when encoder 2 push-button is pressed. It should
 *        remain non-blocking and must not perform any long delays or heavy I/O.
 */
void button_handler_encoder2(MenuCtx *m, CookingCtx *c);
/**
 * @brief  Handles encoder 3 push-button actions for heater maximum power editing.
 *
 * This function manages transitions between the COOKING and COOKING_PWRCHANGE
 * screens and updates the maximum allowed heater power. It also reinitializes
 * all PI controllers with the newly selected power limit.
 *
 * Behavior:
 *  - When m->flag == COOKING:
 *      - Opens the COOKING_PWRCHANGE screen using display_set().
 *      - Initializes the edit screen with the current maximum power (s->max_pwr).
 *
 *  - When m->flag == COOKING_PWRCHANGE:
 *      - Stores the new maximum power from m->counter into s->max_pwr.
 *      - Reinitializes all PI controllers (pi1, pi2, pi3) with:
 *          - their existing gains (s->kp, s->ki),
 *          - output reset to 0.0f,
 *          - updated maximum power limit.
 *      - Returns to the COOKING screen via display_goto().
 *
 * @param[in,out] m    Menu context. Reads and modifies m->flag and uses
 *                     display_set() / display_goto() for screen transitions.
 * @param[in,out] s    Global settings structure. Updates s->max_pwr and reads
 *                     PI parameters s->kp and s->ki for controller reinitialization.
 * @param[in,out] pi1  PI controller instance for heater zone 1.
 * @param[in,out] pi2  PI controller instance for heater zone 2.
 * @param[in,out] pi3  PI controller instance for heater zone 3.
 *
 * @note  This function is typically triggered by encoder 3 push-button.
 *        It must remain non-blocking and should not perform heavy operations.
 */
void button_handler_encoder3(MenuCtx *m, setting_t *s, PI_Oven *p1, PI_Oven *p2, PI_Oven *p3);

/**
 * @brief  Handles encoder 1 tick events and updates the menu counter with bounds.
 *
 * This function is called on every tick (step) of encoder 1. It interprets the
 * current menu flag (m->flag) and uses set_value() to update m->counter within
 * the valid range for the active screen or parameter.
 *
 * For each UI state, it defines the allowed minimum and maximum values of
 * m->counter, for example:
 *  - HOME, PROGRAMS, SETTINGS:
 *      * m->counter selects menu items (0..N).
 *  - PROGRAMS_FASTSTART and related screens:
 *      * Edits fast-start temperature, rise time, fall time and duration within
 *        defined numeric ranges (e.g. 0..300 °C, 0..512 minutes).
 *  - PROGRAMS_ADVTEMPCTRL and all T1..T6 / RISINGTIME_Sx screens:
 *      * Edits multi-stage advanced temperature profile parameters such as:
 *        stage index, rising time, temperature and duration.
 *  - SETTINGS_PI and related screens:
 *      * Edits PI controller gains (Kp, Ki), slew rate and max power as scaled
 *        integer values (e.g. 0..1000 for gains, 0..100 for slew and power).
 *  - SETTINGS_TIMEDATE and related screens:
 *      * Edits RTC hour, minute, day, month and year fields (e.g. 0..23 hours,
 *        0..59 minutes, 2020..2100 years).
 *  - COOKING_DURATION:
 *      * Edits the duration of the active cooking stage within 0..1000 units.
 *
 * All updates to m->counter are performed through set_value(), which is
 * responsible for applying the encoder delta and clamping the result to the
 * specified [min, max] range.
 *
 * @param[in]     e  Pointer to encoder state structure for encoder 1. Provides
 *                   tick/direction information to set_value().
 * @param[in,out] m  Pointer to the active menu context. Reads m->flag to decide
 *                   which range to use and updates m->counter accordingly.
 *
 * @note  This function should be called frequently (e.g. in an encoder ISR or a
 *        periodic task) whenever encoder 1 generates tick events. It must be
 *        non-blocking and should only perform lightweight arithmetic and
 *        boundary checks via set_value().
 */
void tick_handler_encoder1(encoder *e, MenuCtx *m);
/**
 * @brief  Handles encoder 2 tick events for stage temperature adjustment.
 *
 * This function updates the menu counter (m->counter) according to the active
 * cooking stage (c.flag). It is used specifically during temperature editing
 * screens, ensuring that the encoder value remains within a valid numeric range.
 *
 * Behavior:
 *  - For all stage types (STAGE_1..STAGE_6 and FASTSTART):
 *      * Calls set_value() to clamp m->counter between 0 and 300.
 *      * This range corresponds to allowable temperature values (°C).
 *
 *  - If c.flag does not correspond to a temperature-editable stage,
 *    no action is performed.
 *
 * @param[in]     e  Encoder state structure for encoder 2. Provides direction
 *                   and tick information to set_value().
 * @param[in,out] m  Menu context. Updates m->counter based on encoder movement.
 * @param[in]     c  Cooking context. Only c.flag is used to determine which
 *                   range to apply; c is not modified.
 *
 * @note  This function must remain lightweight and is typically called from
 *        an encoder ISR or a fast periodic task when encoder 2 generates ticks.
 */
void tick_handler_encoder2(encoder *e, MenuCtx *m, CookingCtx c);
/**
 * @brief  Handles encoder 3 tick events and updates the menu counter.
 *
 * This function applies encoder 3 tick movements to m->counter by calling
 * set_value(). It restricts the counter to the range [0, 100], which is used
 * when editing power-related parameters (e.g., maximum heater power percentage).
 *
 * Behavior:
 *  - Regardless of the current menu screen, m->counter is updated using
 *    set_value(e, &m->counter, 0, 100).
 *  - The encoder movement is processed and clamped so that the resulting value
 *    always stays within 0..100.
 *
 * @param[in]     e  Pointer to encoder state data for encoder 3. Provides tick
 *                   delta and direction information to set_value().
 * @param[in,out] m  Pointer to the active menu context. Updates m->counter
 *                   according to encoder movement within the 0..100 range.
 *
 * @note  This function is lightweight and suitable for encoder ISR usage or
 *        a fast periodic task that processes encoder ticks.
 */
void tick_handler_encoder3(encoder *e, MenuCtx *m);


#endif /* INC_USERLIBRARIES_ENCODER_HANDLER_H_ */
