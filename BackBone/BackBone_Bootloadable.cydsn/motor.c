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
#include "debug.h"

static volatile bool m_is_running;
static volatile uint8_t m_count;
static uint8_t m_duty_cycle;
static uint16_t m_milliseconds;

/**
 * Interrupt handler for the motor timer timout.
 *
 * The motor timer is run in continuous mode.  m_count is set to two times the
 * "pattern" where "pattern" is the number of times to turn on the motor when
 * the distance and time threshold are exceeded in a posture monitoring session.
 *
 * When m_count is even the motor is running and needs to be turned off.  When
 * m_count is odd the idle period has just expired and the motor needs to be
 * turned back on.
 */
CY_ISR(motor_timer_timeout)
{
    MotorTimer_ClearInterrupt(MotorTimer_INTR_MASK_TC);

    if (!(m_count & 0x01))
    {
        if (m_count > 2)
        {
            // turn off just the motor  let the timer keep running
            MotorPWM_Stop();

            // but set the timer period to 100 milliseconds for a short delay
            // between motor pulses
            MotorTimer_WritePeriod(100);
        }
        else
        {
            // finished pulsing the motor  stop everything.
            motor_stop();
        }
    }
    else
    {
        // The delay between motor pulses just finished running so
        // start the motor again
        MotorPWM_Init();
        MotorPWM_Enable();
        MotorPWM_WriteCompare(m_duty_cycle);

        // and set the timer period back to the motor on duration
        MotorTimer_WritePeriod(m_milliseconds);
    }

    m_count -= 1;
}

void motor_init(void)
{
    m_is_running = false;
    MotorTimerInterrupt_StartEx(motor_timer_timeout);
}

void motor_start(uint8_t duty_cycle, uint16_t milliseconds, uint8_t pattern)
{
    if (pattern > 0)
    {
        m_duty_cycle = duty_cycle;
        m_milliseconds = milliseconds;
        m_count = pattern * 2;

        // turn on the motor
        m_is_running = true;
        MotorPWM_Init();
        MotorPWM_Enable();
        MotorPWM_WriteCompare(m_duty_cycle);

        // start the timer so the motor will not run indefinitely
        MotorTimer_WriteCounter(0);
        MotorTimer_Start();
        MotorTimer_SetOneShot(0);
        MotorTimer_WritePeriod(m_milliseconds);
    }
    else
    {
        m_duty_cycle = 0;
        m_milliseconds = 0;
        m_count = 0;
    }
}

void motor_stop(void)
{
    MotorPWM_Stop();
    MotorTimer_Stop();
    m_is_running = false;
}

bool motor_is_running(void)
{
    return m_is_running;
}
