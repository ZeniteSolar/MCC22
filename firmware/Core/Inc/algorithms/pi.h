/**
 * This algorithm consist in maintain a pi duty cycle
*/
#ifndef pi_H
#define pi_H

#include "control.h"

/**
 * @brief initialize the algorithm
 * @param initial_duty initial duty cycle guess 
*/
void pi_init(float initial_duty);

/**
 * @brief Run a iteration of the algorithm
 * @retval duty cycle control action 
*/
float pi_run(void);

/**
 * @brief get metadata
 * @return metadata
*/
const algorithms_metadata_t *pi_get_metadata(void);

#endif