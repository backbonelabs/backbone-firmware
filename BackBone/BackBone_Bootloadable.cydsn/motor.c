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
#include "motor.h"

static volatile bool m_is_running;

CY_ISR(motor_timer_timeout)
{
    motor_stop();
    MotorTimer_ClearInterrupt(MotorTimer_INTR_MASK_TC);
}

void motor_init(void)
{
    m_is_running = false;
    MotorTimerInterrupt_StartEx(motor_timer_timeout);
}

void motor_start(uint8_t duty_cycle, uint16_t milliseconds)
{
    m_is_running = true;

    MotorPWM_Init();
    MotorPWM_Enable();
    MotorPWM_WriteCompare(duty_cycle);

    // start the timer so the motor will not run indefinitely
    MotorTimer_WriteCounter(0);
    MotorTimer_Start();
    MotorTimer_WritePeriod(milliseconds);
}

void motor_stop(void)
{
    m_is_running = false;
    MotorPWM_Stop();
}

bool motor_is_running(void)
{
    return m_is_running;
}
