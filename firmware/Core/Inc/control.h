#ifndef CONTROL_H
#define CONTROL_H

#include "stm32l4xx_hal.h"
#include "adc.h"
#include "conf.h"
#include "pwm.h"

typedef enum
{
	// Perturb and Observe
	PEO,
	// Perturb and Observe with dynamic step
	PEO_DYN_STEP,
	// Brute Force
	BRUTE_FORCE,
	// Proportional Integrator
	PI,
	// Fixed duty cycle
	FIXED,
} algorithms_t;

typedef struct
{
	// absolute maximum power
	float absolute_mpp_power;
	// absolute maximum power duty cycle
	float absolute_mpp_duty;
	// if the algorithm is finite should be true when is done
	uint32_t done;

} algorithms_metadata_t;


typedef enum {
	INPUT_OVER_VOLTAGE = (1U << 0),
	OUTPUT_OVER_VOLTAGE = (1U << 1),
	INPUT_OVER_CURRENT = (1U << 2),
	OUTPUT_OVER_CURRENT = (1U << 3),
	INPUT_OVER_POWER = (1U << 4),
	OUTPUT_OVER_POWER = (1U << 5),
} errors_t;



typedef struct
{
	FunctionalState enable;
	// Actual state
	algorithms_t algorithm_running;
	/**
	 * Internal variables
	 */
	// if algorithm is in follower mode
	FunctionalState forced_algorithm;

	/* Control period in ms */
	uint32_t period; 

	errors_t errors;

} control_t;

/**
 * @brief Initialize control algorithms
 */
void control_init(void);

/**
 * Run control algorithms
 */
void control_run(void);

/**
 * @brief Force an algorithm
 *
 * @param algorithm the algorithm to use
 * @param initial_duty initial duty cycle
 */
void control_force_algorithm(algorithms_t algorithm, float initial_duty);

/**
 * @brief get the actual algorithm
 *
 */
algorithms_t control_get_algorithm(void);

/**
 * @brief enable/disable control.
 * set duty cycle to 0 if disabled.
 * start control if enabled.
 */
void control_set_enable(FunctionalState enable);

/**
 * @brief Trigger an hardware error 
 * @param error: triggered error
 * 
 */
void control_set_error(errors_t error);

/**
 * @brief  Set control frequency
 * 
 * @param freq control frequency in Hz
 */
float control_get_freq(void);

/**
 * @brief  Set control frequency
 * 
 * @param freq control frequency in Hz
 */
void control_set_freq(float freq);

/**
 * @brief  Set control print
 * 
 * @param enable enable/disable print
 */
void control_set_print(uint8_t enable);

/**
 * @brief  Set control period
 * 
 * @param period control period in ms
 */
void control_set_period(uint32_t period);

/**
 * @brief Get control period
 * 
 * @return uint32_t control period in ms
 */
uint32_t control_get_period(void);

#endif