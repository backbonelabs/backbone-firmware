/*******************************************************************************
 * UART command interpreter functions
 ******************************************************************************/
#ifndef __UART_CMD_INT_H__
#define __UART_CMD_INT_H__

#include <project.h>
#include "CyBLE_MTK.h"

#define MAX_CMD_LEN         3
#define MAX_CMD_ARG_LEN     6

typedef enum UART_cmds
{
    NO_CMD = 0,
    TXP,
    RXP,
    RRS,
    PST,
    SPL,
    SPT,
    VER,
    TXC,
    DUT,
    PCS,
    WHO,
    DCW,
    STC,
    CUS,
    ARU,
    RSX,
    WBA,
    RBA,
    RTR,
    WTR,
    STR,
    LTR,
    // NEW command to be added here,
    NUMBER_OF_COMMANDS
} UART_CMDS_T;

typedef enum UART_cmd_args_type
{
    UNDEFINED,
    INTEGER,
    HEXADECIMAL
} UART_CMD_ARGS_T;

#define CI_STATE_CMD        0x03
#define CI_STATE_DATA       0x04

#define ARG_MIN_VAL         0
#define ARG_DEF_VAL         0
#define ARG_MAX_VAL         2147483647

#define ARG_MIN_INDEX       0
#define ARG_DEF_INDEX       1
#define ARG_MAX_INDEX       2

typedef struct cmd_def_t
{
    int8_t          cmd_string[MAX_CMD_LEN];
    UART_CMDS_T     cmd_ID;
    UART_CMD_ARGS_T arg_type;
    uint8_t         number_of_arguments;
    int32_t         argument[MAX_CMD_ARG_LEN][3];
} CMD_DEF_T;

#if (UART_INTERFACE_ENABLED == 1)
    extern void             CI_start(void);
    extern UART_CMDS_T      CI_get_command(int32_t *argument_array, int32_t *num_args);
#endif  // UART_INTERFACE_ENABLED

#endif // __UART_CMD_INT_H__
