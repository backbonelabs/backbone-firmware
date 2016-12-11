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

#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdbool.h>
#include <stdint.h>

void motor_init(void);
void motor_start(uint8_t duty_cycle, uint16_t milliseconds, uint8_t pattern);
void motor_stop(void);
bool motor_is_running(void);

#endif /* MOTOR_H_ */
