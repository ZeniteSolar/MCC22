#ifndef CONF_H
#define CONF_H

// Analog Configuration
#define ANALOG_VREF 3.0f

// ADC Configuration
#define ADC_OVERSAMPLE 4
#define ADC_CHANNELS 7
#define ADC_SAMPLES 2
#define ADC_BUFFER_SIZE ADC_CHANNELS *ADC_SAMPLES



// PWM Configuration
#define PWM_MIN 0.0f
#define PWM_MAX 0.98f


// Control Configuration
#define MINIMUM_POWER 10.0f // in W


#endif