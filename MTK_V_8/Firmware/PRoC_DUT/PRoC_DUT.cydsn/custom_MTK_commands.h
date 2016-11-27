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
#ifndef __CUSTOM_MTK_COMMANDS__
#define __CUSTOM_MTK_COMMANDS__
#include "stdint.h"
#include "CyBLE_MTK.h"

#define NO_ERROR                0
#define CMD_ERROR               -1

#define CUSTOM_CMD_READ_GPIO    1
#define CUSTOM_CMD_READ_SW2     2

extern int32_t process_custom_command(int32_t *command, uint8_t *output_buffer, int32_t *output_buffer_length);

#endif // __CUSTOM_MTK_COMMANDS__
/* [] END OF FILE */
