#include "adc.h"

static adc_t adc;

const float ADC_ANG_COEFFS[ADC_CHANNELS] =
    {0.00022896,
     0.00263243,
     0.00092899,
     0.00044165,
     4.65838685983877E-05,
     4.65838685983877E-05,
     4.65838685983877E-05};
const float ADC_LIN_COEFFS[ADC_CHANNELS] =
    {0.0062598,
     0.21382463,
     -0.06610585,
     0.01470944,
     4.65838685983877E-05,
     4.65838685983877E-05,
     4.65838685983877E-05};

uint8_t adc_ready = 0;
volatile inputs_t adc_avg;
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE];
volatile uint32_t adc_sum[ADC_CHANNELS];

void adc_init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim_trigger)
{
    adc.hadc = hadc;
    adc.htim_trigger = htim_trigger;
}

HAL_StatusTypeDef adc_start(void)
{
    if (HAL_ADCEx_Calibration_Start(adc.hadc, ADC_SINGLE_ENDED) != HAL_OK)
    {
        LOG_ERROR("error calibrating ADC");
        return HAL_ERROR;
    }

    if (HAL_ADC_Start_DMA(adc.hadc, (uint32_t *)adc_buffer, ADC_BUFFER_SIZE) != HAL_OK)
    {
        LOG_ERROR("error starting DMA");
        return HAL_ERROR;
    }

    if (HAL_TIM_Base_Start_IT(adc.htim_trigger) != HAL_OK)
    {
        LOG_ERROR("error starting timer 2");
        return HAL_ERROR;
    }

    return HAL_OK;
}

const volatile inputs_t *adc_get_measurements(void)
{
    return &adc_avg;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
    for (int i = (ADC_BUFFER_SIZE / 2); i < ADC_BUFFER_SIZE; i++)
    {
        adc_sum[i % ADC_CHANNELS] += adc_buffer[i];
    }

    for (int i = 0; i < ADC_CHANNELS; i++)
    {
        adc_avg.channels[i] = ADC_ANG_COEFFS[i] * (float)adc_sum[i] + ADC_LIN_COEFFS[i];
        adc_sum[i] = 0;
    }
    adc_ready = 1;
}

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    for (int i = 0; i < ADC_BUFFER_SIZE / 2; i++)
    {
        adc_sum[i % ADC_CHANNELS] += adc_buffer[i];
    }
}