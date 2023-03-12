/**
 * This algorithm consist in find the maximum power point, 
 * iterating for all the possible duty cycles with a defined step and frequency
*/
#include "brute_force.h"

static algorithms_metadata_t metadata;
static float actual_duty = 0.0;


void brute_force_init(float initial_duty)
{
    metadata.absolute_mpp_duty = 0.0;
    metadata.absolute_mpp_power = 0.0;
    metadata.done = 0;
    actual_duty = initial_duty;
}

float brute_force_run(const volatile inputs_t *inputs)
{
    static float step = 0.001;
    float pi = inputs->v_p * inputs->i_p;
    
    if (pi > metadata.absolute_mpp_power)
    {
        metadata.absolute_mpp_power = pi;
        metadata.absolute_mpp_duty = pwm_get_duty();
    }
    actual_duty += step;

    if (actual_duty > PWM_MAX)
        metadata.done = 1;

    return actual_duty;
}

const algorithms_metadata_t *brute_force_get_metadata(void)
{
    return &metadata;
}