#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include "main.h"

/**
 * Debugging macros
 */

	#define LOG_INFO(...)       \
	{                     \
		printf("INFO: "); \
		printf(__VA_ARGS__);        \
		printf("\n");      \
	}
	#define LOG_WARN(...)       \
	{                     \
		printf("WARN: "); \
		printf(__VA_ARGS__);        \
		printf("\n");      \
	}
	#define LOG_ERROR(...)       \
	{                      \
		printf("ERROR: at line %d in function %s", __LINE__, __func__); \
		printf(__VA_ARGS__);         \
		printf("\n");       \
	}

/**
 * @brief Init leds with a fancy animation
 */
void leds_init(uint8_t signature);

#endif