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

#ifndef TIMEOUT_H_
#define TIMEOUT_H_

#include <project.h>

/* Variable declaration */
extern volatile uint8 TimeoutErrorFlags;
extern volatile uint8 I2CTimeoutCounter;

/* Flags for the TimeoutErrorFlags variables */
#define I2C_TIMEOUT             0x01

/* Function prototypes*/
void Timeout_Start(uint8_t Timeout);
void Timeout_Stop(void);

#endif   /* TIMEOUT_H_ */
