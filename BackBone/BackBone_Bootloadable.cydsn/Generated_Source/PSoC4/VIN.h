/*******************************************************************************
* File Name: VIN.h  
* Version 2.20
*
* Description:
*  This file contains Pin function prototypes and register defines
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_VIN_H) /* Pins VIN_H */
#define CY_PINS_VIN_H

#include "cytypes.h"
#include "cyfitter.h"
#include "VIN_aliases.h"


/***************************************
*     Data Struct Definitions
***************************************/

/**
* \addtogroup group_structures
* @{
*/
    
/* Structure for sleep mode support */
typedef struct
{
    uint32 pcState; /**< State of the port control register */
    uint32 sioState; /**< State of the SIO configuration */
    uint32 usbState; /**< State of the USBIO regulator */
} VIN_BACKUP_STRUCT;

/** @} structures */


/***************************************
*        Function Prototypes             
***************************************/
/**
* \addtogroup group_general
* @{
*/
uint8   VIN_Read(void);
void    VIN_Write(uint8 value);
uint8   VIN_ReadDataReg(void);
#if defined(VIN__PC) || (CY_PSOC4_4200L) 
    void    VIN_SetDriveMode(uint8 mode);
#endif
void    VIN_SetInterruptMode(uint16 position, uint16 mode);
uint8   VIN_ClearInterrupt(void);
/** @} general */

/**
* \addtogroup group_power
* @{
*/
void VIN_Sleep(void); 
void VIN_Wakeup(void);
/** @} power */


/***************************************
*           API Constants        
***************************************/
#if defined(VIN__PC) || (CY_PSOC4_4200L) 
    /* Drive Modes */
    #define VIN_DRIVE_MODE_BITS        (3)
    #define VIN_DRIVE_MODE_IND_MASK    (0xFFFFFFFFu >> (32 - VIN_DRIVE_MODE_BITS))

    /**
    * \addtogroup group_constants
    * @{
    */
        /** \addtogroup driveMode Drive mode constants
         * \brief Constants to be passed as "mode" parameter in the VIN_SetDriveMode() function.
         *  @{
         */
        #define VIN_DM_ALG_HIZ         (0x00u) /**< \brief High Impedance Analog   */
        #define VIN_DM_DIG_HIZ         (0x01u) /**< \brief High Impedance Digital  */
        #define VIN_DM_RES_UP          (0x02u) /**< \brief Resistive Pull Up       */
        #define VIN_DM_RES_DWN         (0x03u) /**< \brief Resistive Pull Down     */
        #define VIN_DM_OD_LO           (0x04u) /**< \brief Open Drain, Drives Low  */
        #define VIN_DM_OD_HI           (0x05u) /**< \brief Open Drain, Drives High */
        #define VIN_DM_STRONG          (0x06u) /**< \brief Strong Drive            */
        #define VIN_DM_RES_UPDWN       (0x07u) /**< \brief Resistive Pull Up/Down  */
        /** @} driveMode */
    /** @} group_constants */
#endif

/* Digital Port Constants */
#define VIN_MASK               VIN__MASK
#define VIN_SHIFT              VIN__SHIFT
#define VIN_WIDTH              1u

/**
* \addtogroup group_constants
* @{
*/
    /** \addtogroup intrMode Interrupt constants
     * \brief Constants to be passed as "mode" parameter in VIN_SetInterruptMode() function.
     *  @{
     */
        #define VIN_INTR_NONE      ((uint16)(0x0000u)) /**< \brief Disabled             */
        #define VIN_INTR_RISING    ((uint16)(0x5555u)) /**< \brief Rising edge trigger  */
        #define VIN_INTR_FALLING   ((uint16)(0xaaaau)) /**< \brief Falling edge trigger */
        #define VIN_INTR_BOTH      ((uint16)(0xffffu)) /**< \brief Both edge trigger    */
    /** @} intrMode */
/** @} group_constants */

/* SIO LPM definition */
#if defined(VIN__SIO)
    #define VIN_SIO_LPM_MASK       (0x03u)
#endif

/* USBIO definitions */
#if !defined(VIN__PC) && (CY_PSOC4_4200L)
    #define VIN_USBIO_ENABLE               ((uint32)0x80000000u)
    #define VIN_USBIO_DISABLE              ((uint32)(~VIN_USBIO_ENABLE))
    #define VIN_USBIO_SUSPEND_SHIFT        CYFLD_USBDEVv2_USB_SUSPEND__OFFSET
    #define VIN_USBIO_SUSPEND_DEL_SHIFT    CYFLD_USBDEVv2_USB_SUSPEND_DEL__OFFSET
    #define VIN_USBIO_ENTER_SLEEP          ((uint32)((1u << VIN_USBIO_SUSPEND_SHIFT) \
                                                        | (1u << VIN_USBIO_SUSPEND_DEL_SHIFT)))
    #define VIN_USBIO_EXIT_SLEEP_PH1       ((uint32)~((uint32)(1u << VIN_USBIO_SUSPEND_SHIFT)))
    #define VIN_USBIO_EXIT_SLEEP_PH2       ((uint32)~((uint32)(1u << VIN_USBIO_SUSPEND_DEL_SHIFT)))
    #define VIN_USBIO_CR1_OFF              ((uint32)0xfffffffeu)
#endif


/***************************************
*             Registers        
***************************************/
/* Main Port Registers */
#if defined(VIN__PC)
    /* Port Configuration */
    #define VIN_PC                 (* (reg32 *) VIN__PC)
#endif
/* Pin State */
#define VIN_PS                     (* (reg32 *) VIN__PS)
/* Data Register */
#define VIN_DR                     (* (reg32 *) VIN__DR)
/* Input Buffer Disable Override */
#define VIN_INP_DIS                (* (reg32 *) VIN__PC2)

/* Interrupt configuration Registers */
#define VIN_INTCFG                 (* (reg32 *) VIN__INTCFG)
#define VIN_INTSTAT                (* (reg32 *) VIN__INTSTAT)

/* "Interrupt cause" register for Combined Port Interrupt (AllPortInt) in GSRef component */
#if defined (CYREG_GPIO_INTR_CAUSE)
    #define VIN_INTR_CAUSE         (* (reg32 *) CYREG_GPIO_INTR_CAUSE)
#endif

/* SIO register */
#if defined(VIN__SIO)
    #define VIN_SIO_REG            (* (reg32 *) VIN__SIO)
#endif /* (VIN__SIO_CFG) */

/* USBIO registers */
#if !defined(VIN__PC) && (CY_PSOC4_4200L)
    #define VIN_USB_POWER_REG       (* (reg32 *) CYREG_USBDEVv2_USB_POWER_CTRL)
    #define VIN_CR1_REG             (* (reg32 *) CYREG_USBDEVv2_CR1)
    #define VIN_USBIO_CTRL_REG      (* (reg32 *) CYREG_USBDEVv2_USB_USBIO_CTRL)
#endif    
    
    
/***************************************
* The following code is DEPRECATED and 
* must not be used in new designs.
***************************************/
/**
* \addtogroup group_deprecated
* @{
*/
#define VIN_DRIVE_MODE_SHIFT       (0x00u)
#define VIN_DRIVE_MODE_MASK        (0x07u << VIN_DRIVE_MODE_SHIFT)
/** @} deprecated */

#endif /* End Pins VIN_H */


/* [] END OF FILE */
