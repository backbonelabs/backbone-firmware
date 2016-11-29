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
#include "CyBLE_MTK.h"
#include "stdio.h"
#include "anritsu.h"

#define MTK_MOD_SEL                                     0xAB
#define ANRITSU_SEL                                     0xAA

int32_t MTK_triggered()
{
#if 0
    if (MTK_Trigger_Read() == 0)
    {
        CyDelay(15);
        while (!MTK_Trigger_Read());
        return 1;
    }
    return 0;
#endif
    return 1;
}

int main()
{
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
#if 0
    uint8_t var = MTK_MOD_SEL;
#endif
    CyGlobalIntEnable; /* Uncomment this line to enable global interrupts. */

    if (MTK_triggered())
    {
        while (1)
        {
            MTK_mode();
#if 0
            if (var == MTK_MOD_SEL)
            {
                var = MTK_mode();
            }
            else if (var == ANRITSU_SEL)
            {
                var = Anritsu();
            }
#endif
        }
    }

    /* Implement application here. */
#if 0
    Red_LED_Write(0);
#endif
    while (1);

    return 0;
}

/* [] END OF FILE */
