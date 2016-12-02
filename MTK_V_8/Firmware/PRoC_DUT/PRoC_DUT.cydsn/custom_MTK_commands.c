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
#include "project.h"
#include "custom_MTK_commands.h"

#if (CYBLE_MTK_DUT == 1)
int32_t process_custom_command(int32_t *command, uint8_t *output_buffer, int32_t *output_buffer_length)
{
    *output_buffer_length = 0;
    output_buffer[0] = 0;
    output_buffer[1] = 0;

    if (command[0] == CUSTOM_CMD_READ_GPIO)
    {
#if 0
        output_buffer[0] = Pin4_0_Read();
        output_buffer[0] <<= 1;

        output_buffer[0] |= Pin4_1_Read();
        output_buffer[0] <<= 1;

        output_buffer[0] |= Pin5_0_Read();
        output_buffer[0] <<= 1;

        output_buffer[0] |= Pin5_1_Read();
        output_buffer[0] <<= 1;

        output_buffer[0] |= Pin0_4_Read();
        output_buffer[0] <<= 1;

        output_buffer[0] |= Pin0_5_Read();
        output_buffer[0] <<= 1;

        output_buffer[0] |= Pin1_6_Read();
        output_buffer[0] <<= 1;

        output_buffer[0] |= Pin1_7_Read();
        /* output_buffer[1] */
        output_buffer[1] = Pin3_6_Read();
        output_buffer[1] <<= 1;

        output_buffer[1] |= Pin3_7_Read();
#endif

        *output_buffer_length = 2;
        return NO_ERROR;
    }
    else if (command[0] == CUSTOM_CMD_READ_SW2)
    {
        *output_buffer_length = 1;
        output_buffer[0] = 0;
#if 0
        output_buffer[0] = MTK_Trigger_Read();
#endif
        return NO_ERROR;
    }

    return CMD_ERROR;
}
#endif  // CYBLE_MTK_DUT
/* [] END OF FILE */
