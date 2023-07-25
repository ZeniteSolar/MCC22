#include "pwm.h"

static pwm_t pwm;

void pwm_init(TIM_HandleTypeDef *htim)
{
	// Configure pwm
	pwm.htim = htim;
	pwm.freq = 0;
	pwm.duty = 0;
}

HAL_StatusTypeDef pwm_start(float initial_freq, float initial_duty)
{
	pwm_set_freq(initial_freq);
	pwm_set_duty(initial_duty);

	if (HAL_TIM_Base_Start(pwm.htim) != HAL_OK)
	{
		LOG_ERROR("error initializing pwm timer");
		return HAL_ERROR;
	}

	if (HAL_TIM_PWM_Start(pwm.htim, TIM_CHANNEL_1) != HAL_OK)
	{
		LOG_ERROR("error initializing pwm");
		return HAL_ERROR;
	}
	return HAL_OK;
}

void pwm_set_duty(float duty)
{

	if (duty < PWM_MIN)
		duty = PWM_MIN;
	else if (duty > PWM_MAX)
		duty = PWM_MAX;

	uint32_t top = __HAL_TIM_GET_AUTORELOAD(pwm.htim);

	__HAL_TIM_SET_COMPARE(pwm.htim, TIM_CHANNEL_1, (int)(((float)top + 1.0f) * duty));

	pwm.duty = duty;
}

void pwm_set_freq(float freq)
{
	/**
	 * Frequency = TIM_FREQ / (2 * period * prescaler)
	 * for better resolution period should be great as possible
	 */

	// Calculate period for the required frequency assuming prescaler as 1
	uint32_t period = TIM_FREQ / (freq * 2);

	if (period > 65535)
		period = 65535; // 16bit period

	// if period was not enough to achieve the frequency, calculate the prescaler
	uint32_t prescaler = TIM_FREQ / (2 * freq * period);

	if (prescaler > 65535)
		prescaler = 65535;

	// Update duty cycle for the new Counter Period

	uint32_t last_top = __HAL_TIM_GET_AUTORELOAD(pwm.htim);
	uint32_t last_compare = __HAL_TIM_GET_COMPARE(pwm.htim, TIM_CHANNEL_1);

	float duty = (float)last_compare / ((float)last_top + 1.0f);

	pwm_set_duty(0.0f);

	__HAL_TIM_SetAutoreload(pwm.htim, period);
	__HAL_TIM_SET_PRESCALER(pwm.htim, prescaler);

	pwm_set_duty(duty);

	pwm.freq = freq;
}

float pwm_get_duty(void)
{
	uint32_t top = __HAL_TIM_GET_AUTORELOAD(pwm.htim);
	uint32_t compare = __HAL_TIM_GET_COMPARE(pwm.htim, TIM_CHANNEL_1);

	return (float)compare / ((float)top + 1.0f);
}

float pwm_get_freq(void)
{
	uint32_t period = __HAL_TIM_GET_AUTORELOAD(pwm.htim);
	// HAL doesn't have macro to get prescaler
	uint32_t prescaler = pwm.htim->Instance->PSC;
	return TIM_FREQ / (2 * period * prescaler);
}