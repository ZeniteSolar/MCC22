/**
 * This algorithm consist in find the maximum power point, 
 * Making a perturbation and observing the behavior
*/
#ifndef PEO_H
#define PEO_H

#include "control.h"

/**
 * @brief initialize the algorithm
 * @param initial_duty initial duty cycle guess 
*/
void peo_init(float initial_duty);

/**
 * @brief Run a iteration of the algorithm
 * @retval duty cycle control action 
*/
float peo_run(void);

void peo_set_step(float step_input);

/**
 * @brief get metadata
 * @return metadata
*/
const algorithms_metadata_t *peo_get_metadata(void);

#endif