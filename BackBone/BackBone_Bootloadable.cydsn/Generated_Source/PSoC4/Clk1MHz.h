/*******************************************************************************
* File Name: Clk1MHz.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_Clk1MHz_H)
#define CY_CLOCK_Clk1MHz_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
*        Function Prototypes
***************************************/
#if defined CYREG_PERI_DIV_CMD

void Clk1MHz_StartEx(uint32 alignClkDiv);
#define Clk1MHz_Start() \
    Clk1MHz_StartEx(Clk1MHz__PA_DIV_ID)

#else

void Clk1MHz_Start(void);

#endif/* CYREG_PERI_DIV_CMD */

void Clk1MHz_Stop(void);

void Clk1MHz_SetFractionalDividerRegister(uint16 clkDivider, uint8 clkFractional);

uint16 Clk1MHz_GetDividerRegister(void);
uint8  Clk1MHz_GetFractionalDividerRegister(void);

#define Clk1MHz_Enable()                         Clk1MHz_Start()
#define Clk1MHz_Disable()                        Clk1MHz_Stop()
#define Clk1MHz_SetDividerRegister(clkDivider, reset)  \
    Clk1MHz_SetFractionalDividerRegister((clkDivider), 0u)
#define Clk1MHz_SetDivider(clkDivider)           Clk1MHz_SetDividerRegister((clkDivider), 1u)
#define Clk1MHz_SetDividerValue(clkDivider)      Clk1MHz_SetDividerRegister((clkDivider) - 1u, 1u)


/***************************************
*             Registers
***************************************/
#if defined CYREG_PERI_DIV_CMD

#define Clk1MHz_DIV_ID     Clk1MHz__DIV_ID

#define Clk1MHz_CMD_REG    (*(reg32 *)CYREG_PERI_DIV_CMD)
#define Clk1MHz_CTRL_REG   (*(reg32 *)Clk1MHz__CTRL_REGISTER)
#define Clk1MHz_DIV_REG    (*(reg32 *)Clk1MHz__DIV_REGISTER)

#define Clk1MHz_CMD_DIV_SHIFT          (0u)
#define Clk1MHz_CMD_PA_DIV_SHIFT       (8u)
#define Clk1MHz_CMD_DISABLE_SHIFT      (30u)
#define Clk1MHz_CMD_ENABLE_SHIFT       (31u)

#define Clk1MHz_CMD_DISABLE_MASK       ((uint32)((uint32)1u << Clk1MHz_CMD_DISABLE_SHIFT))
#define Clk1MHz_CMD_ENABLE_MASK        ((uint32)((uint32)1u << Clk1MHz_CMD_ENABLE_SHIFT))

#define Clk1MHz_DIV_FRAC_MASK  (0x000000F8u)
#define Clk1MHz_DIV_FRAC_SHIFT (3u)
#define Clk1MHz_DIV_INT_MASK   (0xFFFFFF00u)
#define Clk1MHz_DIV_INT_SHIFT  (8u)

#else 

#define Clk1MHz_DIV_REG        (*(reg32 *)Clk1MHz__REGISTER)
#define Clk1MHz_ENABLE_REG     Clk1MHz_DIV_REG
#define Clk1MHz_DIV_FRAC_MASK  Clk1MHz__FRAC_MASK
#define Clk1MHz_DIV_FRAC_SHIFT (16u)
#define Clk1MHz_DIV_INT_MASK   Clk1MHz__DIVIDER_MASK
#define Clk1MHz_DIV_INT_SHIFT  (0u)

#endif/* CYREG_PERI_DIV_CMD */

#endif /* !defined(CY_CLOCK_Clk1MHz_H) */

/* [] END OF FILE */
