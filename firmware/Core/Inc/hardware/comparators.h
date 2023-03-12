#ifndef COMPARATORS_H
#define COMPARATORS_H

#include "stm32l4xx_hal.h"


/**
 * @brief init comparator using DAC in the minus input for battery overvoltage protection
 * @param hcomp comparator handler
 * @param hdac DAC for the minus input 
 * @param dac_channel Channel for the minus input 
*/
void comparator_init_dac(COMP_HandleTypeDef *hcomp, DAC_HandleTypeDef *hdac, uint32_t dac_channel, float voltage);

/**
 * @brief init comparator using an extern minus input for battery overvoltage protection
 * @param hcomp comparator handler
 * @param hdac DAC for the minus input 
 * @param dac_channel Channel for the minus input 
*/
void comparator_init_extern(COMP_HandleTypeDef *hcomp, DAC_HandleTypeDef *hdac, uint32_t dac_channel);

#endif