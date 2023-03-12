#include "control.h"
#include "peo.h"
#include "brute_force.h"
#include "fixed.h"
#include "pi.h"

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

/**
 * @brief Script for the control logic,
 * this function describes the sequence and the conditions to use different algorithms
 */
void control_script(void)
{

    // When brute force ends initialize the continuous algorithm
    if (control.algorithm_running == BRUTE_FORCE && brute_force_get_metadata()->done)
    {
        control_set_peo(brute_force_get_metadata()->absolute_mpp_duty);
    }

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