#include "machine.h"
#include "control.h"
#include "utils.h"

static machine_t machine;
/**
 * External flags
 */
system_flags_t system_flags;
error_flags_t error_flags;


/*
 * States of the machine
 */
// Initialize the machine
static void task_initializing(void);
// In this state, is waiting for
static void task_idle(void);
// In this state, the motor is ready to run
static void task_running(void);
// Error state
static void task_error(void);

static void machine_set_state_initializing(void)
{
    control_init();
    control_set_enable(DISABLE);
    LOG_INFO("==> STATE INITIALIZING");
    machine.state = STATE_INITIALIZING;
}

static void machine_set_state_idle(void)
{
    control_set_enable(DISABLE);
    LOG_INFO("==> STATE IDLE");
    machine.state = STATE_IDLE;
}

static void machine_set_state_running(void)
{
    control_set_enable(ENABLE);
    LOG_INFO("==> STATE RUNNING");
    machine.state = STATE_RUNNING;
}
static void machine_set_state_error(void)
{
    control_set_enable(DISABLE);
    LOG_INFO("==> STATE ERROR");
    machine.state = STATE_ERROR;
}

static void task_initializing(void)
{
    machine_set_state_idle();
}

static void task_idle(void)
{

    if (machine.flags.enable)
    {
       machine_set_state_running();
    }
}

static void task_running(void)
{

    if (!machine.flags.enable)
    {
        machine_set_state_idle();
    }
}

static void task_error(void)
{
    LOG_ERROR("error");
    machine_set_state_initializing();
}

void machine_init(void)
{
    machine_set_state_initializing();
}

void machine_set_run(void)
{
    machine.run = 1;
}

void machine_set_enable(FunctionalState enable)
{
    machine.flags.enable = enable;
}

void machine_run(void)
{

    if (machine.run)
    {
        machine.run = 0;

        switch (machine.state)
        {
        case STATE_INITIALIZING:
            task_initializing();
            break;
        case STATE_IDLE:
            task_idle();
            break;
        case STATE_RUNNING:
            task_running();
            break;
        default:
            LOG_WARN("Unrecognized machine state!");
        case STATE_ERROR:
            task_error();
            break;
        }
        system_flags = machine.flags;
        error_flags = machine.errors;
    }
}