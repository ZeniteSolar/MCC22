#ifndef MESSAGE_STATE_H
#define MESSAGE_STATE_H

#include "control.h"
#include "../can_parser_types.h"
#include "../can_ids.h"

/**
 * @brief Return the id of the state message
 * 
 * @param board_number number of the board
 * @return uint32_t id of the message
 */
uint32_t canbus_get_state_id(uint8_t board_number)
{
	/* Array of ids */
	const uint32_t ids[] = {
		CAN_MSG_MCC23_1_STATE_ID,
		CAN_MSG_MCC23_2_STATE_ID,
		CAN_MSG_MCC23_3_STATE_ID,
		CAN_MSG_MCC23_4_STATE_ID,
		CAN_MSG_MCC23_5_STATE_ID,
		CAN_MSG_MCC23_6_STATE_ID,
		CAN_MSG_MCC23_7_STATE_ID,
		CAN_MSG_MCC23_8_STATE_ID,
		CAN_MSG_MCC23_9_STATE_ID,
	};

	/* Check signature */
	if (board_number > sizeof(ids) / sizeof(ids[0]))
	{
		LOG_ERROR("invalid board number");
		/* Return default id */
		return CAN_MSG_MCC23_1_STATE_ID;
	}

	/* Return id */
	return ids[board_number];
}

/**
 * @brief Callback to update state message
 * 
 * @param message pointer to the message
 */
void canbus_update_state_message(uint8_t board_number, can_msg_t *message)
{
	/* Update ID */
	message->id = canbus_get_state_id(board_number);
	/* Update message */
	message->mcc23_1_state.state = (uint8_t)control_get_algorithm();
	
	/* Get control errors */
	errors_t control_errors = control_get_error_state();
	
	message->mcc23_1_state.control.vo_safe_range = 
		!(control_errors & (OUTPUT_OVER_VOLTAGE));

	message->mcc23_1_state.control.vi_safe_range =
		!(control_errors & (INPUT_OVER_VOLTAGE));

	message->mcc23_1_state.control.vi_stable = 
		(control_get_algorithm() == PEO);

	/* Update length */
	message->dlc = CAN_MSG_MCC23_1_STATE_LENGTH;
}

#endif //MESSAGE_STATE_H