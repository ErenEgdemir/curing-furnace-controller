/*
 * pi_contor.h
 *
 *  Created on: Oct 22, 2025
 *      Author: erenegdemir
 */

#ifndef INC_PI_CONTROL_H_
#define INC_PI_CONTROL_H_

#include <stdint.h>

typedef struct {
    /* Ayarlar */
    float kp;            // Oransal kazanç
    float ki;            // İntegral kazanç (1/s)

    float out_min;       // Çıkış alt sınır (genelde 0%)
    float out_max;       // Çıkış üst sınır (genelde 100%)

    float meas_alpha;    // 0..1, ölçüm EMA filtresi (1 = filtre yok)
    float slew_per_sec;  // [%/s] çıkış değişim limiti (0 = kapalı)

    /* Çalışma durumu */
    float integrator;    // integral bellek
    float last_u;        // son çıkış (%)
    float y_filt;        // filtreli ölçüm
    uint8_t enabled;     // 1=aktif

} PI_Oven;

void PI_Control_Init(PI_Oven* c, float kp, float ki, float out_min, float out_max);

/* Bir adım çalıştır: setpoint [°C], measurement [°C], dt [s] -> çıktı [% 0..100] */
float PI_Control_Update(PI_Oven* c, float setpoint, float measurement, float dt);

static inline void PI_Control_SetTunings(PI_Oven* c, float kp, float ki){
    c->kp = kp; c->ki = ki;
}
static inline void PI_Control_SetFilter(PI_Oven* c, float alpha){
    c->meas_alpha = alpha;
}
static inline void PI_Control_SetSlew(PI_Oven* c, float slew_per_sec){
    c->slew_per_sec = slew_per_sec;
}
static inline void PI_Control_Reset(PI_Oven* c){
    c->integrator = 0.0f; c->last_u = 0.0f; c->y_filt = 0.0f;
}

/* ==================== SSR Zaman-Orantılı Sürüş ====================
   Sabit pencere süresinde (ör. 1.0 s) duty% kadar ON yapar.
   AC SSR/triac veya DC MOSFET için uygundur.
*/
typedef struct {
    float window_s;   // pencere [s], örn 1.0
    float acc_s;      // pencere içi sayaç [s]
} TPWM_Window;

static inline void TPWM_Init(TPWM_Window* w, float window_seconds){
    w->window_s = (window_seconds > 0.05f) ? window_seconds : 0.05f;
    w->acc_s = 0.0f;
}

/* duty_percent: 0..100, dt [s] -> 0/1 gate */
uint8_t TPWM_Step(TPWM_Window* w, float duty_percent, float dt);





#endif /* INC_PI_CONTROL_H_ */
