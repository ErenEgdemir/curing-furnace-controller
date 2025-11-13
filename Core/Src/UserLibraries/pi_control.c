/*
 * pi_control.c
 *
 *  Created on: Oct 22, 2025
 *      Author: erenegdemir
 */
#include "pi_control.h"
#include <math.h>

/* İç yardımcılar */
static inline float clampf(float v, float lo, float hi){
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void PI_Control_Init(PI_Oven* c, float kp, float ki, float out_min, float out_max)
{
    c->kp = kp;
    c->ki = ki;
    c->out_min = (out_min < out_max) ? out_min : out_max;
    c->out_max = (out_max > out_min) ? out_max : out_min;

    c->meas_alpha = 1.0f;   // filtre kapalı
    c->slew_per_sec = 0.0f; // slew kapalı

    c->integrator = 0.0f;
    c->last_u = 0.0f;
    c->y_filt = 0.0f;
    c->enabled = 1u;
}

/* Anti-windup stratejisi:
   - İntegrali her adım e += ki*dt ile büyütmeden önce hesaplarız.
   - Çıkış doygunluğuna göre aynı işaretliyse integrali kısıtlarız (clamp).
   - Böylece doygunlukta taşma (windup) sınırlanır.
*/
float PI_Control_Update(PI_Oven* c, float setpoint, float measurement, float dt)
{
    /* 1) Ölçümü filtrele (EMA) */
    float y = measurement;
    if (c->meas_alpha < 1.0f){
        float a = c->meas_alpha; if (a < 0.0f) a = 0.0f; if (a > 1.0f) a = 1.0f;
        c->y_filt = (c->y_filt == 0.0f) ? y : (a*y + (1.0f - a)*c->y_filt);
        y = c->y_filt;
    } else {
        c->y_filt = y;
    }

    /* 2) Hata ve P terimi */
    float e  = setpoint - y;
    float up = c->kp * e;

    /* 3) İ n t e g r a l  (anti-windup ile) */
    if (fabsf(e) > 10.0f) {

    }else if(fabsf(e) < 7.0f){
        if (dt > 0.0f && c->enabled){
            c->integrator += c->ki * e * dt;
            /* Çıkış sınırını aşmayacak şekilde önceden kısıtla */
            c->integrator = clampf(c->integrator, c->out_min - up, c->out_max - up);
        }
    }



    /* 4) Toplam ve sınır */
    float u = up + c->integrator;
    u = clampf(u, c->out_min, c->out_max);

    /* 5) Slew (opsiyonel) */
    if (c->slew_per_sec > 0.0f && dt > 0.0f){
        float du_max = c->slew_per_sec * dt;
        float du = u - c->last_u;
        if (du > du_max) u = c->last_u + du_max;
        else if (du < -du_max) u = c->last_u - du_max;
    }

    c->last_u = u;
    return u; // % (0..100 aralığı önerilir)
}

/* Zaman-orantılı PWM: pencere içinde duty% kadar ON */
uint8_t TPWM_Step(TPWM_Window* w, float duty_percent, float dt)
{
    if (dt <= 0.0f) dt = 0.0f;
    w->acc_s += dt;
    if (w->acc_s >= w->window_s) w->acc_s -= w->window_s;

    if (duty_percent <= 0.0f) return 0u;
    if (duty_percent >= 100.0f) return 1u;

    float on_time = (duty_percent * 0.01f) * w->window_s;
    return (w->acc_s <= on_time) ? 1u : 0u;
}

