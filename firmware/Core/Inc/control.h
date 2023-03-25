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
    // adc values
    const volatile inputs_t *inputs;

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

#endif