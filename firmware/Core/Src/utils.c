#include "utils.h"
#include "main.h"

/**
 * printf write implementation
 */
extern UART_HandleTypeDef huart1;

int _write(int fd, char *ptr, int len)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
	return len;
}

void leds_init(uint8_t signature)
{
	const uint16_t LEDS_PIN[] = {LED_0_Pin, LED_1_Pin, LED_STATUS_Pin, LED_CAN_BUSY_Pin};
	GPIO_TypeDef *LEDS_PORT[] = {LED_0_GPIO_Port, LED_1_GPIO_Port, LED_STATUS_GPIO_Port, LED_CAN_BUSY_GPIO_Port};
	
	const uint32_t LEDS_SIZE = (uint32_t)(sizeof(LEDS_PIN) / sizeof(uint16_t));
	for (uint32_t i = 0; i <= 2 * LEDS_SIZE; i++)
	{
		uint32_t index = i < LEDS_SIZE ? i : 2 * LEDS_SIZE - (i);
		HAL_GPIO_WritePin(LEDS_PORT[index], LEDS_PIN[index], i <= LEDS_SIZE ? GPIO_PIN_RESET : GPIO_PIN_SET);
		HAL_Delay(100);
	}

	/* Repeat twice the animation*/
	for (uint32_t i = 0; i < 2; i++)
	{
		/* Signature identification */
		for (uint32_t j = 0; j < LEDS_SIZE; j++)
		{
			// for each bit on the signature turn on led when bit is 1 and turn off if 0
			HAL_GPIO_WritePin(
				LEDS_PORT[j],
				LEDS_PIN[j],
				((signature >> ((LEDS_SIZE - 1) - j)) & 0x1) ? GPIO_PIN_RESET : GPIO_PIN_SET);
		}

		HAL_Delay(200);

		/* Turn off leds */
		for (uint32_t j = 0; j < LEDS_SIZE; j++)
		{
			HAL_GPIO_WritePin(
				LEDS_PORT[j],
				LEDS_PIN[j],
				GPIO_PIN_RESET);
		}

		HAL_Delay(100);
	}


}