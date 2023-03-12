#ifndef DAC_H
#define DAC_H

#include "stm32l4xx_hal.h"


/**
 * @brief Set an the DAC Output register for a given voltage
 * @param hdac DAC handler
 * @param channel DAC channel
 * @param voltage Output voltage
*/
HAL_StatusTypeDef dac_set_voltage(DAC_HandleTypeDef *hdac, uint32_t channel, float voltage);

#endif