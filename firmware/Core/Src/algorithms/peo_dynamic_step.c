#include "peo_dynamic_step.h"
#include "conf.h"
#include "math.h"

static algorithms_metadata_t metadata;


enum observations
{
    PREV = 0,
    ACTUAL = 1,
    SIZE
};

static float duty[SIZE] = {0, 0};

void peo_dynamic_step_init(float initial_duty)
{
    metadata.absolute_mpp_duty = 0;
    metadata.absolute_mpp_power = 0;
    metadata.done = 0;
    duty[PREV] = initial_duty;
}

float peo_dynamic_step_run(const volatile inputs_t *inputs)
{
    // Configurations
    const float PO_STEP_SIZE = 0.001f;
    const float stepsize_max = PO_STEP_SIZE * 10;
    const float stepsize_min = PO_STEP_SIZE / 10;
    const float k = 0.1f;
    // steps
    static float slope = 1;
    static float stepsize = PO_STEP_SIZE;

    static float pi[SIZE] = {0.0, 0.0};
    static float vi[SIZE] = {0.0, 0.0};

    duty[ACTUAL] = duty[PREV];
    // Compute input power
    pi[ACTUAL] = inputs->v_p * inputs->i_p;
    vi[ACTUAL] = inputs->v_p;
    // Compute input power variation
    float dpi = pi[ACTUAL] - pi[PREV];
    // Compute input voltage variation
    float dvi = vi[ACTUAL] - vi[PREV];
    // Derivate input voltage
    float diff_vi = (vi[ACTUAL] != 0) ? (dvi / vi[ACTUAL]) : 0;
    // Compute dinamic step size
    stepsize -= PO_STEP_SIZE * k * atan(diff_vi);

    if (stepsize > stepsize_max)
        stepsize = stepsize_max;
    else if (stepsize < stepsize_min)
        stepsize = stepsize_min;

    if (dpi < 0)
    {
        slope = -slope;
    }

    duty[ACTUAL] += slope * stepsize;

    if (duty[ACTUAL] > PWM_MAX)
    {
        duty[ACTUAL] = PWM_MAX;
        slope = -slope;
    }
    else if (duty[ACTUAL] < PWM_MIN)
    {
        duty[ACTUAL] = PWM_MIN;
        slope = -slope;
    }

    // Update Prev
    pi[PREV] = pi[ACTUAL];
    vi[PREV] = vi[ACTUAL];
    duty[PREV] = duty[ACTUAL];

    // save metadata
    if (pi[ACTUAL] > metadata.absolute_mpp_power)
    {
        metadata.absolute_mpp_duty = pwm_get_duty();
        metadata.absolute_mpp_power = pi[ACTUAL];
    }

    return duty[ACTUAL];
}

const algorithms_metadata_t *peo_dynamic_step_get_metadata(void)
{
    return &metadata;
}