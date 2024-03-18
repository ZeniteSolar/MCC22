#ifndef MESSAGE_STATE_H
#define MESSAGE_STATE_H

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
		CAN_MSG_MCC19_1_STATE_ID,
		CAN_MSG_MCC19_2_STATE_ID,
		CAN_MSG_MCC19_3_STATE_ID,
		CAN_MSG_MCC19_4_STATE_ID,
		CAN_MSG_MCC19_5_STATE_ID,
		CAN_MSG_MCC19_6_STATE_ID,
	};

	/* Check signature */
	if (board_number > 6)
	{
		LOG_ERROR("invalid board number");
		/* Return default id */
		return CAN_MSG_MCC19_1_STATE_ID;
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
	message->mcc19_1_state.state = 0;
}

#endif //MESSAGE_STATE_H