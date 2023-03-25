#include "control.h"
#include "peo.h"
#include "brute_force.h"
#include "fixed.h"
#include "pi.h"
#include "peo_dynamic_step.h"

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
    control.inputs = adc_get_measurements();

    if (adc_start() != HAL_OK)
    {
        LOG_ERROR("error starting adc");
    }

    if (pwm_start(100e3, 0.0) != HAL_OK)
    {
        LOG_ERROR("error starting pwm");
    }

    control_start();
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
    control_set_brute_force(0.0);
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
    if (control.forced_algorithm)
        return;

    // // When brute force ends initialize the continuous algorithm
    // if (control.algorithm_running == BRUTE_FORCE && brute_force_get_metadata()->done)
    // {
    //     control_set_peo(brute_force_get_metadata()->absolute_mpp_duty);
    // }

    // if power is too low, maybe the algorithm is lost
    if (control.inputs->v_p * control.inputs->i_p <= MINIMUM_POWER)
    {
        control_set_brute_force(PWM_MIN);
    }
}

void control_run(void)
{
    float duty = 0.0;
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

    pwm_set_duty(duty);
}