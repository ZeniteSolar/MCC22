#ifndef CANBUS_H
#define CANBUS_H

/* STM HAL */
#include "stm32l4xx_hal.h"
/* Canbus parser types */
#include "canbus/can_parser_types.h"

/**
 * @brief Canbus main structure
 * 
 */
typedef struct {
	/* Canbus handler */
	CAN_HandleTypeDef hcan;
	/* Board distinction number */
	uint8_t self_board_number;
} canbus_t;

/**
 * @brief Canbus transmission message
 * 
 */
typedef struct {
	/* Canbus message */
	can_msg_t message;
	/* Time to send the message */
	uint32_t schedule_time;
	/* Frequency of the message */
	uint32_t frequency;
	/* Update function */
	void (*update)(can_msg_t*);
} canbus_tx_msg_t;

/**
 * @brief Init canbus controller
 * @param hcan canbus handler 
 */
void canbus_init(CAN_HandleTypeDef hcan);

/**
 * @brief Run canbus task
 * 
 */
void canbus_run(void);

#endif //CANBUS_H