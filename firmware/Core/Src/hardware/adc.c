/* Stm32 HAL */
#include "stm32l4xx_hal.h"
/* ADS111x Driver*/
#include "ads111x.h"
/* Utils */
#include "utils.h"
/* ADC header */
#include "adc.h"

/**
 * @brief ADC correction coefficients
 *  Linearization equation: 
 *  y = ADC_CORRECTION_COEFFICIENTS[0] + ADC_CORRECTION_COEFFICIENTS[1] * x
 * 
 */
const static float ADC_CORRECTION_COEFFICIENTS[ADC_CHANNELS_SIZE][2] = {
	{-0.0014958, 4.659992f},
	{-0.133195, 10.16618f},
	{0.0330273983f, 2.3719258767660f},
	{0.12551552f, 30.30599253f}
};

const static uint8_t ADC_CHANNELS[] = {
	ADS111X_MUX_0_GND,
	ADS111X_MUX_1_GND,
	ADS111X_MUX_2_GND,
	ADS111X_MUX_3_GND,
};

/**
 * @brief ADC structure
 * 
 */
adc_t adc;


/**
 * @brief Initialize ADC
 * 
 * @param hi2c I2C handle
 * @return HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef adc_init(I2C_HandleTypeDef *hi2c)
{
	/* Check if device is ready */
	if (HAL_I2C_IsDeviceReady(hi2c, (uint16_t)(ADS111X_ADDR_GND << 1), 10, 100) != HAL_OK)
	{
		return HAL_ERROR;
	}

	/* Save I2C handler */
	adc.hi2c = hi2c;

	/* Initialize in single mode */
	if (ads111x_set_mode(hi2c, ADS111X_MODE_SINGLE_SHOT) != HAL_OK)
	{
		LOG_ERROR("ads111x_set_mode");
	}

	/* Initialize data rate */
	if (ads111x_set_data_rate(hi2c, ADS111X_DATA_RATE_860) != HAL_OK)
	{
		LOG_ERROR("ads111x_set_data_rate");
	}

	/* Initialize on channel 0 */
	if (ads111x_set_input_mux(hi2c, ADS111X_MUX_0_GND) != HAL_OK)
	{
		LOG_ERROR("ads111x_set_input_mux");
	}
	/* Save last channel */
	adc.last_channel_index = 0;

	/* Initialize gain */
	if (ads111x_set_gain(hi2c, ADS111X_GAIN_4V096) != HAL_OK)
	{
		LOG_ERROR("ads111x_set_gain");
	}
	/* Start conversion */
	if (ads111x_start_conversion(hi2c) != HAL_OK)
	{
		LOG_ERROR("ads111x_start_conversion");
	}

	/* Initialize channels */
	for (uint8_t i = 0; i < ADC_CHANNELS_SIZE; i++)
	{
		adc.channels[i].gain = ADS111X_GAIN_4V096;
		adc.channels[i].value.samples = 0;
		adc.channels[i].value.sum = 0;
		adc.channels[i].value.avg = 0;
	}	

	/* Adc initialized correctly */
	return HAL_OK;

}

/**
 * @brief Set ADC channel gain
 * 
 */
HAL_StatusTypeDef adc_set_gain(adc_channels_t channel, uint16_t value)
{
	const uint16_t LOW_TRESHOLD = (ADS111X_MAX_VALUE / 2);
	const uint16_t HIGH_TRESHOLD = ADS111X_MAX_VALUE;
	const uint16_t HYSTERESIS = ADS111X_MAX_VALUE / 50;
	

	/* Check if channel is valid */
	if (channel >= ADC_CHANNELS_SIZE)
	{
		LOG_ERROR("Invalid channel");
		return HAL_ERROR;
	}

	/* if value is near of top reduce gain */
	ads111x_gain_t gain = adc.channels[channel].gain;
	if (value > (ADS111X_MAX_VALUE - HYSTERESIS) && gain != ADS111X_GAIN_4V096)
	{
		gain--;
		printf("Setting gain of channel %d: %f\n",channel , ads111x_gain_values[gain]);
	}
	else if (value < (LOW_TRESHOLD - HYSTERESIS) && gain != ADS111X_GAIN_0V256)
	{
		gain++;
		printf("Setting gain of channel %d: %f\n",channel , ads111x_gain_values[gain]);
	}



	/* Set gain */
	adc.channels[channel].gain = gain;

	return HAL_OK;
}

/**
 * @brief Measure a channel
 * 
 * @return HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef adc_measure(void)
{
	/* If conversion available */
	uint8_t busy;
	
	if (ads111x_is_busy(adc.hi2c, &busy) != HAL_OK)
	{
		LOG_ERROR("ads111x_failed reading busy flag");
		return HAL_ERROR;
	}

	if (busy)
	{
		printf("busy\n");
		return HAL_OK;
	}

	/* Read conversion result */
	int16_t value;
	if (ads111x_get_value(adc.hi2c, &value) != HAL_OK)
	{
		LOG_ERROR("ads111x_get_value");
		return HAL_ERROR;
	}
	/* Convert to voltage */
	float voltage = (float)value * (ads111x_gain_values[adc.channels[adc.last_channel_index].gain] / ADS111X_MAX_VALUE);

	/* Sum the sample */
	adc.channels[adc.last_channel_index].value.sum += voltage;
	adc.channels[adc.last_channel_index].value.samples++;

	adc_set_gain(adc.last_channel_index, value);

	/* Read next channel */
	adc.last_channel_index = (adc.last_channel_index + 1) % ADC_CHANNELS_SIZE;

	/* If all channels are measured set flag */
	if (adc.last_channel_index == 0)
	{
		adc.all_channels_measured = 1;
	}

	/* Select next channel */
	if (ads111x_set_input_mux(adc.hi2c, ADC_CHANNELS[adc.last_channel_index]) != HAL_OK)
	{
		LOG_ERROR("ads111x_set_input_mux");
		return HAL_ERROR;
	}

	/* Select gain of the channel */
	if (ads111x_set_gain(adc.hi2c, adc.channels[adc.last_channel_index].gain) != HAL_OK)
	{
		LOG_ERROR("ads111x_set_gain");
		return HAL_ERROR;
	}

	/* Start conversion */
	if (ads111x_start_conversion(adc.hi2c) != HAL_OK)
	{
		LOG_ERROR("ads111x_start_conversion");
		return HAL_ERROR;
	}

	return HAL_OK;
}

/** 
 * @brief Calculate average of all channels
*/
void adc_calculate_average(void)
{
	/* Iterate over channels */
	for (uint8_t i = 0; i < ADC_CHANNELS_SIZE; i++)
	{
		/* Division by zero protection */
		if (adc.channels[i].value.samples == 0)
		{
			continue;
		}
		
		/* Calculate average */
		adc.channels[i].value.avg = adc.channels[i].value.sum / adc.channels[i].value.samples;
		
		/* Reset sum and samples */
		adc.channels[i].value.sum = 0;
		adc.channels[i].value.samples = 0;
	}
}

/**
 * @brief Get channel value
 * 
 * @param channel Channel
 * @return float Value
 */
float adc_get_value(adc_channels_t channel)
{
	/* Check if channel is valid */
	if (channel >= ADC_CHANNELS_SIZE)
	{
		LOG_ERROR("Invalid channel");
		return 0;
	}

	float value = adc.channels[channel].value.avg;

	/* Apply correction coefficients */
	value = ADC_CORRECTION_COEFFICIENTS[channel][1] * value + ADC_CORRECTION_COEFFICIENTS[channel][0];

	/* Get value */
	return value;

}

/**
 * @brief ADC all channels measured
 * 
 */
uint8_t adc_all_channels_measured(void)
{
	uint8_t all_channels_measured = adc.all_channels_measured;
	adc.all_channels_measured = 0;
	return all_channels_measured;
}