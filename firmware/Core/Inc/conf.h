#ifndef CONF_H
#define CONF_H

// Analog Configuration
#define ANALOG_VREF 3.0f

// ADC Configuration
#define ADC_OVERSAMPLE 4
#define ADC_CHANNELS 7
#define ADC_SAMPLES 9
#define ADC_BUFFER_SIZE ADC_CHANNELS *ADC_SAMPLES



// PWM Configuration
#define PWM_MIN 0.0f
#define PWM_MAX 0.8f


// Control Configuration
#define MINIMUM_POWER 1.0f // in W
#define CONTROL_ERROR_RESET_MILLIS 3000 /* Time to wait for system stabilization when a error is triggered*/

// ADC default frequency
#define ADC_TIM_FREQ 80e6 //ADC timer clock frequency
#define DEFAULT_ADC_FREQ 800.0f


#endif