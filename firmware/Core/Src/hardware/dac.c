
#include "dac.h"
#include "conf.h"

HAL_StatusTypeDef dac_set_voltage(DAC_HandleTypeDef *hdac, uint32_t channel, float voltage)
{
	/* 
		DOR = DAC_OUT * 4095 / VREF+
		DOR is the Data Ouput Register
		DAC_OUT is the output voltage
		Vref+ is the Analog supply voltage
		For more information see Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dac.c
	*/

	if (voltage > ANALOG_VREF)
		voltage = ANALOG_VREF;
	
	if (voltage < 0.0)
		voltage = 0;
	
	uint32_t output_register = voltage * 4095 / ANALOG_VREF;

	return HAL_DAC_SetValue(hdac, channel, DAC_ALIGN_12B_R, (uint32_t)output_register);

}