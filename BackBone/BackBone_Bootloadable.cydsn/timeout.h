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
extern uint8 TimeoutEnableFlags;
extern uint8 TimeoutErrorFlags;
extern uint8 I2CTimeoutCounter;
extern uint16 BoostCircuitTimeoutCounter;

/* Flags for the TimeoutEnableFlags and TimeoutErrorFlags variables */
#define I2C_TIMEOUT             0x01

/* Function prototypes*/
void Timeout_Start(void);
void Timeout_Stop(void);
void I2CCommTimeout_Enable(uint8 Timeout);
void I2CCommTimeout_Disable(void);
void I2CCommTimeout_ClearFlag(void);

#endif   /* TIMEOUT_H_ */
