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
    
void posture_update(float y, float z, uint32_t time);
float posture_get_distance(void);
bool posture_is_notify_slouch();
void posture_start(uint32_t start_time, 
                   uint32_t duration, 
                   float distance_threshold, 
                   uint16_t time_threshold);
void posture_stop(void);

#endif /* POSTURE_H_ */
