#ifndef MEASUREMENTS_H
#define MEASUREMENTS_H

#include "adc.h"
#include "pwm.h"

/**
 * @brief Return the id of the measurements message
 * 
 * @param board_number number of the board
 * @return uint32_t id of the message
 */
uint32_t canbus_get_measurements_id(uint8_t board_number)
{
	/* Array of ids */
	const uint32_t ids[] = {
		CAN_MSG_MCC19_1_MEASUREMENTS_ID,
		CAN_MSG_MCC19_2_MEASUREMENTS_ID,
		CAN_MSG_MCC19_3_MEASUREMENTS_ID,
		CAN_MSG_MCC19_4_MEASUREMENTS_ID,
		CAN_MSG_MCC19_5_MEASUREMENTS_ID,
		CAN_MSG_MCC19_6_MEASUREMENTS_ID,
	};

	/* Check signature */
	if (board_number > 6)
	{
		LOG_ERROR("invalid board number");
		/* Return default id */
		return CAN_MSG_MCC19_1_MEASUREMENTS_ID;
	}

	/* Return id */
	return ids[board_number];
}

/**
 * @brief Callback to update measurements message
 * 
 * @param board_number number of the board
 * @param message pointer to the message
 */
void canbus_update_measurements_message(uint8_t board_number, can_msg_t *message)
{
	/* Update ID */
	message->id = canbus_get_measurements_id(board_number);
	/* Update message */
	message->mcc19_1_measurements.input_voltage = 
		(uint16_t)(adc_get_value(ADC_PANEL_VOLTAGE) * 100.0f);
	message->mcc19_1_measurements.input_current = 
		(uint16_t)(adc_get_value(ADC_PANEL_CURRENT) * 100.0f);
	message->mcc19_1_measurements.output_voltage = 
		(uint16_t)(adc_get_value(ADC_BATTERY_VOLTAGE) * 100.0f);
	message->mcc19_1_measurements.dt = 
		(uint8_t)(pwm_get_duty() * 100.0f);
}

#endif // MEASUREMENTS_H
