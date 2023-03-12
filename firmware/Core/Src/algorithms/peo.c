#include "peo.h"

static algorithms_metadata_t metadata;

enum observations{
    PREV = 0,
    ACTUAL = 1,
    SIZE
};

void peo_init(float initial_duty)
{
    metadata.absolute_mpp_duty = 0;
    metadata.absolute_mpp_power = 0;
    metadata.done = 0;
}

float peo_run(const volatile inputs_t *inputs)
{
    static float step = 0.01;
    static float pi[SIZE] = {0.0, 0.0};
    
    // Compute input power
    pi[ACTUAL] = inputs->v_p * inputs->i_p;
    // Compute input power variation
    float dpi = pi[ACTUAL] - pi[PREV];
    
    // if last power was better, invert step
    if (dpi <= 0)
        step = -step;

    //Update Prev
    pi[PREV] = pi[ACTUAL];

    // save metadata
    if (pi[ACTUAL] > metadata.absolute_mpp_power)
    {
        metadata.absolute_mpp_duty = pwm_get_duty();
        metadata.absolute_mpp_power = pi[ACTUAL];
    }

    return pwm_get_duty() + step;
}

const algorithms_metadata_t *peo_get_metadata(void)
{
    return &metadata;
}