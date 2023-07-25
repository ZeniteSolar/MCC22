#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include "main.h"

/**
 * Debugging macros
 */

#define LOG_INFO(x)       \
    {                     \
        printf("INFO: "); \
        printf(x);        \
        printf("\n");      \
    }
#define LOG_WARN(x)       \
    {                     \
        printf("WARN: "); \
        printf(x);        \
        printf("\n");      \
    }
#define LOG_ERROR(x)       \
    {                      \
        printf("ERROR: "); \
        printf(x);         \
        printf("\n");       \
    }

/**
 * @brief Init leds with a fancy animation
 */
void leds_init(uint8_t signature);

#endif