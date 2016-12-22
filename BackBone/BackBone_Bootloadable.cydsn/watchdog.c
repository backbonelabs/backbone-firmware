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
#include "debug.h"

#define WDT_COUNT0_MATCH    (0x8000u) // 32768 -  1 second
#define WDT_COUNT1_MATCH    (0x0014u) //    20 - 20 seconds

typedef struct
{
    volatile uint32_t seconds;
    volatile bool clear_requested;
} watchdog_t;

static volatile watchdog_t self;

CY_ISR(WdtIsrHandler)
{
    self.seconds += 1;
    self.clear_requested = true;
    CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);
    WdtIsr_ClearPending();
}

/**
 * Configures watchdog timer 0 to generate an interrupt once per second.
 * Cascades watchdog timer 1 to watchdog timer 0 so that the watchdog timer 1
 * counter increments once per second.  If watchdog timer 1 ever reaches its
 * match value (20) then a reset is generated.  This will happen if the main
 * loop does not call watchdog_clear() for 20 seconds.
 */
void watchdog_init()
{
    CySysWdtUnlock();
    self.seconds = 0;

	/* Setup ISR for interrupts at WDT counter 0 events. */
    WdtIsr_StartEx(WdtIsrHandler);

    /* Set WDT counter 0 to generate interrupt on match */
    CySysWdtSetMode(CY_SYS_WDT_COUNTER0, CY_SYS_WDT_MODE_INT);
    CySysWdtSetMatch(CY_SYS_WDT_COUNTER0, WDT_COUNT0_MATCH);
    CySysWdtSetClearOnMatch(CY_SYS_WDT_COUNTER0, 1u);

    /* Enable WDT counters 0 and 1 cascade */
    CySysWdtSetCascade(CY_SYS_WDT_CASCADE_01);

    /* Set WDT counter 1 to generate reset on match */
    CySysWdtSetMatch(CY_SYS_WDT_COUNTER1, WDT_COUNT1_MATCH);
    CySysWdtSetMode(CY_SYS_WDT_COUNTER1, CY_SYS_WDT_MODE_RESET);
    CySysWdtSetClearOnMatch(CY_SYS_WDT_COUNTER1, 1u);

    /* Enable WDT counters 0 and 1 */
    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK | CY_SYS_WDT_COUNTER1_MASK);
    CySysWdtLock();
}

bool watchdog_is_clear_requested()
{
    return self.clear_requested;
}

void watchdog_clear()
{
    // If the watchdog is not fed it will get hangry and reset the application
    CySysWdtUnlock();
    CySysWatchdogFeed(CY_SYS_WDT_COUNTER1);
    CySysWdtLock();

    self.clear_requested = false;
}

uint32_t watchdog_get_time()
{
    return self.seconds;
}
