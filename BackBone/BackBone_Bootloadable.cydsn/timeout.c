/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include <timeout.h>

uint8 TimeoutEnableFlags = 0;
uint8 TimeoutErrorFlags = 0;

uint8 I2CTimeoutCounter = 0;

CY_ISR(Timer1ms_ISR)
{

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
void Timeout_Start(void)
{
    Timer1ms_Start();
    isr_Timer1ms_Start();
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


/******************************************************************************
* Function Name: I2CCommTimeout_Enable
*******************************************************************************
*
* Summary:
*  Enables the timeout for the I2C communication
*
*
* Parameters:
*  Timeout - 8 bit timeout value in ms
*
* Return:
*  void
*
*******************************************************************************/
void I2CCommTimeout_Enable(uint8 Timeout)
{
    TimeoutEnableFlags |= I2C_TIMEOUT;
    I2CTimeoutCounter = Timeout;
}

/******************************************************************************
* Function Name: I2CCommTimeout_Disable
*******************************************************************************
*
* Summary:
*  This function disables the timeout for the I2C communication
*
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void I2CCommTimeout_Disable(void)
{
    TimeoutEnableFlags &= ~I2C_TIMEOUT;
}

/******************************************************************************
* Function Name: I2CCommTimeout_ClearFlag
*******************************************************************************
*
* Summary:
*  This function clears the I2C timeout error flag
*
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void I2CCommTimeout_ClearFlag(void)
{
    TimeoutErrorFlags &= ~I2C_TIMEOUT;
}




/* [] END OF FILE */
