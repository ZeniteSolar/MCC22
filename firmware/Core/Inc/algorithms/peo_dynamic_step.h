/**
 * This algorithm consist in find the maximum power point, 
 * Making a perturbation and observing the behavior
*/
#ifndef PEO_DYNAMIC_STEP_H
#define PEO_DYNAMIC_STEP_H

#include "control.h"

/**
 * @brief initialize the algorithm
 * @param initial_duty initial duty cycle guess 
*/
void peo_dynamic_step_init(float initial_duty);

/**
 * @brief Run a iteration of the algorithm
 * @retval duty cycle control action 
*/
float peo_dynamic_step_run(void);

/**
 * @brief get metadata
 * @return metadata
*/
const algorithms_metadata_t *peo_dynamic_step_get_metadata(void);

#endif