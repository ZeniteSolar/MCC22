#include "control.h"
#include "peo.h"
#include "brute_force.h"
#include "fixed.h"
#include "pi.h"
#include "peo_dynamic_step.h"

/**
 * @brief Enable/disable control variables print
 * 
 */
uint8_t print;

/**
 * @brief Control main structure
 * 
 */
static control_t control;

/**
 * @brief initialize control variables
 */
void control_init(void);

/**
 * @brief control start script
 */
void control_start(void);

/**
 * @brief enable/disable control,
 * set duty cycle to 0 if disabled
 * start control if enabled
 */
void control_set_enable(FunctionalState enable);

void control_init(void)
{
	print = 1;

	if (pwm_start(DEFAULT_PWM_FREQUENCY, PWM_MIN) != HAL_OK)
	{
		LOG_ERROR("error starting pwm");
	}

	control_set_freq(CONTROL_DEFAULT_FREQUENCY);
	control_start();
}

void control_set_error(errors_t error)
{
	control.errors = error;
}

/**
 * @brief  Set control period
 * 
 * @param period control period in ms
 */
void control_set_period(uint32_t period)
{
	if (period < (uint32_t)(1000 / ADC_DEFAULT_FREQUENCY))
	{
		LOG_ERROR("control period can't be faster than adc period, setting to adc period");
		control.period = (uint32_t)(1000 / ADC_DEFAULT_FREQUENCY);
		return;
	}

	control.period = period;
}

/**
 * @brief Get control period
 * 
 * @return uint32_t control period in ms
 */
uint32_t control_get_period(void)
{
	return control.period;
}

/**
 * @brief Set control frequency
 * 
 * @param freq control frequency in Hz
 */
void control_set_freq(float freq)
{
	control_set_period((uint32_t)(1000 / freq));
}

/**
 * @brief Get control frequency
 * 
 * @return float control frequency in Hz
 */
float control_get_freq(void)
{
	return 1 / control.period;
}

void control_set_print(uint8_t enable)
{
	print = enable;
}

/**
 * Control algorithms transitions
 */
void control_set_brute_force(float initial_duty)
{
	brute_force_init(initial_duty);
	control.algorithm_running = BRUTE_FORCE;
}

void control_set_peo(float initial_duty)
{
	peo_init(initial_duty);
	control.algorithm_running = PEO;
}

void control_set_peo_dynamic_step(float initial_duty)
{
	peo_dynamic_step_init(initial_duty);
	control.algorithm_running = PEO_DYN_STEP;
}

void control_set_pi(float initial_duty)
{
	pi_init(initial_duty);
	control.algorithm_running = PI;
}

void control_set_fixed(float initial_duty)
{
	fixed_init(initial_duty);
	control.algorithm_running = FIXED;
}


void control_start(void)
{
	control.forced_algorithm = DISABLE;
	control.errors = 0;
	control_set_brute_force(0.6);
}

void control_set_enable(FunctionalState enable)
{
	control.enable = enable;
	if (enable == DISABLE)
		control_set_fixed(0.0);
	else
		control_start();
}

algorithms_t control_get_algorithm(void)
{
	return control.algorithm_running;
}

void control_force_algorithm(algorithms_t algorithm, float initial_duty)
{

	switch (algorithm)
	{
	case BRUTE_FORCE:
		control_set_brute_force(initial_duty);
		break;
	case PEO:
		control_set_peo(initial_duty);
		break;
	case PEO_DYN_STEP:
		control_set_peo_dynamic_step(initial_duty);
		break;
	case PI:
		control_set_pi(initial_duty);
		break;
	case FIXED:
		control_set_fixed(initial_duty);
		break;
	default:
		return;
	}

	control.forced_algorithm = ENABLE;
}

/**
 * @brief Script for the control logic,
 * this function describes the sequence and the conditions to use different algorithms
 */
void control_script(void)
{
	static uint32_t error_triggered_time = 0;

	if (adc_get_value(ADC_BATTERY_VOLTAGE) > CONTROL_MAXIMUM_BATTERY_VOLTAGE)
	{
		control.errors |= OUTPUT_OVER_VOLTAGE;
	}

	if (control.errors && !error_triggered_time){
		/* Error handling */
		control_set_fixed(0.0);
		printf("Trating errors\n");
		error_triggered_time = HAL_GetTick();
	}

	/* Wait CONTROL_ERROR_RESET_MILLIS when a error was triggered for system stabilization*/
	if (error_triggered_time)
	{
		if ((error_triggered_time + CONTROL_ERROR_RESET_MILLIS > HAL_GetTick()))
		{
			//control_set_fixed(0.0);
			printf("Waiting system stabilization\n");
			return;
		} else
		{
			control.errors = 0;
			error_triggered_time = 0;
			control_set_brute_force(0.1);
			return;
		}
	}

	if (control.forced_algorithm)
		return;

	// When brute force ends initialize the continuous algorithm
	if (control.algorithm_running == BRUTE_FORCE && brute_force_get_metadata()->done)
	{
		control_set_peo(brute_force_get_metadata()->absolute_mpp_duty);
	}

	//if power is too low, maybe the algorithm is lost
	static uint32_t last_brute_force_time = 0;
	float panel_power = adc_get_value(ADC_PANEL_VOLTAGE) * adc_get_value(ADC_PANEL_CURRENT);
	if ( 
		(panel_power <= MINIMUM_POWER) && 
		(control.algorithm_running != BRUTE_FORCE)
	)
	{
		if (HAL_GetTick() >= last_brute_force_time)
		{
			last_brute_force_time = HAL_GetTick() + 10000;
			printf("Power too low, running brute force from algorithm %d\n", control.algorithm_running);
			control_set_brute_force(0.2f);
			return;
		}
	}

}

void control_run(void)
{
	static float duty = PWM_MIN;
	static uint32_t print_delay = 0;

	/* Compute adc average */
	adc_calculate_average();

	/* Control algorithms decision script */
	control_script();

	switch (control.algorithm_running)
	{
	case BRUTE_FORCE:
		duty = brute_force_run();
		break;
	case PEO:
		duty = peo_run();
		break;
	case PEO_DYN_STEP:
		duty = peo_dynamic_step_run();
		break;
	case PI:
		duty = pi_run();
		break;
	case FIXED:
		duty = fixed_run();
		break;
	default:
		control_set_brute_force(0.0);
		duty = 0;
	}

	if (print)
	{
		if (print_delay < HAL_GetTick())
		{
			print_delay = HAL_GetTick() + 500;
			printf("Algo: %d, Vpan: %0.2f, Ipan: %0.2f, Vbat: %0.2f, Ibat: %0.2f, Duty: %0.2f, Freq: %0.1f, Pin: %0.2f, Pout: %0.2f, Eff: %0.2f\n", 
				control.algorithm_running,
				adc_get_value(ADC_PANEL_VOLTAGE),
				adc_get_value(ADC_PANEL_CURRENT),
				adc_get_value(ADC_BATTERY_VOLTAGE),
				adc_get_value(ADC_BATTERY_CURRENT),
				pwm_get_duty() * 100.0f,
				pwm_get_freq(),
				adc_get_value(ADC_PANEL_VOLTAGE) * adc_get_value(ADC_PANEL_CURRENT),
				adc_get_value(ADC_BATTERY_VOLTAGE) * adc_get_value(ADC_BATTERY_CURRENT),
				(adc_get_value(ADC_BATTERY_VOLTAGE) * adc_get_value(ADC_BATTERY_CURRENT)) / (adc_get_value(ADC_PANEL_VOLTAGE) * adc_get_value(ADC_PANEL_CURRENT)) * 100.0f
			);
		}
	}

	pwm_set_duty(duty);

}