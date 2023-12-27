#ifndef ADC_H
#define ADC_H

/* ADS111x Driver*/
#include "ads111x.h"

/**
 * @brief ADC channels in order of the ADC multiplexer
 * 
 */
typedef enum {
	ADC_PANEL_CURRENT,
	ADC_PANEL_VOLTAGE,
	ADC_BATTERY_CURRENT,
	ADC_BATTERY_VOLTAGE,
	ADC_CHANNELS_SIZE
} adc_channels_t;

/**
 * @brief ADC average structure
 * 
 */
typedef struct
{
	/* Sum of samples */
	float sum;
	/* Number of samples */
	uint32_t samples;
	/* Average value */
	float avg;

} adc_average_t;

/**
 * @brief ADC channel structure
 * 	Contains gain and value
 */
typedef struct
{
	/* Channel gain */
	ads111x_gain_t gain;
	/* Channel value */
	adc_average_t value;

} adc_channel_t;

/**
 * @brief ADC structure
 * 	Contains I2C handler and channels
 */
typedef struct
{
	/* I2C Handler */
	I2C_HandleTypeDef *hi2c;
	/* ADC channels */
	adc_channel_t channels[ADC_CHANNELS_SIZE];
	/* Last channel read */
	uint8_t last_channel_index;
	/* ADC all channels measured */
	uint8_t all_channels_measured;
} adc_t;

/**
 * @brief Initialize ADC
 * 
 * @param hi2c I2C handle
 * @return HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef adc_init(I2C_HandleTypeDef *hi2c);

/**
 * @brief Measure a channel
 * 
 * @return HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef adc_measure(void);

/** 
 * @brief Calculate average of all channels
*/
HAL_StatusTypeDef adc_calculate_average(void);

/**
 * @brief Get channel value
 * 
 * @param channel Channel
 * @return float Value
 */
float adc_get_value(adc_channels_t channel);

/**
 * @brief ADC all channels measured
 * 
 */
uint8_t adc_all_channels_measured(void);

#endif // ADC_H