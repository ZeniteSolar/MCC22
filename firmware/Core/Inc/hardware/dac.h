#ifndef DAC_H
#define DAC_H

#include "stm32l4xx_hal.h"
/**
 * @brief Dac initializer
 * 
 * @param hdac dac handler
 * @param channel channel to init
 * @return HAL_StatusTypeDef status
 */
HAL_StatusTypeDef dac_init(DAC_HandleTypeDef *hdac);

/**
 * @brief Returns last initialized handler 
 * 
 * @return DAC_HandleTypeDef* 
 */
DAC_HandleTypeDef *dac_get_handler(void);

/**
 * @brief Set an the DAC Output register for a given voltage
 * @param hdac DAC handler
 * @param channel DAC channel
 * @param voltage Output voltage
*/
HAL_StatusTypeDef dac_set_voltage(DAC_HandleTypeDef *hdac, uint32_t channel, float voltage);

#endif