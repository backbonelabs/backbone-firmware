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

// TODO: Remove this include.  It is only for testing until I get
// hardware that actually works.  Added so I can set hal.new_gyro = 1.
#include <inv.h>

CY_ISR(WdtIsrHandler)
{
    /* Clear Interrupt */
    WdtIsr_ClearPending();
    CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);    
    hal.new_gyro = 1;
}

void watchdog_init()
{
    LED_Green_Write(0); // on
    CyDelay(250);
    LED_Green_Write(1); // off
    LED_Blue_Write(0); // on
    CyDelay(250);
    LED_Blue_Write(1); // off
    
    WdtIsr_StartEx(WdtIsrHandler);

    CySysWdtUnlock();
    CySysWdtSetMode(CY_SYS_WDT_COUNTER0, CY_SYS_WDT_MODE_INT_RESET);
    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);    
    CySysWdtLock();
}

/* [] END OF FILE */
