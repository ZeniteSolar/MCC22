#ifndef ADC_H
#define ADC_H

#include "stm32l4xx_hal.h"
#include "conf.h"
#include "utils.h"

typedef struct
{
    union 
    {
        float channels[ADC_CHANNELS];
        struct 
        {
            float i_b; // Battery current
            float v_b; // Battery voltage
            float v_p; // Panel voltage
            float i_p; // Panel current
            float t_d; // Diode temperature
            float t_m; // Mosfet temperature
            float t_r; // Room temperature 
        };
    };
}inputs_t;

typedef struct
{
    ADC_HandleTypeDef *hadc;
    TIM_HandleTypeDef *htim_trigger;
}adc_t;

extern uint8_t adc_ready;

void adc_init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim_trigger);

HAL_StatusTypeDef adc_start(float freq);

void adc_set_freq(float freq);
/**
 * @brief returns the pointer of the measurements values 
 * @return pointer of the measurements 
*/
const volatile inputs_t *adc_get_measurements(void);

#endif