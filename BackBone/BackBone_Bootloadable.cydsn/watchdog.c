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
#include "watchdog.h"

typedef struct
{
    volatile uint32_t seconds;
} watchdog_t;

static volatile watchdog_t self;

CY_ISR(WdtIsrHandler)
{
    self.seconds += 1;
    /* Clear Interrupt */
    WdtIsr_ClearPending();
    CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);
}

void watchdog_init()
{
    self.seconds = 0;
    WdtIsr_StartEx(WdtIsrHandler);

    CySysWdtUnlock();
    CySysWdtSetMode(CY_SYS_WDT_COUNTER0, CY_SYS_WDT_MODE_INT_RESET);
    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
    CySysWdtLock();
}

uint32_t watchdog_get_time()
{
    return self.seconds;
}
