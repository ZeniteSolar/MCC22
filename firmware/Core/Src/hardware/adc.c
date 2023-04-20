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

HAL_StatusTypeDef adc_start(float freq)
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
    adc_set_freq(freq);

    return HAL_OK;
}

void adc_set_freq(float freq)
{
    
    /**
     * Frequency = ADC_TIM_FREQ / (2 * period * prescaler)
     * for better resolution period should be great as possible
     */

    float trigger_freq = freq * ADC_SAMPLES;

    // Calculate period for the required frequency assuming prescaler as 1
    uint32_t period = ADC_TIM_FREQ / (2 * trigger_freq);

    if (period > 65535)
        period = 65535; // 16bit period

    // if period was not enough to achieve the frequency, calculate the prescaler
    uint32_t prescaler = ADC_TIM_FREQ / (2 * trigger_freq * period);

    if (prescaler > 65535)
        prescaler = 65535;

    __HAL_TIM_SetAutoreload(adc.htim_trigger, period);
    __HAL_TIM_SET_PRESCALER(adc.htim_trigger, prescaler);

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

