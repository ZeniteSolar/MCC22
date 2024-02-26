#include "canbus.h"
#include "adc.h"
#include "pwm.h"
#include "machine.h"
#include "canbus/can_ids.h"

static canbus_t canbus;

void canbus_init(CAN_HandleTypeDef hcan)
{
	/** Save can handler */
	canbus.hcan = hcan;
	canbus.self_board_number = machine_get_signature();
}

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
		return 0;

	/* Return id */
	return ids[board_number];
}

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
		return 0;

	/* Return id */
	return ids[board_number];
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
		return 0;
	
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
	HAL_CAN_AddTxMessage(&canbus.hcan, &txHeader, message->message.raw, &mailbox);
}

/**
 * @brief Callback to update measurements message
 * 
 * @param message pointer to the message
 */
void canbus_update_measurements_message(can_msg_t *message)
{
	/* Update ID */
	message->id = canbus_get_measurements_id(canbus.self_board_number);
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

/**
 * @brief Callback to update state message
 * 
 * @param message pointer to the message
 */
void canbus_update_state_message(can_msg_t *message)
{
	/* Update ID */
	message->id = canbus_get_state_id(canbus.self_board_number);
	/* Update message */
	message->mcc19_1_state.state = 0;
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
			canbus_update_measurements_message(
				&messages[i].message
			);

			/* Send message */
			canbus_send(&messages[i]);
		}
	}
}

void canbus_run(void)
{
	canbus_send_messages();
}