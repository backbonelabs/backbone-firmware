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

void watchdog_init();
uint32_t watchdog_get_time();

#endif /* WATCHDOG_H_ */
