#ifndef CONF_H
#define CONF_H

// Analog Configuration
#define ANALOG_VREF 3.0f

// ADC Configuration
#define ADC_DEFAULT_FREQUENCY 200.0f // in Hz



// PWM Configuration
#define PWM_MIN 0.0f
#define PWM_MAX 0.8f
#define DEFAULT_PWM_FREQUENCY 300e3f


// Control Configuration
#define MINIMUM_POWER 20.0f // in W
#define CONTROL_ERROR_RESET_MILLIS 3000 /* Time to wait for system stabilization when a error is triggered*/
#define CONTROL_DEFAULT_FREQUENCY  1.0f

#endif