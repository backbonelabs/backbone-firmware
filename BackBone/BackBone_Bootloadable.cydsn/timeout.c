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
#include <timeout.h>

volatile uint8 TimeoutErrorFlags = 0;
volatile uint8 I2CTimeoutCounter = 0;

CY_ISR(Timer1ms_ISR)
{
    I2CTimeoutCounter -= 1;
    if (I2CTimeoutCounter == 0)
    {
        TimeoutErrorFlags |= I2C_TIMEOUT;
    }
}

/******************************************************************************
* Function Name: Timeout_Start
*******************************************************************************
*
* Summary:
*  This function initializes the Timeout mechanism
*
*
* Parameters:
*  Timeout - Timeout in ms
*
* Return:
*  void
*
*******************************************************************************/
void Timeout_Start(uint8 Timeout)
{
    TimeoutErrorFlags = 0;
    I2CTimeoutCounter = Timeout;
    Timer1ms_Start();
    isr_Timer1ms_StartEx(Timer1ms_ISR);
}

/******************************************************************************
* Function Name: Timeout_Stop
*******************************************************************************
*
* Summary:
*  This function stops the timeout mechanism
*
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Timeout_Stop(void)
{
    Timer1ms_Stop();
    isr_Timer1ms_Stop();
}
