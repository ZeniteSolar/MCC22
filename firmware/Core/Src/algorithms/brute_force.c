/**
 * This algorithm consist in find the maximum power point, 
 * iterating for all the possible duty cycles with a defined step and frequency
*/
#include "brute_force.h"
#include "adc.h"
#include "math.h"

// Brute Force configuration 
#define CONTROL_BRUTE_FORCE_FREQUENCY 10.0f
#define BRUTE_FORCE_PWM_MAX 0.8f
#define BRUTE_FORCE_PWM_MIN 0.5f

static algorithms_metadata_t metadata;
static float actual_duty = 0.0;
static uint8_t done; /* Internal done */


void brute_force_init(float initial_duty)
{
	control_set_freq(CONTROL_BRUTE_FORCE_FREQUENCY);
	metadata.absolute_mpp_duty = 0.0;
	metadata.absolute_mpp_power = 0.0;
	metadata.done = 0;
	done = 0;
	actual_duty = initial_duty;
}

float brute_force_run(void)
{
	static float step = 0.03;
	float pi = adc_get_value(ADC_PANEL_VOLTAGE) * adc_get_value(ADC_PANEL_CURRENT);
	
	if (pi > metadata.absolute_mpp_power)
	{
		metadata.absolute_mpp_power = pi;
		metadata.absolute_mpp_duty = pwm_get_duty();
	}
	
/** When sweep is done go to mppt point*/

	if (!done)
		actual_duty += step;
	else
	{
		if (actual_duty > metadata.absolute_mpp_duty)
		{
			actual_duty -= step;
		}
		else
		{
			printf("Brute force done!\n");
			metadata.done = 1;
		}
	}

	if (actual_duty > BRUTE_FORCE_PWM_MAX)
	{
		actual_duty = BRUTE_FORCE_PWM_MAX;
		done = 1;
	}

	return actual_duty;
}

const algorithms_metadata_t *brute_force_get_metadata(void)
{
	return &metadata;
}