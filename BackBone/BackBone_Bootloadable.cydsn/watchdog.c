/* ===========================================
 *
 * Copyright BACKBONE LABS INC, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION,
 * WHICH IS THE PROPERTY OF BACKBONE LABS INC.
 *
 * ===========================================
 */

#include <project.h>

CY_ISR(WdtIsrHandler)
{
    /* Clear Interrupt */
    WdtIsr_ClearPending();
    CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);
}

void watchdog_init()
{
    WdtIsr_StartEx(WdtIsrHandler);

    CySysWdtUnlock();
    CySysWdtSetMode(CY_SYS_WDT_COUNTER0, CY_SYS_WDT_MODE_INT_RESET);
    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
    CySysWdtLock();
}
