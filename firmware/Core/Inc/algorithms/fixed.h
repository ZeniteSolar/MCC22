/**
 * This algorithm consist in maintain a fixed duty cycle
*/
#ifndef fixed_H
#define fixed_H

#include "control.h"

/**
 * @brief initialize the algorithm
 * @param initial_duty initial duty cycle guess 
*/
void fixed_init(float initial_duty);

/**
 * @brief Run a iteration of the algorithm
 * @retval duty cycle control action 
*/
float fixed_run(void);

/**
 * @brief get metadata
 * @return metadata
*/
const algorithms_metadata_t *fixed_get_metadata(void);

#endif