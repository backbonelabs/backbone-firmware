/*******************************************************************************
* File Name: Timer1ms_PM.c
* Version 2.10
*
* Description:
*  This file contains the setup, control, and status commands to support
*  the component operations in the low power mode.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2013-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "Timer1ms.h"

static Timer1ms_BACKUP_STRUCT Timer1ms_backup;


/*******************************************************************************
* Function Name: Timer1ms_SaveConfig
********************************************************************************
*
* Summary:
*  All configuration registers are retention. Nothing to save here.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Timer1ms_SaveConfig(void)
{

}


/*******************************************************************************
* Function Name: Timer1ms_Sleep
********************************************************************************
*
* Summary:
*  Stops the component operation and saves the user configuration.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Timer1ms_Sleep(void)
{
    if(0u != (Timer1ms_BLOCK_CONTROL_REG & Timer1ms_MASK))
    {
        Timer1ms_backup.enableState = 1u;
    }
    else
    {
        Timer1ms_backup.enableState = 0u;
    }

    Timer1ms_Stop();
    Timer1ms_SaveConfig();
}


/*******************************************************************************
* Function Name: Timer1ms_RestoreConfig
********************************************************************************
*
* Summary:
*  All configuration registers are retention. Nothing to restore here.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Timer1ms_RestoreConfig(void)
{

}


/*******************************************************************************
* Function Name: Timer1ms_Wakeup
********************************************************************************
*
* Summary:
*  Restores the user configuration and restores the enable state.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void Timer1ms_Wakeup(void)
{
    Timer1ms_RestoreConfig();

    if(0u != Timer1ms_backup.enableState)
    {
        Timer1ms_Enable();
    }
}


/* [] END OF FILE */
