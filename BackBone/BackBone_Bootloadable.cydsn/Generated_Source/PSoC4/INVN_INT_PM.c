/*******************************************************************************
* File Name: INVN_INT.c  
* Version 2.20
*
* Description:
*  This file contains APIs to set up the Pins component for low power modes.
*
* Note:
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "INVN_INT.h"

static INVN_INT_BACKUP_STRUCT  INVN_INT_backup = {0u, 0u, 0u};


/*******************************************************************************
* Function Name: INVN_INT_Sleep
****************************************************************************//**
*
* \brief Stores the pin configuration and prepares the pin for entering chip 
*  deep-sleep/hibernate modes. This function must be called for SIO and USBIO
*  pins. It is not essential if using GPIO or GPIO_OVT pins.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None 
*  
* \sideeffect
*  For SIO pins, this function configures the pin input threshold to CMOS and
*  drive level to Vddio. This is needed for SIO pins when in device 
*  deep-sleep/hibernate modes.
*
* \funcusage
*  \snippet INVN_INT_SUT.c usage_INVN_INT_Sleep_Wakeup
*******************************************************************************/
void INVN_INT_Sleep(void)
{
    #if defined(INVN_INT__PC)
        INVN_INT_backup.pcState = INVN_INT_PC;
    #else
        #if (CY_PSOC4_4200L)
            /* Save the regulator state and put the PHY into suspend mode */
            INVN_INT_backup.usbState = INVN_INT_CR1_REG;
            INVN_INT_USB_POWER_REG |= INVN_INT_USBIO_ENTER_SLEEP;
            INVN_INT_CR1_REG &= INVN_INT_USBIO_CR1_OFF;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(INVN_INT__SIO)
        INVN_INT_backup.sioState = INVN_INT_SIO_REG;
        /* SIO requires unregulated output buffer and single ended input buffer */
        INVN_INT_SIO_REG &= (uint32)(~INVN_INT_SIO_LPM_MASK);
    #endif  
}


/*******************************************************************************
* Function Name: INVN_INT_Wakeup
****************************************************************************//**
*
* \brief Restores the pin configuration that was saved during Pin_Sleep().
*
* For USBIO pins, the wakeup is only triggered for falling edge interrupts.
*
* <b>Note</b> This function is available in PSoC 4 only.
*
* \return 
*  None
*  
* \funcusage
*  Refer to INVN_INT_Sleep() for an example usage.
*******************************************************************************/
void INVN_INT_Wakeup(void)
{
    #if defined(INVN_INT__PC)
        INVN_INT_PC = INVN_INT_backup.pcState;
    #else
        #if (CY_PSOC4_4200L)
            /* Restore the regulator state and come out of suspend mode */
            INVN_INT_USB_POWER_REG &= INVN_INT_USBIO_EXIT_SLEEP_PH1;
            INVN_INT_CR1_REG = INVN_INT_backup.usbState;
            INVN_INT_USB_POWER_REG &= INVN_INT_USBIO_EXIT_SLEEP_PH2;
        #endif
    #endif
    #if defined(CYIPBLOCK_m0s8ioss_VERSION) && defined(INVN_INT__SIO)
        INVN_INT_SIO_REG = INVN_INT_backup.sioState;
    #endif
}


/* [] END OF FILE */
