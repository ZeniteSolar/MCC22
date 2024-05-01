#include "canbus.h"
#include "utils.h"
#include "machine.h"
/* Can lib */
#include "canbus/can_ids.h"
/* Can messages */
#include "canbus/messages/state.h"
#include "canbus/messages/measurements.h"


static canbus_t canbus;

void canbus_init(CAN_HandleTypeDef *hcan)
{
	/** Save can handler */
	canbus.hcan = hcan;
	canbus.self_board_number = machine_get_signature();
	HAL_CAN_Start(canbus.hcan);
}

uint32_t canbus_get_signature(uint8_t board_number)
{
	/* Signatures */
	const uint8_t signatures[] = {
		CAN_SIGNATURE_MCC19_1,
		CAN_SIGNATURE_MCC19_2,
		CAN_SIGNATURE_MCC19_3,
		CAN_SIGNATURE_MCC19_4,
		CAN_SIGNATURE_MCC19_5,
		CAN_SIGNATURE_MCC19_6,
	};

	/* Check signature */
	if (board_number > 6)
	{
		LOG_ERROR("invalid board number");
		/* Return default signature */
		return CAN_SIGNATURE_MCC19_1;
	}
	
	/* Return signature */
	return signatures[board_number];
}

void canbus_send(canbus_tx_msg_t *message)
{
	/* Update message with board signature */
	message->message.generic_generic.signature = canbus_get_signature(canbus.self_board_number);

	/* Create message header */
	CAN_TxHeaderTypeDef txHeader;
	/* Message ID */
	txHeader.StdId = message->message.id;
	/* Message type */
	txHeader.IDE = CAN_ID_STD;
	/* Message format */
	txHeader.RTR = CAN_RTR_DATA;
	/* Message length */
	txHeader.DLC = message->message.dlc;
	/* Transmit global time */
	txHeader.TransmitGlobalTime = DISABLE;

	/* Send message */
	uint32_t mailbox;
	HAL_CAN_AddTxMessage(canbus.hcan, &txHeader, message->message.raw, &mailbox);
}

/**
 * @brief Send canbus messages
 * 
 */
void canbus_send_messages(void)
{
	/* Initialize messages */
	static canbus_tx_msg_t messages[] = {
		{
			.schedule_time = 0,
			.frequency = CAN_MSG_MCC19_1_MEASUREMENTS_FREQUENCY,
			.update = canbus_update_measurements_message,
		},
		{
			.schedule_time = 0,
			.frequency = CAN_MSG_GENERIC_STATE_FREQUENCY,
			.update = canbus_update_state_message,
		}
	};

	/* Loop through messages */
	for (int i = 0; i < sizeof(messages) / sizeof(canbus_tx_msg_t); ++i)
	{
		/* Check if it is time to send the message */
		if (messages[i].schedule_time <= HAL_GetTick())
		{
			/* Update message */
			messages[i].update(
				canbus.self_board_number,
				&messages[i].message
			);

			/* Schedule next message */
			messages[i].schedule_time = HAL_GetTick() + (1000.0f / (float)messages[i].frequency);

			/* Send message */
			canbus_send(&messages[i]);
		}
	}
}

void canbus_run(void)
{
	canbus_send_messages();
}