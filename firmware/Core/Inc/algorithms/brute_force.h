/**
 * This algorithm consist in find the maximum power point, 
 * iterating for all the possible duty cycles with a defined step and frequency
*/
#ifndef BRUTE_FORCE_H
#define BRUTE_FORCE_H

#include "control.h"

/**
 * @brief initialize the algorithm
 * @param initial_duty initial duty cycle guess 
*/
void brute_force_init(float initial_duty);

/**
 * @brief Run a iteration of the algorithm
 * @retval duty cycle control action 
*/
float brute_force_run(void);

/**
 * @brief get metadata
 * @return metadata
*/
const algorithms_metadata_t *brute_force_get_metadata(void);

#endif