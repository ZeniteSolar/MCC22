#include "control.h"
#include "peo.h"
#include "brute_force.h"
#include "fixed.h"
#include "pi.h"
#include "peo_dynamic_step.h"

static uint32_t ctrl_clk_div = 2;

uint8_t print;
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
	control.inputs = adc_get_measurements();

	if (adc_start(DEFAULT_ADC_FREQ) != HAL_OK)
	{
		LOG_ERROR("error starting adc");
	}

	if (pwm_start(DEFAULT_PWM_FREQ, PWM_MIN) != HAL_OK)
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

void control_set_freq(float freq)
{
	ctrl_clk_div = 5e3 / freq;
}

float control_get_freq(void)
{
	return adc_get_freq();
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

	if (control.errors && !error_triggered_time){
		/* Trate errors */
		//control_set_fixed(0.0);
		printf("Trating errors\n");
		error_triggered_time = HAL_GetTick();
	}

	/* Wait CONTROL_ERROR_RESET_MILLIS when a error was triggered for system stabilization*/
	if (0)
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

	// if power is too low, maybe the algorithm is lost
	// static uint32_t last_brute_force_time = 0;
	// if ((control.inputs->v_p * control.inputs->i_p <= MINIMUM_POWER) && (control.algorithm_running != BRUTE_FORCE))
	// {
	//     if (last_brute_force_time + 250 >= HAL_GetTick())
	//     {
	//         last_brute_force_time = HAL_GetTick();
	//         printf("Alg: %d\n", control.algorithm_running);
	//         printf("Is brut\n");
	//         control_set_brute_force(0.1);
	//         return;
	//     }
	// }

	// if (brute_force_get_metadata()->done && control.algorithm_running == BRUTE_FORCE)
	// {
	//     printf("Set fixed with %f\n", brute_force_get_metadata()->absolute_mpp_duty);
	//     control_set_fixed(brute_force_get_metadata()->absolute_mpp_duty);
	// }
}

void control_run(void)
{
	static float duty = PWM_MIN;
	static uint32_t print_delay = 0;

	static uint32_t ctrl_clk_div_value = 0;

	if (++ctrl_clk_div_value >= ctrl_clk_div)
	{
		ctrl_clk_div_value = 0;
		control_script();

		switch (control.algorithm_running)
		{
		case BRUTE_FORCE:
			duty = brute_force_run(control.inputs);
			break;
		case PEO:
			duty = peo_run(control.inputs);
			break;
		case PEO_DYN_STEP:
			duty = peo_dynamic_step_run(control.inputs);
			break;
		case PI:
			duty = pi_run(control.inputs);
			break;
		case FIXED:
			duty = fixed_run(control.inputs);
			break;
		default:
			control_set_brute_force(0.0);
			duty = 0;
		}

		if (print)
		{
			if (print_delay < HAL_GetTick())
			{
				print_delay = HAL_GetTick() + 250;
				printf("Algo: %d, Vpan: %0.2f, Ipan: %0.2f, Vbat %0.2f, Ibat: %0.2f, Duty: %0.3f, Freq: %0.2f, Pin: %0.2f, Tr: %0.2f, Td: %0.2f, Tm: %0.2f\n", 
					control.algorithm_running,
					control.inputs->v_p,
					control.inputs->i_p,
					control.inputs->v_b,
					control.inputs->i_b,
					pwm_get_duty(),
					pwm_get_freq(),
					control.inputs->v_p * control.inputs->i_p,
					control.inputs->t_r,
					control.inputs->t_d,
					control.inputs->t_m
				);
			}
		}

		pwm_set_duty(duty);
	}

}