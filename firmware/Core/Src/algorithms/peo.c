#include "peo.h"
#include "math.h"
#include "stdlib.h"

static algorithms_metadata_t metadata;
#define CONTROL_PEO_FREQUENCY 1000.0f
#define CONTROL_PEO_STEP 0.005f
#define PEO_PWM_MAX 0.78f
#define PEO_PWM_MIN 0.51f

static float step = CONTROL_PEO_STEP;
static float dir = 1;
static float duty;

enum observations{
	PREV = 0,
	ACTUAL = 1,
	SIZE
};

void peo_init(float initial_duty)
{
	control_set_freq(CONTROL_PEO_FREQUENCY);
	step = CONTROL_PEO_STEP;
	metadata.absolute_mpp_duty = 0;
	metadata.absolute_mpp_power = 0;
	metadata.done = 0;

	duty = initial_duty;
}

void peo_set_step(float step_input)
{
	printf("step before: %f", step);
	step = step_input;
	printf("step after: %f\n", step);

}

float peo_run(const volatile inputs_t *inputs)
{
	
	static float pi[SIZE] = {0.0, 0.0};
	static float vi[SIZE] = {0.0, 0.0};
	

	// Compute input power
	vi[ACTUAL] = inputs->v_p;
	pi[ACTUAL] = vi[ACTUAL] * inputs->i_p;

	// Compute input power variation
	float dpi = (pi[ACTUAL] - pi[PREV]);

	if (dpi <= -2.0){
		dir = -dir;
	}

	pi[PREV] = pi[ACTUAL];
	vi[PREV] = vi[ACTUAL];

	// save metadata
	if (pi[ACTUAL] > metadata.absolute_mpp_power)
	{
		metadata.absolute_mpp_duty = pwm_get_duty();
		metadata.absolute_mpp_power = pi[ACTUAL];
	}

	duty += (dir * step);

	if (duty > PEO_PWM_MAX)
		duty = PEO_PWM_MAX - (3 * step);

	if (duty < PEO_PWM_MIN)
		duty = PEO_PWM_MIN + (3 * step);

	return duty;
}

const algorithms_metadata_t *peo_get_metadata(void)
{
	return &metadata;
}