/**
 * @file    Cooking_Handler.h
 * @brief   Public API for the cooking control subsystem.
 *
 * This header exposes the interfaces for:
 *   - Cooking_Init()       : PI kontrolcülerini ve PWM çıkışlarını başlatır.
 *   - Cooking_Handler()    : Fırının ana pişirme durum makinesini yürütür.
 *   - update_DutyCycle()   : Hesaplanan görev döngülerini TIM PWM kanallarına yazar.
 *   - temp_drift()         : Sensör–ortalama sıcaklık sapmasını hesaplar.
 *   - check_error()        : Sensör sapması, aşırı sıcaklık ve sensör arızalarını kontrol eder.
 *
 * Temel bileşenler:
 *   - CookingCtx           : Pişirme sırasındaki durum, profil indexleri, görev döngüleri ve
 *                            kalan süre bilgilerini tutar.
 *   - CookingFlag          : Pişirme durum makinesi bayrakları (PENDING, STAGE_1..6, FASTSTART, ABORT).
 *   - Sens                 : Tüm sıcaklık sensörlerinden okunan anlık değerleri içerir.
 *   - PI_Oven              : Her bir ısıtıcı bölgesi için PI kontrol yapısını temsil eder.
 *
 * Tipik kullanım:
 *   1. Sistem açılışında ve ayarlar yüklendikten sonra Cooking_Init() çağrılır.
 *   2. Sabit zaman adımıyla (PI_DT) ana kontrol döngüsünde Cooking_Handler() çağrılır.
 *   3. update_DutyCycle() doğrudan Cooking_Handler() içinde kullanılır.
 *   4. Güvenlik ve hata kontrolleri check_error() ile yapılır.
 *
 * Notlar:
 *   - Bu modül, TIM5 PWM kanallarını (CH2, CH3, CH4) kullanarak üç ısıtıcı bölgesini sürer.
 *   - PI_DT sabiti, PI_Control_Update() fonksiyonuna verilen zaman adımını tanımlar.
 *   - Güvenlik ihlallerinde CookingCtx->flag, ABORT durumuna çekilir ve sistem reset’e gidebilir.
 *
 * @author
 *   Eren Eğdemir
 */

#ifndef INC_USERLIBRARIES_COOKING_HANDLER_H_
#define INC_USERLIBRARIES_COOKING_HANDLER_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "pi_control.h"
#include "common_def.h"
#include "types.h"

#define PI_DT	0.1f
#define TRUE	1


extern TIM_HandleTypeDef htim5;

/**
 * @brief  Computes the instantaneous temperature drift between a sensor value
 *         and the average system temperature.
 *
 * This function returns the difference between the provided average temperature
 * (`avg`) and the current sensor reading (`sens`). It is used to evaluate
 * whether a specific sensor deviates from the expected thermal behavior.
 *
 * Behavior:
 *  - Calculates: drift = avg - sens
 *  - Positive result → sensor is reading lower than the average
 *  - Negative result → sensor is reading higher than the average
 *
 * @param[in] sens  Current temperature reading of the sensor.
 * @param[in] avg   Average temperature computed across multiple sensors/zones.
 *
 * @return float    The computed drift value (avg − sens).
 *
 * @note  This function is `static inline`, making it safe and efficient to call
 *        inside fast control loops such as PI update cycles.
 */
static inline float temp_drift(float sens, float avg)
{
	float x = avg - sens;

	return x;
}

/**
 * @brief  Performs safety checks on sensor readings and aborts cooking on fault.
 *
 * This function evaluates the current temperature readings in @p t and updates
 * the cooking state @p c->flag to ABORT if any safety condition is violated.
 * It checks for:
 *
 *  1) Sensor-to-average drift on the composite sensors (sens1..sens3):
 *      - Computes comp_temp as the average of sens1_temp, sens2_temp, sens3_temp.
 *      - For each of sens1_temp, sens2_temp, sens3_temp:
 *          • If temp_drift(sensor, comp_temp) > 5  or < -5  → ABORT.
 *
 *  2) Sensor-to-average drift on the furnace sensors (sens4..sens5):
 *      - Computes furnice_temp as the average of sens4_temp and sens5_temp.
 *      - For sens4_temp (and sens5_temp in the intended design):
 *          • If temp_drift(sensor, furnice_temp) > 10 or < -10 → ABORT.
 *
 *  3) Over-temperature / invalid sensor readings:
 *      - If any of sens1_temp..sens5_temp exceeds 250°C → ABORT.
 *      - If any of sens1_temp..sens5_temp equals 0 (assumed sensor fault) → ABORT.
 *
 * When any condition is met, c->flag is set to ABORT, which will eventually
 * stop heating and trigger a system reset in the Cooking_Handler().
 *
 * @param[in,out] c  Cooking context. Its flag field (c->flag) is updated to
 *                   ABORT when a fault is detected. Other fields are not
 *                   modified.
 * @param[in]     t  Snapshot of current temperature readings from all sensors:
 *                   t.sens1_temp .. t.sens5_temp. This parameter is not
 *                   modified.
 *
 * @note  This function is `static inline` and intended to be called frequently
 *        from the main control loop (e.g., within Cooking_Handler()) to ensure
 *        fast reaction to faults.
 */
static inline void check_error(CookingCtx *c, Sens t){

	float comp_temp = 0;
	float furnice_temp = 0;

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

/**
 * @brief  Initializes the cooking system and all PI controllers.
 *
 * This function prepares the heating system before cooking begins by:
 *
 *  - Starting PWM outputs for all heater channels (TIM5 CH2, CH3, CH4).
 *  - Initializing PI controllers (pi1, pi2, pi3) using the global settings:
 *        • kp, ki gains from @p setting
 *        • max power limit from @p setting.max_pwr
 *        • output initialized to 0.0f
 *  - Applying default low-pass filtering to each PI controller
 *        (PI_Control_SetFilter(..., 0.6f); recommended range 0.3–0.7)
 *  - Applying configured slew-rate limits from @p setting.slew
 *
 * This function must be called before starting any cooking sequence.
 *
 * @param[in]     setting  Current system settings (PI gains, max power, slew rate).
 *                         Values are copied, @p setting is not modified.
 * @param[in,out] pi1      PI controller instance for heater channel 1.
 * @param[in,out] pi2      PI controller instance for heater channel 2.
 * @param[in,out] pi3      PI controller instance for heater channel 3.
 *
 * @note  This function assumes that TIM5 PWM channels have been properly
 *        configured in CubeMX prior to calling Cooking_Init().
 *
 * @note  All PI controllers are reset to zero output on initialization.
 */
void Cooking_Init(setting_t setting, PI_Oven *pi1, PI_Oven *pi2, PI_Oven *pi3);

/**
 * @brief  Updates PWM duty cycles for all heater channels.
 *
 * This function writes the computed heater duty-cycle values from the
 * cooking context into the corresponding PWM output channels of the timer.
 *
 * Behavior:
 *  - Each duty value (r1_duty, r2_duty, r3_duty) is scaled by ×10 before being
 *    written to the timer compare registers.
 *    (Example: r1_duty = 50 → CCR = 500)
 *
 *  - Duty cycles are applied to:
 *        • TIM_CHANNEL_2 → Heater zone 1
 *        • TIM_CHANNEL_3 → Heater zone 2
 *        • TIM_CHANNEL_4 → Heater zone 3
 *
 * @param[in]     c     Cooking context containing duty-cycle values for each
 *                      heater zone (r1_duty, r2_duty, r3_duty).
 *                      These values are expected to be 0–100 (%).
 *
 * @param[in,out] htim  Timer handle whose PWM channels are used for heater
 *                      control. This function modifies the CCR registers of
 *                      CH2, CH3, and CH4.
 *
 * @note  The ×10 scaling assumes that the PWM timer period is configured for a
 *        0–1000 resolution (e.g., ARR = 1000). Adjust scaling if timer settings
 *        change.
 *
 * @note  This function is lightweight and suitable for periodic calls inside
 *        the main control loop.
 */
void update_DutyCycle(CookingCtx *c, TIM_HandleTypeDef *htim);

/**
 * @brief  Main cooking state machine handler for the curing oven.
 *
 * This function executes the active cooking stage according to c->flag. For each
 * active mode, it:
 *  - Computes new PI controller outputs for all three heater zones,
 *  - Updates the average duty cycle,
 *  - Performs safety checks,
 *  - Writes new PWM duty cycles to the hardware timer,
 *  - Updates remaining time and stage index counters.
 *
 * Behavior by state:
 *
 *  - PENDING:
 *      - Cooking has not started yet. Only check_error() is called to monitor
 *        for faults before starting.
 *
 *  - FASTSTART:
 *      - Uses c->faststart->tempForMin[i] as the target temperature profile
 *        over time (per-minute or per-index resolution).
 *      - For each zone:
 *          rX_duty = PI_Control_Update(piX,
 *                                      c->faststart->tempForMin[i],
 *                                      t.sensX_temp,
 *                                      PI_DT);
 *      - duty_mean is computed as the average of all three zone duties.
 *      - check_error() is called to validate safety limits.
 *      - update_DutyCycle() writes the new PWM values to htim5.
 *      - A per-loop counter k is incremented; when k reaches 600:
 *          * i is incremented (advance in the tempForMin[] profile),
 *          * k is reset to 0,
 *          * c->faststart->remTime is decremented.
 *      - When i >= c->faststart->totalDuration, cooking is finished and
 *        c->flag is set to ABORT.
 *
 *  - STAGE_1 .. STAGE_6:
 *      - Uses c->stageX->tempForMin[i] as the current target temperature for
 *        that stage X, applied to all three heater zones.
 *      - For each zone:
 *          rX_duty = PI_Control_Update(piX,
 *                                      c->stageX->tempForMin[i],
 *                                      t.sensX_temp,
 *                                      PI_DT);
 *      - duty_mean is calculated as the average of r1_duty, r2_duty, r3_duty.
 *      - check_error() is called to validate temperature and system safety.
 *      - update_DutyCycle() writes PWM updates to htim5.
 *      - On each call, k is incremented; when k reaches 600:
 *          * i is incremented to move to the next profile index,
 *          * k is reset to 0,
 *          * c->stageX->remTime is decremented.
 *      - When i >= c->stageX->totalDuration:
 *          * If the next stage (stageX+1) is enabled (state == TRUE),
 *            c->flag is advanced to the next STAGE, and i, k are reset to 0.
 *          * Otherwise, the sequence ends and c->flag is set to ABORT.
 *
 *  - ABORT:
 *      - Immediately sets PWM compare for heater channel 3 to 0.
 *      - Triggers a system reset via HAL_NVIC_SystemReset().
 *
 * @param[in,out] c    Cooking context. Holds current state (c->flag), per-stage
 *                     profiles (tempForMin, totalDuration, remTime, state),
 *                     and computed duty cycles (r1_duty, r2_duty, r3_duty,
 *                     duty_mean). This function updates these fields.
 * @param[in]     t    Snapshot of current sensor readings and time information.
 *                     At minimum, uses t.sens1_temp, t.sens2_temp, t.sens3_temp
 *                     as feedback inputs for the PI controllers. Not modified.
 * @param[in,out] pi1  PI controller instance for heater zone 1. Its internal
 *                     state (integrator, output, etc.) is updated by
 *                     PI_Control_Update().
 * @param[in,out] pi2  PI controller instance for heater zone 2.
 * @param[in,out] pi3  PI controller instance for heater zone 3.
 *
 * @note  This function is intended to be called periodically with a fixed
 *        time step corresponding to PI_DT. The internal counters i and k,
 *        together with the threshold 600, define the temporal resolution of
 *        the temperature profile (e.g., 600 calls per minute depending on
 *        PI_DT and scheduler frequency).
 *
 * @note  Global/static variables such as i, k and timer handle htim5 are used
 *        internally. They must be initialized consistently before starting
 *        the cooking sequence.
 *
 * @note  In ABORT state, a full system reset is requested. Ensure any critical
 *        data is saved before entering this state.
 */
void Cooking_Handler(CookingCtx *c, Sens t, PI_Oven *pi1, PI_Oven *pi2, PI_Oven *pi3);


#endif /* INC_USERLIBRARIES_COOKING_HANDLER_H_ */
