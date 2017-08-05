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

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#include <stdint.h>
#include <stdbool.h>

#define WATCHDOG_SECONDS_PER_DAY (60*60*24) // 86,400

void watchdog_init(uint32_t day_time);
uint32_t watchdog_get_time();
uint32_t watchdog_get_day_time();
void watchdog_set_day_time(uint32_t day_time);
bool watchdog_is_clear_requested();
void watchdog_clear();

#endif /* WATCHDOG_H_ */
