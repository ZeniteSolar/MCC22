#ifndef PWM_H
#define PWM_H

#include "stm32l4xx_hal.h"
#include "utils.h"
#include "conf.h"

#define TIM_FREQ 80e6

typedef struct
{
    // Pwm Timer Handler
    TIM_HandleTypeDef *htim;
    float duty;
    float freq;
} pwm_t;

/**
 * @brief Initialize pwm
 * @param htim pwm timer handler
 */
void pwm_init(TIM_HandleTypeDef *htim);

/**
 * @brief Start pwm
 * @param initial_freq initial frequency in hertz
 * @param initial_duty initial duty cycle
 */
HAL_StatusTypeDef pwm_start(float initial_freq, float initial_duty);

/**
 * @brief Set pwm duty cycle
 * @param duty duty cycle in a range of 0.0 to 1.0
 */
void pwm_set_duty(float duty);

/**
 * @brief Set pwm frequency
 * @param duty frequency in Hz
 */
void pwm_set_freq(float freq);

/**
 * @brief Get pwm duty cycle
 * @return duty cycle in a range of 0.0 to 1.0
 */
float pwm_get_duty(void);

/**
 * @brief Get pwm frequency
 * @return duty frequency in Hz
 */
float pwm_get_freq(void);

#endif