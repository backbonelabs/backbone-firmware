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

#ifndef POSTURE_H_
#define POSTURE_H_

#include <stdint.h>
#include <stdbool.h>

void posture_start(uint32_t start_time,
                   uint32_t duration,
                   float distance_threshold,
                   uint16_t time_threshold,
                   uint8_t pattern,
                   uint8_t duty_cycle,
                   uint16_t motor_on_time);
void posture_resume(uint32_t resume_time,
                    float distance_threshold,
                    uint16_t time_threshold,
                    uint8_t pattern,
                    uint8_t duty_cycle,
                    uint16_t motor_on_time);
void posture_update(float x, float y, float z, uint32_t time);
void posture_pause(void);
void posture_stop(void);
bool posture_is_monitoring(void);
uint32_t posture_get_elapsed_time(void);
uint32_t posture_get_slouch_time(void);
uint32_t posture_get_session_duration(void);
float posture_get_distance(void);
bool posture_is_notify_slouch(void);
bool posture_is_slouch(void);
uint8_t posture_get_vibration_pattern(void);
uint8_t posture_get_duty_cycle(void);
uint16_t posture_get_motor_on_time(void);

#endif /* POSTURE_H_ */
