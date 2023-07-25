#include <stdio.h>
#include "utils.h"
#include "pwm.h"
#include "control.h"
#include "comparators.h"
#include "main.h"
#include "dac.h"

void comparator_init_dac(COMP_HandleTypeDef *hcomp, DAC_HandleTypeDef *hdac, uint32_t dac_channel, float voltage)
{
	// Start DAC and set the output voltage
	HAL_DAC_Start(hdac, dac_channel);
	dac_set_voltage(hdac, dac_channel, voltage);

	// Start compator
	HAL_COMP_Start(hcomp);
}

void comparator_init_extern(COMP_HandleTypeDef *hcomp, DAC_HandleTypeDef *hdac, uint32_t dac_channel)
{
	// Start compator
	HAL_COMP_Start(hcomp);
}

/**
 * @brief  Comparator trigger callback.
 * @param  hcomp  COMP handle
 */
void HAL_COMP_TriggerCallback(COMP_HandleTypeDef *hcomp)
{
	if (hcomp->Instance == COMP1)
	{
		printf("COMP1\n");

	}
	if (hcomp->Instance == COMP2)
	{
		if (HAL_COMP_GetOutputLevel(hcomp))
		{
			printf("COMP2\n");
			pwm_set_duty(0.0);
			
			control_set_error(OUTPUT_OVER_VOLTAGE);

		}
	}
}