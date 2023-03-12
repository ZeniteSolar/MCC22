/**
 * TODO
*/
#include "pi.h"

static algorithms_metadata_t metadata;
static float actual_duty = 0.0;


void pi_init(float initial_duty)
{
    metadata.absolute_mpp_duty = 0.0;
    metadata.absolute_mpp_power = 0.0;
    metadata.done = 0;
    actual_duty = initial_duty;
}

float pi_run(const volatile inputs_t *inputs)
{
    float pi = inputs->v_p * inputs->i_p;
    
    if (pi > metadata.absolute_mpp_power)
    {
        metadata.absolute_mpp_power = pi;
        metadata.absolute_mpp_duty = pwm_get_duty();
    }
    return 0.0;
}

const algorithms_metadata_t *pi_get_metadata(void)
{
    return &metadata;
}