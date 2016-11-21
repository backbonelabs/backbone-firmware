/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <project.h>
#include "uart_cmd_int.h"
#include "stdbool.h"

#if (UART_INTERFACE_ENABLED == 1)

static uint8_t CI_state;
static uint8_t rx_cmd, cmd_done_flag;
static int32_t cmd_index, cmd_argument_array[MAX_CMD_ARG_LEN], rx_cmd_len;
static uint8_t rx_data[4] = {'\0', '\0', '\0', '\0'}, negative_arg = 0, rx_data_neg_one = 0;

static const CMD_DEF_T command_lookup[NUMBER_OF_COMMANDS - 1] = 
{
/*       1    2  3    0,0          0,1    0,2     1,0          1,1    1,2    2,0          2,1         2,2*/
    {"TXP", TXP,     INTEGER, 3, {{  0,           0,         39}, { -18,         -18,          3}, { -1,        1000,      65535}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"RXP", RXP,     INTEGER, 2, {{  0,           0,         39}, {  -1,        1000,      65535}, {  0, ARG_DEF_VAL,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"RRS", RRS,   UNDEFINED, 0, {{  0, ARG_DEF_VAL,          0}, {   0, ARG_DEF_VAL,          0}, {  0, ARG_DEF_VAL,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"PST", PST,   UNDEFINED, 0, {{  0, ARG_DEF_VAL,          0}, {   0, ARG_DEF_VAL,          0}, {  0, ARG_DEF_VAL,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"SPL", SPL,     INTEGER, 1, {{  0,          37,         37}, {   0, ARG_DEF_VAL,          0}, {  0, ARG_DEF_VAL,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"SPT", SPT,     INTEGER, 1, {{  0,           0,          8}, {   0, ARG_DEF_VAL,          0}, {  0, ARG_DEF_VAL,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"VER", VER,   UNDEFINED, 0, {{  0, ARG_DEF_VAL,          0}, {   0, ARG_DEF_VAL,          0}, {  0, ARG_DEF_VAL,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"TXC", TXC,     INTEGER, 3, {{  0,           0,         39}, { -18,         -18,          3}, { -1,          -1, 2147483647}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"DUT", DUT,     INTEGER, 1, {{  0,           0,          1}, {   0, ARG_DEF_VAL,          0}, {  0, ARG_DEF_VAL,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"PCS", PCS,   UNDEFINED, 0, {{  0, ARG_DEF_VAL,          0}, {   0, ARG_DEF_VAL,          0}, {  0, ARG_DEF_VAL,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"WHO", WHO,   UNDEFINED, 0, {{  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"DCW", DCW,     INTEGER, 1, {{ -1,          -1,      65535}, {   0,           0,          0}, {  0,           0,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"STC", STC,     INTEGER, 1, {{ -1,           0, 2147483647}, {   0,           0,          0}, {  0,           0,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"CUS", CUS,     INTEGER, 3, {{  0,           0,        255}, {   0,           0,        255}, {  0,           0,        255}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"ARU", ARU,   UNDEFINED, 0, {{  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"RSX", RSX,     INTEGER, 1, {{  0,           0,         39}, {   0,           0,          0}, {  0,           0,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"WBA", WBA, HEXADECIMAL, 6, {{  0,           0,       0xFF}, {   0,           0,       0xFF}, {  0,           0,       0xFF}, {  0,           0,       0xFF}, {   0,           0,       0xFF}, {  0,           0,       0xFF}}},
    {"RBA", RBA,   UNDEFINED, 0, {{  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"RTR", RTR,   UNDEFINED, 0, {{  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"WTR", WTR, HEXADECIMAL, 1, {{  0,      0x8080,     0xFFFF}, {   0,           0,          0}, {  0,           0,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"STR", STR,   UNDEFINED, 0, {{  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}},
    {"LTR", LTR,   UNDEFINED, 0, {{  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}, {  0,           0,          0}, {   0,           0,          0}, {  0,           0,          0}}}
//  {"NEW", NEW, , 0, {{  MIN, Default,     MAX}, {   MIN, Default,     MAX}, {  MIN, Default,          MAX}}}
};

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static int32_t validate_argument(int value, int command, int argument_index)
{
    int32_t i;

    if ((command < 0) || (argument_index < 0))
        return 0;

    for (i = 0; i < (NUMBER_OF_COMMANDS - 1); i++)
    {
        if (command_lookup[i].cmd_ID == command)
            break;
    }
    if ((i >= (NUMBER_OF_COMMANDS - 1)) || (argument_index >= command_lookup[i].number_of_arguments))
        return 0;

    if ((command_lookup[i].argument[argument_index][ARG_MIN_INDEX] > value) ||
        (command_lookup[i].argument[argument_index][ARG_MAX_INDEX] < value))
    {
        return 0;
    }

    return 1;
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void FSM_cleanup()
{
    CI_state = CI_STATE_CMD;
    rx_data_neg_one = '\0';
    rx_data[0] = '\0';
    rx_data[1] = '\0';
    rx_data[2] = '\0';
    rx_data[3] = '\0';
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void ACK_reset_UART_FSM()
{
    UART_UartPutString("ACK\r\n");
    cmd_done_flag = 1;
    FSM_cleanup();
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void NAK_reset_UART_FSM()
{
    UART_UartPutString("NAC\r\n");
    cmd_done_flag = 0;
    FSM_cleanup();
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
void UART_RX_ISR()
{
    uint8_t temp;
    int32_t i, j;
    static int32_t cmd_param_index;
    static int32_t current_arg_value;
    static bool new_arg = false, ignore_space = false;

    UART_ClearRxInterruptSource(UART_INTR_RX_NOT_EMPTY);
#ifdef UART_LOOPBACK
    UART_UartPutChar(UART_UartGetChar());
#endif  //  UART_LOOPBACK

    temp = UART_UartGetChar();
    if (temp == '\0')
        return;

    if ((ignore_space == true) && ((temp == ' ') || (temp == '\t')))
        return;

    if (ignore_space == false)
    {
        rx_data_neg_one = rx_data[0];
        rx_data[0] = rx_data[1];
        rx_data[1] = rx_data[2];
        rx_data[2] = rx_data[3];
    }
    rx_data[3] = temp;

    if (CI_state == CI_STATE_CMD)
    {
        for (i = 0; i < (NUMBER_OF_COMMANDS - 1); i++)
        {
            for (j = 0; j < MAX_CMD_LEN; j++)
            {
                if (command_lookup[i].cmd_string[j] != rx_data[j])
                {
                    break;
                }
            }

            if ((j >= MAX_CMD_LEN) && ((rx_data_neg_one == ' ') || (rx_data_neg_one == '\n') ||
                (rx_data_neg_one == '\r') || (rx_data_neg_one == '\0') || (rx_data_neg_one == '\b')
                || (rx_data_neg_one == '\t')))
            {
                if ((command_lookup[i].number_of_arguments == 0) && ((rx_data[3] == ' ')
                                                    || (rx_data[3] == '\t')))
                {
                    ignore_space = true;
                }
                else if (((command_lookup[i].number_of_arguments > 0) && (rx_data[3] == ' ')) || 
                        (rx_data[3] == '\n') || (rx_data[3] == '\r') || (rx_data[3] == '\0'))
                {
                    cmd_done_flag = 0;
                    rx_cmd = command_lookup[i].cmd_ID;
                    rx_cmd_len = command_lookup[i].number_of_arguments;
                    cmd_index = i;
                    new_arg = false;
                    ignore_space = false;

                    if (rx_cmd_len == 0)
                    {
                        ACK_reset_UART_FSM();
                        return;
                    }
                    else if ((command_lookup[i].number_of_arguments > 0) && ((rx_data[3] == '\n') ||
                            (rx_data[3] == '\r') || (rx_data[3] == '\0')))
                    {
                        int32_t k;

                        for (k = 0; k < command_lookup[i].number_of_arguments; k++)
                        {
                            cmd_argument_array[k] = command_lookup[i].argument[k][ARG_DEF_INDEX];
#ifdef UART_CI_DEBUG
                            UART_print_int(cmd_argument_array[k]);
                            UART_UartPutString("\r\n");
#endif  // UART_CI_DEBUG
                        }
                        ACK_reset_UART_FSM();
                        return;
                    }
                    else
                    {
                        new_arg = true;
                        cmd_param_index = 0;
                        CI_state = CI_STATE_DATA;
                    }
                    break;
                }
                else
                {
                    ignore_space = false;
                }
            }
            else if (j >= MAX_CMD_LEN)
            {
                NAK_reset_UART_FSM();
                return;
            }
        }
        
        if ((i >= (NUMBER_OF_COMMANDS - 1)) && ((rx_data[3] == '\n') ||
                (rx_data[3] == '\r') || (rx_data[3] == '\0')))
        {
            if ((rx_data[2] != '\r') && (rx_data[2] != '\0') && (rx_data[2] != '\n'))
            {
                NAK_reset_UART_FSM();
            }
            return;
        }
    }
    else if (CI_state == CI_STATE_DATA)
    {
        if ((temp == ' ') || (temp == '\n') || (temp == '\r') || (temp == '\0') || (temp == '\t'))
        {
            if ((new_arg == true) && (temp == ' '))
            {
                return;
            }

            if (new_arg == false)
            {
                if (negative_arg == 1)
                {
                    if (current_arg_value == 0)
                    {
                        NAK_reset_UART_FSM();
                        return;
                    }
                    current_arg_value *= -1;
                }

                if ((cmd_param_index >= MAX_CMD_ARG_LEN) || 
                    !validate_argument(current_arg_value, rx_cmd, cmd_param_index))
                {
                    NAK_reset_UART_FSM();
                    return;
                }

                cmd_argument_array[cmd_param_index] = current_arg_value;
                cmd_param_index++;
            }

            if ((rx_data[3] == '\n') || (rx_data[3] == '\r') || (rx_data[3] == '\0') || (rx_data[3] == '\t'))
            {
                if (cmd_param_index >= command_lookup[cmd_index].number_of_arguments)
                {
                    ACK_reset_UART_FSM();
                    return;
                }
                else if ((cmd_param_index < command_lookup[cmd_index].number_of_arguments))
                {
                    int32_t k;

                    for (k = cmd_param_index; k < command_lookup[cmd_index].number_of_arguments; k++)
                    {
                        cmd_argument_array[k] = command_lookup[cmd_index].argument[k][ARG_DEF_INDEX];
    #ifdef UART_CI_DEBUG
                        UART_print_int(cmd_argument_array[k]);
                        UART_UartPutString("\r\n");
    #endif  // UART_CI_DEBUG
                    }
                    ACK_reset_UART_FSM();
                    return;
                }
            }
            new_arg = true;
            return;
        }

        if (new_arg == true)
        {
            current_arg_value = 0;
            negative_arg = 0;
        }

        if (command_lookup[cmd_index].arg_type == INTEGER)
        {
            if ((temp >= 0x30) && (temp <= 0x39))
            {
                int32_t temp_val;
                temp_val = current_arg_value;
                current_arg_value *= (int32_t)10;
                current_arg_value += (int32_t)(temp - 0x30);
                if (current_arg_value < temp_val)
                {
                    NAK_reset_UART_FSM();
                    return;
                }
            }
            else if ((new_arg == true) && (temp == '-'))
            {
                negative_arg = 1;
            }
            else if ((new_arg == true) && (temp == '+'))
            {
                return;
            }
        }
        else if (command_lookup[cmd_index].arg_type == HEXADECIMAL)
        {
            current_arg_value *= (int32_t)0x10;

            if((temp >= 0x30) && (temp <= 0x39))
			{
				current_arg_value += (int32_t)(temp - 0x30);
			}
			else if((temp >= 0x41) && (temp <= 0x46))
			{
	            current_arg_value += (int32_t)(temp - 0x41 + 10);
			}
			else if((temp >= 0x61) && (temp <= 0x66))
			{
	            current_arg_value += (int32_t)(temp - 0x61 + 10);
			}
            else
            {
                NAK_reset_UART_FSM();
                return;
            }
        }
        else/* if ((temp < 0x30) || (temp > 0x39))*/
        {
            NAK_reset_UART_FSM();
            return;
        }

        new_arg = false;
    }
    else
    {
        CI_state = CI_STATE_CMD;
    }    
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
UART_CMDS_T CI_get_command(int32_t *argument_array, int32_t *num_args)
{
    int32_t i;
    if (cmd_done_flag)
    {
        cmd_done_flag = 0;
        for (i = 0; i < rx_cmd_len; i++)
            argument_array[i] = cmd_argument_array[i];
        *num_args = rx_cmd_len;
        return rx_cmd;
    }

    return NO_CMD;
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
void CI_start()
{
    CI_state = CI_STATE_CMD;
    rx_cmd = NO_CMD;
    cmd_done_flag = 0;
    UART_Start();
    UART_SetCustomInterruptHandler((cyisraddress)UART_RX_ISR);
}
#endif  // UART_INTERFACE_ENABLED
/* [] END OF FILE */
