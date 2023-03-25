#include "uart.h"
#include "adc.h"
#include "pwm.h"
#include "control.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define STR_2_UINT32(STR) *(const uint32_t *)STR
#define VARIABLE 0
#define UART_BUFFER_SIZE 32

/**
 * Helper macro for make a array of nodes
 */
#define MAKE_NODES(NAME, ...)                         \
    const node_t NAME##_NODE[] =                      \
        __VA_ARGS__;                                  \
    const nodes_t NAME = {                            \
        .nodes = NAME##_NODE,                         \
        .size = sizeof(NAME##_NODE) / sizeof(node_t), \
    }

/**
 * Helper macro for crate parsing nodes
 * COMMAND is the command
 * SEPARATOR is the separating char
 * CALLBACK is the callback function
 * NEXT is the next node
 */
#define MAKE_NODE(COMMAND, SEPARATOR, CALLBACK, NEXT) \
    {                                                 \
        .value.string = STR_2_UINT32(COMMAND),        \
        .value.separator = SEPARATOR,                 \
        .callback = CALLBACK,                         \
        .next = NEXT,                                 \
    }

uint8_t data_uart[UART_BUFFER_SIZE];

typedef enum
{
    DOT,
    COLON,
    SPACE,
    QUESTION,
    PERCENT,
    UNDEFINED,
} separator_t;
typedef struct
{
    union
    {
        uint32_t string;
        uint8_t chars[4];
    };
    separator_t separator;
} command_t;

typedef struct parser_t
{
    /* Index of the start of the parse */
    const uint32_t initial_index;
    /* Index of the consumed data */
    uint32_t actual_index;
    /* Index of the last value that can be consumed */
    const uint32_t end_index;
} parser_t;

typedef struct node_t
{
    const command_t value;
    void const (*callback)(struct parser_t *parser, command_t *command);
    const struct nodes_t *next;
} node_t;

typedef struct nodes_t
{
    const int size;
    const node_t *nodes;
} nodes_t;

UART_HandleTypeDef *huart;

command_t get_command(parser_t *parser);
int get_command_int(parser_t *parser, command_t *command);
float get_command_float(parser_t *parser, command_t *command);

void uart_init(UART_HandleTypeDef *huart_rx)
{
    HAL_UART_Receive_DMA(huart_rx, data_uart, UART_BUFFER_SIZE);
    huart = huart_rx;
}

void parse_read_meas_vbat(parser_t *parser, command_t *command)
{
    printf("%f\n", adc_get_measurements()->v_b);
}

void parse_read_meas_vpan(parser_t *parser, command_t *command)
{
    printf("%f\n", adc_get_measurements()->v_p);
}

void parse_read_meas_ibat(parser_t *parser, command_t *command)
{
    printf("%f\n", adc_get_measurements()->i_b);
}

void parse_read_meas_ipan(parser_t *parser, command_t *command)
{
    printf("%f\n", adc_get_measurements()->i_p);
}

void parse_read_duty_cycle(parser_t *parser, command_t *command)
{
    printf("%f\n", pwm_get_duty());
}

void parse_read_frequency(parser_t *parser, command_t *command)
{
    printf("%f\n", pwm_get_freq());
}

void parse_read_meas_allm(parser_t *parser, command_t *command)
{
    for (int i = 0; i < sizeof(inputs_t) / sizeof(float); i++)
    {
        printf("%f, ", adc_get_measurements()->channels[i]);
    }
    printf("\n");
}

void parse_read_control_algorithm(parser_t *parser, command_t *command)
{

    printf("%d\n", control_get_algorithm());
}

void parse_write_duty(parser_t *parser, command_t *command)
{
    float duty = get_command_float(parser, command);
    pwm_set_duty(duty);
    printf("writing duty %f\n", duty);
}

void parse_write_freq(parser_t *parser, command_t *command)
{
    /* In kHz*/
    float freq = get_command_float(parser, command);
    pwm_set_freq(freq * 1e3);
    printf("writing freq %f\n", freq * 1e3);
}

void parse_write_machine_state(parser_t *parser, command_t *command)
{
    int state = get_command_int(parser, command);
    printf("writing %d state\n", state);
}

void parse_write_control_algorithm_fixed(parser_t *parser, command_t *command)
{
    float initial_duty = get_command_float(parser, command);
    control_force_algorithm(FIXED, initial_duty);
    printf("writing fixed algorithm %f\n", initial_duty);
}

void parse_write_control_algorithm_peo(parser_t *parser, command_t *command)
{
    float initial_duty = get_command_float(parser, command);
    control_force_algorithm(PEO, initial_duty);
    printf("writing peo algorithm %f\n", initial_duty);
}

void parse_write_control_algorithm_brute_force(parser_t *parser, command_t *command)
{
    float initial_duty = get_command_float(parser, command);
    control_force_algorithm(BRUTE_FORCE, initial_duty);
    printf("writing brute force algorithm %f\n", initial_duty);
}


/**
 * @brief Get the int value of a command XXXX%
 * 
 * @param parser parser object that have buffer consumption info
 * @param command the first command (first 4 chars)
 * @return int value
 */
int get_command_int(parser_t *parser, command_t *command)
{
    int value = 0;
    command_t reading_command = *command;
    for (int i = 0; i < 4; i++)
    {
        value += (int)((reading_command.chars[i] - '0') * (powf(10, (3 - i))));
    }

    return value;
}

/**
 * @brief Get the float value of a command XXXX.XXXX
 * 
 * @param parser parser object that have buffer consumption info
 * @param command the first command (first 4 chars)
 * @return float value
 */
float get_command_float(parser_t *parser, command_t *command)
{
    float value = 0;
    command_t reading_command = *command;
    value += get_command_int(parser, &reading_command);

    if (reading_command.separator == DOT)
    {
        reading_command = get_command(parser);
        value += get_command_int(parser, &reading_command) / 10000.0f;
    }

    return value;
}

/**
 * @brief Return the command string and separator of the DMA data
 *
 * @param parser parser object that have buffer consumption info
 * @return command_t the command
 */
command_t get_command(parser_t *parser)
{
    command_t command;
    /* Read command (command has 4 chars)*/
    for (int i = 0; i < 4; i++)
    {
        if (parser->actual_index == parser->end_index)
        {
            return command;
        }
        command.chars[i] = data_uart[(parser->actual_index++) % UART_BUFFER_SIZE];
    }
    /* Read separator */
    switch (data_uart[(parser->actual_index++) % UART_BUFFER_SIZE])
    {
    case ':':
        command.separator = COLON;
        break;
    case ' ':
        command.separator = SPACE;
        break;
    case '.':
        command.separator = DOT;
        break;
    case '?':
        command.separator = QUESTION;
        break;
    case '%':
        command.separator = PERCENT;
        break;
    default:
        command.separator = UNDEFINED;
        break;
    }
    return command;
}

void leaf_node_parser(parser_t *parser, command_t *command, const node_t *node)
{
    if (node->callback != NULL)
        node->callback(parser, command);
}

const nodes_t *node_parser(parser_t *parser, command_t *command, const nodes_t *nodes)
{
    const node_t *node = nodes->nodes;
    for (int i = 0; i < nodes->size; i++)
    {
        /* Commands with dot and percent separator are variable*/
        if (command->separator == DOT || command->separator == PERCENT)
        {
            if (node[i].callback != NULL)
                node[i].callback(parser, command);
            return NULL;
        }

        if (command->string == node[i].value.string)
        {
            if (node[i].callback != NULL)
                node[i].callback(parser, command);
            /* Commands with question separator doesn't have next command value */
            if (command->separator == QUESTION && command->separator == node[i].value.separator)
            {
                if (node[i].next != NULL)
                {
                    leaf_node_parser(parser, command, node[i].next->nodes);
                }
                return NULL;
            }

            return node[i].next;
        }
    }
    printf("Unrecognized command\n");
    return NULL;
}

void uart_parse(uint32_t last_index, uint32_t index_diff)
{
    /* Make the parser */
    parser_t parser = {
        .initial_index = last_index,
        .actual_index = last_index,
        .end_index = last_index + index_diff,
    };

    /**
     * Sample command: XXXX:XXXX:XXXX:0000.0000
     * Command should have 4 chars + separator
     * Separators:
     * '.': Float value
     * '%': Int value
     * '?': Respose
     * ':': Command
     * 
     * Commands:
     * READ:
     *      MEAS: MEASurements
     *            VBAT? Get Battery Voltage
     *            IBAT? Get Battery Current
     *            VPAN? Get Panel Voltage
     *            IPAN? Get Panel Current
     *            ALLM? Get All Measurements
     *
     * WRTE:
     *      HARD: HARDware
     *          DUTY:0000.0000 Write PWM DUTY cycle
     *          FREQ:0000.0000 Write FREQuency in kHz
     *      MACH: MACHine
     *          STAT:XXX0% Force machine state
     *      CTRL: ConTRoL
     *          ALGO: Force Control Algorithm
     *              FIXD:0000.0000  Change to fixed duty cycle algorithm with initial duty cycle 0000.0000
     *              PEOF:0000.0000  Change to PeO fixed step algorithm with initial duty cycle 0000.0000
     *              BRUT:0000.0000  Change to Brute Force algorithm with initial duty cycle 0000.0000
     *
     */

    /* READ COMMANDS */

    MAKE_NODES(
        battery_voltage,
        {MAKE_NODE(VARIABLE, UNDEFINED, parse_read_meas_vbat, NULL)});
    MAKE_NODES(
        battert_current,
        {MAKE_NODE(VARIABLE, UNDEFINED, parse_read_meas_ibat, NULL)});
    MAKE_NODES(
        panel_voltage,
        {MAKE_NODE(VARIABLE, UNDEFINED, parse_read_meas_vpan, NULL)});
    MAKE_NODES(
        panel_current,
        {MAKE_NODE(VARIABLE, UNDEFINED, parse_read_meas_ipan, NULL)});
    MAKE_NODES(
        all_measurements,
        {MAKE_NODE(VARIABLE, UNDEFINED, parse_read_meas_allm, NULL)});

    MAKE_NODES(
        meas,
        {
            MAKE_NODE("VBAT", QUESTION, NULL, &battery_voltage),
            MAKE_NODE("IBAT", QUESTION, NULL, &battert_current),
            MAKE_NODE("VPAN", QUESTION, NULL, &panel_voltage),
            MAKE_NODE("IPAN", QUESTION, NULL, &panel_current),
            MAKE_NODE("ALLM", QUESTION, NULL, &all_measurements),
        });

    MAKE_NODES(
        read_duty_cycle,
        {MAKE_NODE(VARIABLE, UNDEFINED, parse_read_duty_cycle, NULL)});
    MAKE_NODES(
        read_frequency,
        {MAKE_NODE(VARIABLE, UNDEFINED, parse_read_frequency, NULL)});

    MAKE_NODES(
        read_hardware,
        {
            MAKE_NODE("DUTY", QUESTION, NULL, &read_duty_cycle),
            MAKE_NODE("FREQ", QUESTION, NULL, &read_frequency),
        });

    MAKE_NODES(
        read_control,
        {
            MAKE_NODE("ALGO", QUESTION, parse_read_control_algorithm, NULL),
        });


    MAKE_NODES(
        read,
        {
            MAKE_NODE("MEAS", COLON, NULL, &meas),
            MAKE_NODE("HARD", COLON, NULL, &read_hardware),        
            MAKE_NODE("CTRL", COLON, NULL, &read_control),        
        });

    /* WRITE COMMANDS */

    MAKE_NODES(
        duty,
        {
            MAKE_NODE(VARIABLE, DOT, parse_write_duty, NULL),
        });
    MAKE_NODES(
        freq,
        {
            MAKE_NODE(VARIABLE, DOT, parse_write_freq, NULL),
        });

    MAKE_NODES(
        write_hardware,
        {
            MAKE_NODE("DUTY", COLON, NULL, &duty),
            MAKE_NODE("FREQ", COLON, NULL, &freq),
        });

    MAKE_NODES(
        write_machine_state,
        {MAKE_NODE(VARIABLE, PERCENT, parse_write_machine_state, NULL)});

    MAKE_NODES(
        write_machine,
        {
            MAKE_NODE("STAT", COLON, NULL, &write_machine_state),
        });

    MAKE_NODES(
        write_fixed_algorithm,
        {MAKE_NODE(VARIABLE, DOT, parse_write_control_algorithm_fixed, NULL)});
    MAKE_NODES(
        write_peo_algorithm,
        {MAKE_NODE(VARIABLE, DOT, parse_write_control_algorithm_peo, NULL)});
    MAKE_NODES(
        write_brute_force_algorithm,
        {MAKE_NODE(VARIABLE, DOT, parse_write_control_algorithm_brute_force, NULL)});

    MAKE_NODES(
        write_control_algorithm,
        {
            MAKE_NODE("FIXD", COLON, NULL, &write_fixed_algorithm),
            MAKE_NODE("PEOF", COLON, NULL, &write_peo_algorithm),
            MAKE_NODE("BRUT", COLON, NULL, &write_brute_force_algorithm)
        });

    MAKE_NODES(
        write_control,
        {
            MAKE_NODE("ALGO", COLON, NULL, &write_control_algorithm),
        });

    MAKE_NODES(
        write,
        {
            MAKE_NODE("HARD", COLON, NULL, &write_hardware),
            MAKE_NODE("MACH", COLON, NULL, &write_machine),
            MAKE_NODE("CTRL", COLON, NULL, &write_control),
        });

    /* ROOT COMMAND */

    MAKE_NODES(
        root,
        {
            MAKE_NODE("READ", COLON, NULL, &read),
            MAKE_NODE("WRTE", COLON, NULL, &write),
        });

    const nodes_t *nodes = &root;

    /* Consume message */
    for (command_t command = get_command(&parser); parser.actual_index <= parser.end_index; command = get_command(&parser))
    {
        if (nodes == NULL)
        {
            break;
        }

        nodes = node_parser(&parser, &command, nodes);
    }
}

void uart_run(void)
{
    static uint32_t last_index_read = 0;
    const uint32_t last_index_dma = UART_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    const uint32_t index_diff = (last_index_dma - last_index_read) % UART_BUFFER_SIZE;
    if (index_diff)
    {
        uart_parse(last_index_read, index_diff);
        last_index_read = last_index_dma;
    }
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
}

/**
 * Uart message recived interrupt callback
 */

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
}