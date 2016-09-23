/*******************************************************************************
* File Name: VIN.h  
* Version 2.20
*
* Description:
*  This file contains the Alias definitions for Per-Pin APIs in cypins.h. 
*  Information on using these APIs can be found in the System Reference Guide.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_VIN_ALIASES_H) /* Pins VIN_ALIASES_H */
#define CY_PINS_VIN_ALIASES_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"


/***************************************
*              Constants        
***************************************/
#define VIN_0			(VIN__0__PC)
#define VIN_0_PS		(VIN__0__PS)
#define VIN_0_PC		(VIN__0__PC)
#define VIN_0_DR		(VIN__0__DR)
#define VIN_0_SHIFT	(VIN__0__SHIFT)
#define VIN_0_INTR	((uint16)((uint16)0x0003u << (VIN__0__SHIFT*2u)))

#define VIN_INTR_ALL	 ((uint16)(VIN_0_INTR))


#endif /* End Pins VIN_ALIASES_H */


/* [] END OF FILE */
