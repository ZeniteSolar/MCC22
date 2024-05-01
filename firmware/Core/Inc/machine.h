#ifndef MACHINE_H
#define MACHINE_H

#include "stm32l4xx_hal.h"
#include "utils.h"

/*
 *  System flags
 */
typedef union
{
    struct
    {
        uint32_t enable : 1;
    };
    uint32_t all__;
} system_flags_t;

/*
 *  Error flags
 */
typedef union
{
    struct
    {
        uint32_t overcurrent_out : 1;
        uint32_t overvoltage_out : 1;
        uint32_t overcurrent_in : 1;
        uint32_t overvoltage_in : 1;
        uint32_t overheat : 1;
        uint32_t sensor_fault : 1;
        uint32_t fault : 1;
        uint32_t no_canbus : 1;
    };
    uint32_t all;
} error_flags_t;

typedef enum
{
    STATE_INITIALIZING,
    STATE_CONTACTOR,
    STATE_IDLE,
    STATE_RUNNING,
    STATE_ERROR,
} state_machine_t;

typedef struct
{
    // Trigger state machine, used to set a constant frequency
    FunctionalState run;
    // Actual state
    state_machine_t state;

    /**
     * Internal variables
     */

    system_flags_t flags;
    error_flags_t errors;

} machine_t;

/*
 *   Initialize module
 */
void machine_init(void);

/**
 *   @brief Run module, Should be called in the main loop
 *   machine_set_run() should be called to trigger the logic
 */
void machine_run(void);

/**
 * @brief Trigger to run state machine,
 * this function should be used at a fixed frequency
 */
void machine_set_run(void);

/**
 * @brief Enable the machine
 */
void machine_set_enable(FunctionalState enable);

/**
 * @brief Get machine signature
 * 
 */
uint8_t machine_get_signature(void);

#endif