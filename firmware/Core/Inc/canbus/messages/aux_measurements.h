#ifndef AUX_MEASUREMENTS_H
#define AUX_MEASUREMENTS_H

#include "adc.h"
#include "pwm.h"
#include "../can_parser_types.h"
#include "../can_ids.h"

/**
 * @brief Return the id of the aux measurements message
 * 
 * @param board_number number of the board
 * @return uint32_t id of the message
 */
uint32_t canbus_get_aux_measurements_id(uint8_t board_number)
{
	/* Array of ids */
	const uint32_t ids[] = {
		CAN_MSG_MCC23_1_AUX_MEASUREMENTS_ID,
		CAN_MSG_MCC23_2_AUX_MEASUREMENTS_ID,
		CAN_MSG_MCC23_3_AUX_MEASUREMENTS_ID,
		CAN_MSG_MCC23_4_AUX_MEASUREMENTS_ID,
		CAN_MSG_MCC23_5_AUX_MEASUREMENTS_ID,
		CAN_MSG_MCC23_6_AUX_MEASUREMENTS_ID,
		CAN_MSG_MCC23_7_AUX_MEASUREMENTS_ID,
		CAN_MSG_MCC23_8_AUX_MEASUREMENTS_ID,
		CAN_MSG_MCC23_9_AUX_MEASUREMENTS_ID,
	};
	/* Check signature */
	if (board_number > sizeof(ids) / sizeof(ids[0]))
	{
		LOG_ERROR("invalid board number, using default id");
		/* Return default id */
		return CAN_MSG_MCC23_1_AUX_MEASUREMENTS_ID;
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
void canbus_update_aux_measurements_message(uint8_t board_number, can_msg_t *message)
{
	/* Update ID */
	message->id = canbus_get_aux_measurements_id(board_number);
	/* Update message */
	message->mcc23_1_aux_measurements.output_current = 
		(uint16_t)(adc_get_value(ADC_BATTERY_CURRENT) * 100.0f);
	message->mcc23_1_aux_measurements.mosfet_temp = 0;
	message->mcc23_1_aux_measurements.diode_temp = 0;
	message->mcc23_1_aux_measurements.room_temp = 0;

	/* Update length */
	message->dlc = CAN_MSG_MCC23_1_AUX_MEASUREMENTS_LENGTH;

}

#endif // AUX_MEASUREMENTS_H
