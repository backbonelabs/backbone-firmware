/*******************************************************************************
* File Name: main.c
*
* Version: 1.10
*
* Description:
*  This example shows how to build a launcher project for a BLE design.
*  This project is required for designs that require Over the Air upgrade
*  capability for both Application and the BLE stack.
*
* References:
*  BLUETOOTH SPECIFICATION Version 4.1
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include "project.h"
#include "options.h"
#include "debug.h"

#if CYDEV_FLASH_SIZE != 0x00040000u
 #error "This design is specifically targeted to parts with 256k of flash. Please change project device to BLE\
 silicon that has 256K Flash array. For example CY8C4248LQI-BL483."
#endif

static void SetApp0Active()
{
    Launcher_SetFlashByte((uint32) Launcher_MD_BTLDB_ACTIVE_OFFSET(0), (uint8)1);    
}

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  Starts the bootloader component in launcher+copier mode.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
int main()
{
    DBG_PRINT_TEXT("> Backbone Launcher\r\n");
    DBG_PRINT_TEXT("> Compile Date and Time: " __DATE__ " " __TIME__ "\r\n\r\n");

#ifdef DISABLE_BOOTLOADER_FIRMWARE_UPGRADE
    uint8 metaBuf[CYDEV_FLS_ROW_SIZE];
    uint8 copyFlag;
    
    copyFlag = Launcher_GetMetadata(Launcher_GET_BTLDB_COPY_FLAG, 
                                    Launcher_MD_BTLDB_ACTIVE_1);
    if (copyFlag)
    {
        // If a bootloader / stack update was performed then do just do everything
        // the Launcher_Copier function does but without the actual copy.  Then
        // reset the system which will cause the launcher to load the bootloader
        Launcher_SetFlashByte(Launcher_MD_BTLDB_COPY_FLAG_OFFSET(Launcher_MD_BTLDB_ACTIVE_1),
                              (copyFlag & (~Launcher_NEED_TO_COPY_SET_BIT)));   
        (void) memset(&metaBuf, 0x00u, sizeof(metaBuf));
        CySysFlashWriteRow((uint16) (CY_FLASH_NUMBER_ROWS - 2u), metaBuf);          
        Launcher_SET_RUN_TYPE(Launcher_SCHEDULE_BTLDB);
        CySoftwareReset();
    }
#endif

    if ((CySysGetResetReason(0) & CY_SYS_RESET_SW) != CY_SYS_RESET_SW)
    {
        // if not a software reset
        SetApp0Active();
        Launcher_SET_RUN_TYPE(Launcher_SCHEDULE_BTLDR);    
        CySoftwareReset();
    }
    
    Launcher_Start();
    for(;;)
    {
        /* Should newer get here. */
    }
}

/* [] END OF FILE */
