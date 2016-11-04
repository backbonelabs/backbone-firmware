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

void posture_update(float y, float z);
float posture_get_distance(void);
void posture_start(void);
void posture_stop(void);

#endif /* POSTURE_H_ */
