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
#include "CyBLE_MTK.h"
#include "stdio.h"

//static uint32_t tx_count;

int32_t MTK_triggered()
{
    if (MTK_Trigger_Read() == 0)
    {
        CyDelay(15);
        while (!MTK_Trigger_Read());
        return 1;
    }
    return 0;
}

int main()
{
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    CyGlobalIntEnable; /* Uncomment this line to enable global interrupts. */
    MTK_mode();

    return 0;
}

/* [] END OF FILE */
