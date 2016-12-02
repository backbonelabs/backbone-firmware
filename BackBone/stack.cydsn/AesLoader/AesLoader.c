/****************************************************************************//**
* \file AesLoader.c
* \version 1.50
*
* \brief
*   Provides an API for the Bootloader component. The API includes functions
*   for starting bootloading operations, validating the application and
*   jumping to the application.
*
********************************************************************************
* \copyright
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#define CY_BOOTLOADER_Loader_H
#include "AesLoader_PVT.h"

#include "cyapicallbacks.h"

//#include "project.h"
#include "cyfitter_cfg.h"
#include "cydevice_trm.h"
#include "cyfitter.h"
#include "cydisabledsheets.h"
#include "AesLoader.h"
#include "AesLoader_PVT.h"
#include "Bootloadable.h"
#include "CyBle_gatt.h"
#include "CyBle.h"
#include "CyBle_HAL_PVT.h"
#include "CyBle_STACK_PVT.h"
#include "CyBle_StackGap.h"
#include "CyBle_StackGatt.h"
#include "CyBle_StackGattDb.h"
#include "CyBle_StackHostMain.h"
#include "CyBle_StackL2cap.h"
#include "CyBle_StackGattServer.h"
#include "CyBle_StackGattClient.h"
#include "CyBle_Stack.h"
#include "CyBle_eventHandler.h"
#include "CyBle_bts.h"
#include "CyBle_bless_isr.h"
#include "core_cm0_psoc4.h"
#include "CyFlash.h"
#include "CyLib.h"
#include "cyPm.h"
#include "cytypes.h"
#include "cypins.h"
#include "CyLFClk.h"
// end
#include <string.h>

#if (CY_BOOT_VERSION < CY_BOOT_5_0)
    #error Component Bootloader_v1_50 requires cy_boot v5.00 or later
#endif /* (CY_BOOT_VERSION >= CY_BOOT_5_0) */

/*******************************************************************************
* The Checksum and SizeBytes are forcefully set in code. We then post process
* the hex file from the linker and inject their values. When the hex file
* is loaded into the device, these two variables should have valid values.
* Because the compiler can do optimization to remove the constant
* accesses, these should not be accessed directly. Instead, the variables
* CyBtldr_ChecksumAccess & CyBtldr_SizeBytesAccess should be used to get the
* proper values at the runtime.
*******************************************************************************/
#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
    #if defined(__ARMCC_VERSION) || defined (__GNUC__)
        __attribute__((section (".bootloader"), used))
    #elif defined (__ICCARM__)
        #pragma location=".bootloader"
    #endif  /* defined(__ARMCC_VERSION) || defined (__GNUC__) */

    #if defined(__ARMCC_VERSION) || defined (__GNUC__) || defined (__C51__)
        const uint8  CYCODE AesLoader_Checksum = 0u;
    #elif defined (__ICCARM__)
        __root const uint8  CYCODE AesLoader_Checksum = 0u;
    #endif  /* defined(__ARMCC_VERSION) || defined (__GNUC__) || defined (__C51__) */
    const uint8  CYCODE *AesLoader_ChecksumAccess  = (const uint8  CYCODE *)(&AesLoader_Checksum);

    #if defined(__ARMCC_VERSION) || defined (__GNUC__)
        __attribute__((section (".bootloader"), used))
        const uint32 CYCODE AesLoader_SizeBytes = 0xFFFFFFFFu;
    #elif defined (__ICCARM__)
        #pragma location=".bootloader"
        __root const uint32 CYCODE AesLoader_SizeBytes = 0xFFFFFFFFu;
    #elif defined (__C51__)
        const uint32 CYCODE AesLoader_SizeBytes = 0xFFFFFFFFu;
    #endif  /* defined(__ARMCC_VERSION) || defined (__GNUC__) */

    const uint32 CYCODE *AesLoader_SizeBytesAccess = (const uint32 CYCODE *)(&AesLoader_SizeBytes);
#endif /*(CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/

/**
 \defgroup variables_group Variables
 @{
*/
#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
    /**
    *  This variable is intended to indicate that in-application
    *  bootloading initialization is done. The initialization itself is performed in the
    *  AesLoader_Initialize() function. Once the initialization is done,
    *  the variable is set and this prevents the functionality from reinitialization.
    */
    uint8 AesLoader_initVar = AesLoader_BOOTLOADING_NOT_INITIALIZED;

    /**
    *  This variable is intended to keep the current application number. It applies
    *  only to in-application bootloading.
    */
    uint8 AesLoader_runningApp = AesLoader_RUNNING_APPLICATION_UNKNOWN;

    /**
    *  This variable is intended to indicate that 'Enter bootloader' command has
    *  been received. It applies only to in-application bootloading.
    */
    uint8 AesLoader_isBootloading = AesLoader_BOOTLOADING_COMPLETED;
#endif /*CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER*/

#if ((0u != AesLoader_DUAL_APP_BOOTLOADER)  || (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))
    /**
    *  This variable is intended to keep the active application number. It applies
    *  to Classic Dual-app Bootloader and in-application bootloading.
    */
    volatile uint8 AesLoader_activeApp = AesLoader_MD_BTLDB_ACTIVE_NONE;
#else /*SINGLE-APP BOOTLOADER*/
    #define AesLoader_activeApp      (AesLoader_MD_BTLDB_ACTIVE_0)
#endif  /* (0u != AesLoader_DUAL_APP_BOOTLOADER) */


#if (0u != AesLoader_SECURITY_KEY_AVAIL)
/**
 *  This variable holds the security key for additional safety during communication.
 */
static const uint8 CYCODE AesLoader_securityKey[AesLoader_SECURITY_KEY_LENGTH] = {0x11u, 0x22u, 0x33u, 0x44u, 0x55u, 0x66u};
#endif /*(0u != AesLoader_SECURITY_KEY_AVAIL)*/

#if ((CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER) && (!CY_PSOC3))
    /**
    *  This variable holds the pointer on the user's callback-function that implements the custom
    *  bootloader command processing.
    */
    static AesLoader_callback_type AesLoader_callback = NULL;
#endif /*(CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER) && (!CY_PSOC3)*/
/** @}*/

/***************************************
*     Function Prototypes
***************************************/
#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
    static void  AesLoader_LaunchApplication(void) CYSMALL \
    ;
    #if(!CY_PSOC3)
        /* Implementation for PSoC 3 resides in Bootloader_psoc3.a51 file.  */
        static void     AesLoader_LaunchBootloadable(uint32 appAddr);
    #endif  /* (!CY_PSOC3) */
#endif /* CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER */

#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER)
    static cystatus AesLoader_WritePacket(uint8 status, uint8 buffer[], uint16 size) CYSMALL \
    ;

    static uint16   AesLoader_CalcPacketChecksum(const uint8 buffer[], uint16 size) CYSMALL \
    ;

    #if (0u != AesLoader_CMD_VERIFY_FLS_ROW_AVAIL)
        static uint8 AesLoader_VerifyRow(uint32 flashStart, const uint8* ramStart, uint16 size) \
        CYSMALL ;
    #endif /*(0u != AesLoader_CMD_VERIFY_FLS_ROW_AVAIL)*/

    #if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
        static void AesLoader_HostLink(uint8 timeOut) CYLARGE ;
    #endif /* (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) */
#endif /*(CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER)*/

#if (((0u != AesLoader_DUAL_APP_BOOTLOADER) && (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER)) \
                                          || (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))
static uint8 AesLoader_CheckImage(uint8 appNumber, uint8 arrayNumber, uint16 row, uint16 rowNumInArray) CYSMALL \
;
#endif /*(((0u != AesLoader_DUAL_APP_BOOTLOADER) && (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER)) \
                                          || (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))*/

#if ((CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) || (0u != AesLoader_DUAL_APP_BOOTLOADER))
    static void AesLoader_SetActiveAppInMetadata(uint8 activeApp) CYSMALL \
    ;
#endif /* (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) || (0u != AesLoader_DUAL_APP_BOOTLOADER) */

#if ((CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) || \
     (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER) || \
     (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER))
static uint8 AesLoader_GetActiveAppFromMetadata(void) CYSMALL \
;
#endif /*((CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) || \
          (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER) || \
          (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER)) */

#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER)
    #if ((0u != AesLoader_COPIER_AVAIL) && (CY_PSOC4))
        static cystatus AesLoader_CopyRow(uint16 srcRowNum, uint16 dstRowNum) \
        CYSMALL ;
        static void AesLoader_Copier(void) CYSMALL ;
    #endif /*(0u != AesLoader_COPIER_AVAIL) && (CY_PSOC4)*/
#endif /*(CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER)*/


/**
 \defgroup functions_group Functions
 @{
*/
#if ((CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) || \
                                  (0u != AesLoader_DUAL_APP_BOOTLOADER))
/*******************************************************************************
* Function Name: AesLoader_SetActiveAppInMetadata
****************************************************************************//**
* \internal
*
* \brief
*    This function sets the active application in metadata.
*    The other application is set inactive.
*
* \param activeApp
*   The active application number (0 or 1). If the active application number is
*   greater than 1, then nothing is done.
*
* \endinternal
*******************************************************************************/
static void AesLoader_SetActiveAppInMetadata(uint8 activeApp) CYSMALL \

{
    if (AesLoader_MD_BTLDB_ACTIVE_NONE > activeApp)
    {
        uint8 CYDATA idx;
        for (idx = 0u; idx < AesLoader_MAX_NUM_OF_BTLDB; idx++)
        {
            (void)AesLoader_SetFlashByte((uint32) AesLoader_MD_BTLDB_ACTIVE_OFFSET(idx),
            (uint8)(idx == activeApp));
        }
    }
}
#endif /* (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) || 
                           (0u != AesLoader_DUAL_APP_BOOTLOADER) */

#if ((CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) || \
     (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER) || \
     (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER))
/*******************************************************************************
* Function Name: AesLoader_GetActiveAppFromMetadata
****************************************************************************//**
* \internal
*
* \brief
*    This function performs reading of both metadata sections to
*    identify an active application. If none application is set active,
*    then AesLoader_MD_BTLDB_ACTIVE_NONE (0x02) is returned.
*
* \return
*   The number of active applications. In the case of error:
*   -  AesLoader_MD_BTLDB_ACTIVE_NONE (0x02) - neither application is initialized;
*   -  AesLoader_BOTH_ACTIVE (0x03) - both applications are set active;
*
* \endinternal
*******************************************************************************/
static  uint8 AesLoader_GetActiveAppFromMetadata(void) CYSMALL \

{
    uint8 CYDATA result;
    uint8 CYDATA app0 = (uint8)AesLoader_GetMetadata(AesLoader_GET_BTLDB_ACTIVE,
    AesLoader_MD_BTLDB_ACTIVE_0);
    uint8 CYDATA app1 = (uint8)AesLoader_GetMetadata(AesLoader_GET_BTLDB_ACTIVE,
    AesLoader_MD_BTLDB_ACTIVE_1);

    if (0u != app0)
    {
        if (0u == app1)
        {
            /* app0 is active */
            result = AesLoader_MD_BTLDB_ACTIVE_0;
        }
        else
        {
            /* Both are active */
            result = AesLoader_BOTH_ACTIVE;
        }
    }
    else /* (0u == app0) */
    {
        if (0u != app1)
        {
            /* app1 is active */
            result = AesLoader_MD_BTLDB_ACTIVE_1;
        }
        else
        {
            /* Neither app is active. */
            result = AesLoader_MD_BTLDB_ACTIVE_NONE;
        }
    }

    return (result);
}
#endif /*((CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) || \
          (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER) || \
          (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER)) */

#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
/*******************************************************************************
* Function Name: AesLoader_Initialize
****************************************************************************//**
*
* \brief
*   Used for in-app bootloading. This function updates the global variable
*   AesLoader_runningApp with a running application number.
*
*   If the running application number is valid (0 or 1), this function also
*   sets the global variable AesLoader_initVar that is used to determine
*   if the component can process bootloader commands or not.
*   This function should be called once in the application project after a startup.
*
* \return
*   Global variables:
*     - AesLoader_runningApp
*     - AesLoader_activeApp
*     - AesLoader_initVar
*
* \details
*   This API should be called first to be able to capture the active application
*   number that is actually the running application number.
*******************************************************************************/
void AesLoader_Initialize(void) CYSMALL \

{
    if (AesLoader_BOOTLOADING_NOT_INITIALIZED ==AesLoader_initVar)
    {
        AesLoader_activeApp = AesLoader_GetActiveAppFromMetadata();

        /* Updating with number of active application */
        if ((AesLoader_MD_BTLDB_ACTIVE_NONE != AesLoader_activeApp) &&
        (AesLoader_BOTH_ACTIVE != AesLoader_activeApp))
        {
            AesLoader_runningApp = AesLoader_activeApp;

            /* Bootloader commands are to be processed */
            AesLoader_initVar = AesLoader_BOOTLADING_INITIALIZED;
        }
        else
        {
            AesLoader_runningApp = AesLoader_RUNNING_APPLICATION_UNKNOWN;
        }
    }
}

/*****************************************************************************
* Function Name: AesLoader_GetRunningAppStatus
**************************************************************************//**
*
* \brief
*   Used for dual-app or in-app bootloader. Returns the value of the global
*   variable AesLoader_runningApp. This function should be called only after the
*   AesLoader_Initialize() has been called once.
*
* \return
*   The application number that is currently being executed. Values are 0 or 1;
*     other values indicate "not defined".
*
*******************************************************************************/
uint8 AesLoader_GetRunningAppStatus(void) CYSMALL \

{
    return (AesLoader_runningApp);
}


/*******************************************************************************
* Function Name: AesLoader_GetActiveAppStatus
****************************************************************************//**
*
* \brief
*   Used for dual-app or in-app bootloader. Returns the value of the global
*   variable AesLoader_activeApp. This function should be called only after the
*   AesLoader_Initialize() has been called once.
*
* \return
*   The desired application to be executed. Values are 0 or 1; other values
*     indicate "not defined".
*
*******************************************************************************/
uint8 AesLoader_GetActiveAppStatus(void) CYSMALL \

{
    /* Read active application number from metadata */
    AesLoader_activeApp = AesLoader_GetActiveAppFromMetadata();

    return (AesLoader_activeApp);
}
#endif /*(CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/

#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER)
/*******************************************************************************
* Function Name: AesLoader_CalcPacketChecksum
****************************************************************************//**
*  \internal
*
*  \brief
*     This computes a 16-bit checksum for the provided number of bytes contained
*     in the provided buffer.
*
*  \param buffer
*     The buffer containing the data to compute the checksum.
*  \param size
*     The number of bytes in the buffer to compute the checksum.
*
*  \return
*     A 16-bit checksum for the provided data.
*
*  \endinternal
*******************************************************************************/
static uint16 AesLoader_CalcPacketChecksum(const uint8 buffer[], uint16 size) \
CYSMALL
{
#if(0u != AesLoader_PACKET_CHECKSUM_CRC)

    uint16 CYDATA crc = AesLoader_CRC_CCITT_INITIAL_VALUE;
    uint16 CYDATA tmp;
    uint8  CYDATA i;
    uint16 CYDATA tmpIndex = size;

    if (0u == size)
    {
        crc = ~crc;
    }
    else
    {
        do
        {
            tmp = buffer[tmpIndex - size];

            for (i = 0u; i < 8u; i++)
            {
                if (0u != ((crc & 0x0001u) ^ (tmp & 0x0001u)))
                {
                    crc = (crc >> 1u) ^ AesLoader_CRC_CCITT_POLYNOMIAL;
                }
                else
                {
                    crc >>= 1u;
                }

                tmp >>= 1u;
            }

            size--;
        }
        while (0u != size);

        crc = ~crc;
        tmp = crc;
        crc = ( uint16 )(crc << 8u) | (tmp >> 8u);
    }

    return (crc);

#else

    uint16 CYDATA sum = 0u;

    while (size > 0u)
    {
        sum += buffer[size - 1u];
        size--;
    }

    return (( uint16 )1u + ( uint16 )(~sum));

#endif /* (0u != AesLoader_PACKET_CHECKSUM_CRC) */
}

#if (!CY_PSOC3)
/*******************************************************************************
* Function Name: AesLoader_InitCallback
****************************************************************************//**
*
* \brief
*  This function initializes the callback functionality.
*
* \param userCallback
*  The user's callback function.
*
*******************************************************************************/
void AesLoader_InitCallback(AesLoader_callback_type userCallback) \
CYSMALL
{
    AesLoader_callback = userCallback;
}
#endif /* (!CY_PSOC3) */


#if (0u != AesLoader_CMD_VERIFY_FLS_ROW_AVAIL)
/*******************************************************************************
* Function Name: AesLoader_VerifyRow
****************************************************************************//**
*  \internal
*
*  \brief
*   This API performs a byte to byte verifying of the flash row against the data
*   in the input buffer.
*
*  \param flashStart
*        The start address of a row in flash.
*  \param ramStart
*        The start address of corresponding data to compare in the RAM buffer.
*  \param size
*        The data length (common for both arrays).
*
*  \return
*   CYRET_SUCCESS - If all data matches.
*  \n AesLoader_ERR_VERIFY - If there is any mismatch.
*
* \endinternal
*******************************************************************************/
static uint8 AesLoader_VerifyRow(uint32 flashStart, const uint8* ramStart, uint16 size) \
CYSMALL
{
    uint8 CYDATA result = CYRET_SUCCESS;
    uint16 CYDATA idx;

    for (idx = 0u; idx < size; idx++)
    {
        if (CY_GET_XTND_REG8((uint8 CYFAR *)(flashStart + idx)) != ramStart[idx])
        {
            result = AesLoader_ERR_VERIFY;
            break;
        }
    }

    return (result);
}
#endif /*(0u != AesLoader_CMD_VERIFY_FLS_ROW_AVAIL)*/
#endif /*CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER*/


/*******************************************************************************
* Function Name: AesLoader_Calc8BitSum
****************************************************************************//**
*
* \brief
*  This computes an 8-bit sum for the provided number of bytes contained in
*  flash (if baseAddr equals CY_FLASH_BASE) or EEPROM (if baseAddr equals
*  CY_EEPROM_BASE).
*
* \param baseAddr
*   CY_FLASH_BASE
*   CY_EEPROM_BASE - applicable only for PSoC 3 / PSoC 5LP devices.
*
* \param start
*     The starting address to start summing data.
* \param size
*     The number of bytes to read and compute the sum.
*
* \return
*   An 8-bit sum for the provided data.
*
*******************************************************************************/
uint8 AesLoader_Calc8BitSum(uint32 baseAddr, uint32 start, uint32 size) \
CYSMALL
{
    uint8 CYDATA sum = 0u;

#if(!CY_PSOC4)
    CYASSERT((baseAddr == CY_EEPROM_BASE) || (baseAddr == CY_FLASH_BASE));
#else
    CYASSERT(baseAddr == CY_FLASH_BASE);
#endif  /* (!CY_PSOC4) */

    while (size > 0u)
    {
        size--;
        sum += (*((uint8  *)(baseAddr + start + size)));
    }

    return (sum);
}


#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER)
#if ((0u != AesLoader_COPIER_AVAIL) && (CY_PSOC4))
/*******************************************************************************
* Function Name: AesLoader_CopyRow
****************************************************************************//**
* \internal
*
* \brief
*   This function performs copying of a specified row to specified
*     location.
*
* \param
*   srcRowNum - The number of a row to copy from.
* \param
*   dstRowNum - The number of a row to be copied to.
*
* \return
*   \n CYRET_SUCCESS - If everything went right.
*   \n Otherwise, produce an error code indicating a problem.
*
* \endinternal
*******************************************************************************/
static cystatus AesLoader_CopyRow(uint16 srcRowNum, uint16 dstRowNum) \
CYSMALL
{
    /*Buffer for row copy procedure*/
    uint8  rowBuffer[CYDEV_FLS_ROW_SIZE];
    cystatus result = CYRET_SUCCESS;

    uint32 srcBaseAddr = CYDEV_FLASH_BASE + srcRowNum * CYDEV_FLS_ROW_SIZE;
    uint16 idx;

    for (idx = 0u; idx < CYDEV_FLS_ROW_SIZE; idx++)
    {
        rowBuffer[idx] = AesLoader_GET_CODE_BYTE(srcBaseAddr + idx);
    }

    result = CySysFlashWriteRow((uint16) dstRowNum, rowBuffer);

    return (result);
}

/*******************************************************************************
* Function Name: AesLoader_Copier
****************************************************************************//**
*  \internal
*
*  \brief
*    This option is specific for Launcher-Combination architecture only and to
*    the case when there is a large communication component like BLE. To save
*    flash space for the user application, such a large communication component
*    is present only in the first application (Stack application) and is "shared"
*    with the second application (the user's application actually) by "Code
*    sharing feature" (See code sharing and BLE Upgradable Stack example project).
*
* \n If this option is enabled and the "need-to-copy" flag is set in the second
*    application's metadata, Copier performs copying of a previously saved Stack
*    application image from a "temporary location" to the app#1 flash
*    space (over the current app#1).
*
* \n Before performing the copying operation the new Stack application image
*    should be received and stored at the temporary location by the current
*    Stack application. The "need-to-copy" flag should be set in app#2 metadata
*    indicating that copying operation is required.
*
* \n The temporary location is half of the flash space that left after Launcher
*    and 2 metadata rows. This implies that Stack application should fit in
*    that half of the flash space.
*
* \n The destination for the copy operation is obtained as the first row after
*    Launcher (from app#1 metadata).
*
* \n The source of copy operation is the first row of temporary location that
*    is calculated by the following formula:
* \n     srcRow = (((CY_FLASH_NUMBER_ROWS - dstRow - metaRows) / 2) + dstRow);
* \n     where:
*          - dstRow - the destination's first row (mentioned above);
*          - CY_FLASH_NUMBER_ROWS - number of all flash rows;
*          - metaRows - 2 metadata rows;
*
* \n Copying itself means row-by-row copying of the stored application's image.
*    If it fails to copy some row, there is a defined number of attempts
*    (AesLoader_MAX_ATTEMPTS) to try again. In case of no success,
*    a user callback function Launcher_CopierCallback() is called.
*    If it is not defined, then the device is going to be halted.
*
* \n After performing the application image copying, in case of success
*    the app#2 metadata is copied in the stead of app#1 metadata and then
*    app#2 metadata is cleared.
*
* \n A "need-to-copy" flag is cleared just after performing the app#1 image
*    copying from the temporary location to avoid another consecutive
*    copying operation.
*
* \endinternal
*******************************************************************************/
static void AesLoader_Copier(void) CYSMALL \

{
    uint16 idx = 0u;

    /* Number of attempts to perform copying again */
    uint8 copierAttempts;

    /* "need-to-copy" flag from second application metadata */
    uint8 copyFlag = AesLoader_GetMetadata(AesLoader_GET_BTLDB_COPY_FLAG, AesLoader_MD_BTLDB_ACTIVE_1);

    /* Buffer to temporary hold metadata before flashing */
    uint8 metaBuf[CYDEV_FLS_ROW_SIZE];

    /* Launcher's last row number */
    uint16 launcherLastRow = AesLoader_GetMetadata(AesLoader_GET_BTLDR_LAST_ROW, AesLoader_MD_BTLDB_ACTIVE_0);

    /* Calculating destination's start row number (first row of app#1) */
    uint16 dstRow = launcherLastRow + 1u;

    /* Calculating the source's/temporary location's start row number */
    uint16 srcRow = ((((CY_FLASH_NUMBER_ROWS - dstRow - 2u) + 1u) >> 1u) + dstRow);

    /* Taking length from app#2's metadata*/
    uint32 srcLength = AesLoader_GetMetadata(AesLoader_GET_BTLDB_LENGTH, AesLoader_MD_BTLDB_ACTIVE_1);

    /* Copy operation status initialization */
    cystatus copyStatus = CYRET_SUCCESS;

    /* Calculating number of rows for copying */
    uint16 numberOfRows = srcLength / CYDEV_FLS_ROW_SIZE;

    if (0u != (srcLength % CYDEV_FLS_ROW_SIZE))
    {
        numberOfRows++;
    }

    /*************************************************************************
    * Copying the number of rows from source (temporary location) to destination.
    * If the operation is unsuccessful, there will be a few more attempts to
    * perform the copying operation (a number is defined by AesLoader_MAX_ATTEMPTS)
    ***************************************************************************/
    for (idx = 0; idx < numberOfRows; idx++)
    {
        copierAttempts = AesLoader_MAX_ATTEMPTS;

        do
        {
            /* Copying row */
            copyStatus = AesLoader_CopyRow(srcRow + idx, dstRow + idx);

            /* Decrementing counter of attempts to repeat copying operation */
            copierAttempts--;
        }
        while ((CYRET_SUCCESS != copyStatus) && (0u != copierAttempts));


        /*************************************************************************
        * If a problem occurs, when a row can't be copied, "CopyFlag" should be
        * cleared to prevent the recurrent copying that might follow the software reset.
        ***************************************************************************/
        if (CYRET_SUCCESS != copyStatus)
        {
            /* Clear copy flag in metadata#1 */
            AesLoader_SetFlashByte(AesLoader_MD_BTLDB_COPY_FLAG_OFFSET(AesLoader_MD_BTLDB_ACTIVE_1),
                                   (copyFlag & (~AesLoader_NEED_TO_COPY_SET_BIT)));

            /* Callback macro definition */
#if (0u != LAUNCHER_COPIER_CALLBACK)
            AesLoader_CopierCallback();
#endif /* 0u != LAUNCHER_COPIER_CALLBACK */

            /*************************************************************************
            * If the copying operation was unsuccessful after certain number of attempts
            * and AesLoader_CopierCallback() is either undefined or returns, then the
            * device should be halted.
            *************************************************************************/
            CyHalt(0x00u);
        }
    }

    /* Copy metadata app#2 -> app#1*/
    if (CYRET_SUCCESS == copyStatus)
    {
        /******************************************************************************
        *                               Metadata#1 update
        ******************************************************************************/
        /* Getting start address of metadata #2 */
        uint32 metaStartAddr = AesLoader_MD_BASE_ADDR(AesLoader_MD_BTLDB_ACTIVE_1) +
                               AesLoader_MD_SIZEOF - CYDEV_FLS_ROW_SIZE;

        /* Read all data from metadata #2 */
        for (idx = 0u; idx < CYDEV_FLS_ROW_SIZE; idx++)
        {
            metaBuf[idx] = AesLoader_GET_CODE_BYTE(metaStartAddr + idx);
        }

        /* Setting 'Copier available' flag (copyFlag field) in buffer to be stored in metadata#1 */
        metaBuf[AesLoader_MD_BUFFER_START_OFFSET + AesLoader_MD_COPY_FLAG_BYTE_OFFSET] = AesLoader_COPIER_SUPPORT_SET_BIT;

        /* Set active app#1 */
        AesLoader_activeApp = AesLoader_MD_BTLDB_ACTIVE_0;

        /* Setting app#1 as active application in buffer to be stored in metadata#1 */
        metaBuf[AesLoader_MD_BUFFER_START_OFFSET + AesLoader_MD_ACTIVE_APP_BYTE_OFFSET] = 0x01u;

        /* Setting app#1 as valid for 'Fast validation' to be stored in metadata#1 */
        metaBuf[AesLoader_MD_BUFFER_START_OFFSET + AesLoader_MD_FAST_VALID_BYTE_OFFSET] = 0x01u;

        /* Update metadata#1 in flash */
        copyStatus = CySysFlashWriteRow((uint16) (CY_FLASH_NUMBER_ROWS - 1u), metaBuf);

        /******************************************************************************
        *                               Metadata#2 update
        * Clear the whole buffer for metadata#2, which will make it invalid.
        * The 'Need-to-copy' flag will be cleared and app#2 will be set as non-active.
        *******************************************************************************/
        (void) memset(&metaBuf, 0x00u, sizeof(metaBuf));

        /* Updating metadata#2 in flash */
        copyStatus |= CySysFlashWriteRow((uint16) (CY_FLASH_NUMBER_ROWS - 2u), metaBuf);
    }

    if (CYRET_SUCCESS == copyStatus)
    {
        /* Schedule*/
        AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDB);

        /* Software reset */
        CySoftwareReset();
    }
    else
    {
        /* Halt device */
        CyHalt(0x00u);
    }
}
#endif /*(0u != AesLoader_COPIER_AVAIL) && (CY_PSOC4)*/
#endif /*(CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER)*/


/*******************************************************************************
* Function Name: AesLoader_Start
****************************************************************************//**
* \brief
*  This function is called to execute the following algorithm:
*
* -    Validate the Bootloadable application for the Classic Single-app Bootloader or
*   both Bootloadable/Combination applications for the Classic Dual-app Bootloader/
*   Launch-only Bootloader (Launcher for short) respectively.
*
* -    For the Classic Single-app Bootloader: if the Bootloadable application is valid,
*   then the flow switches to it after a software reset. Otherwise it stays in
*   the Bootloader, waiting for a command(s) from the host.
*
* -    For the Classic Dual-app Bootloader: the flow acts according to the switching table
*  (see in the code below) and enabled/disabled options (for instance, auto-switching).
*   NOTE If the valid Bootloadable application is identified, then the control is passed
*   to it after a software reset. Otherwise it stays in the Classic Dual-app Bootloader
*   waiting for a command(s) from the host.
*
* -    For the Launcher: the flow acts according to the switching table (see below) and
*   enabled/disabled options. NOTE If the valid Combination application is identified, then
*   the control is passed to it after a software reset. Otherwise it stays in the Launcher
*   forever.
*
* -    Validate the Bootloader/Launcher application(s) (design-time configurable, Bootloader
*   application validation option of the component customizer).
*
* -    Run a communication subroutine (design-time configurable, the Wait for command
*   option of the component customizer). NOTE This is NOT applicable for the Launcher.
*
* -    Schedule the Bootloadable and reset the device.
*
*  \ref page_switching_logic
*
* \return
*  This method will never return. It will either load a new application and
*  reset the device or jump directly to the existing application. The CPU is
*  halted, if the validation fails when the "Bootloader application validation"
*  option is enabled.
*  PSoC 3/PSoC 5: The CPU is halted if flash initialization fails.
*
* \details
*  If the "Bootloader application validation" option is enabled and this method
*  determines that the Bootloader application itself is corrupt, this method
*  will not return, instead it will simply hang the application.
*******************************************************************************/
void AesLoader_Start(void) CYSMALL
{

#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)

#if(0u != AesLoader_DUAL_APP_BOOTLOADER)
    cystatus app0Status = CYRET_BAD_DATA;
    cystatus app1Status = CYRET_BAD_DATA;
#endif /*0u != AesLoader_DUAL_APP_BOOTLOADER*/

    cystatus validApp  = CYRET_BAD_DATA;

#if(0u != AesLoader_BOOTLOADER_APP_VALIDATION)
    uint8 CYDATA calcedChecksum;
#endif    /* (0u != AesLoader_BOOTLOADER_APP_VALIDATION) */

#if(!CY_PSOC4)
#if(0u != AesLoader_FAST_APP_VALIDATION)
    /* Initialize flash subsystem for non-PSoC 4 devices */
    if (CYRET_SUCCESS != CySetTemp())
    {
        CyHalt(0x00u);
    }

#if !defined(CY_BOOT_VERSION)
    /* Not required with cy_boot 4.20 */
    if (CYRET_SUCCESS != CySetFlashEEBuffer(AesLoader_flashBuffer))
    {
        CyHalt(0x00u);
    }

    uint8 CYXDATA AesLoader_flashBuffer[AesLoader_FROW_SIZE];
#endif /* !defined(CY_BOOT_VERSION) */
#endif  /* (0u != AesLoader_FAST_APP_VALIDATION) */
#endif  /* (!CY_PSOC4) */

#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER)
#if ((0u != AesLoader_COPIER_AVAIL) && (CY_PSOC4))
    /* Checking "need-to-copy" flag in metadata#1. If it is set, then copy operation is required*/
    uint8 needToCopyFlag = AesLoader_GetMetadata(AesLoader_GET_BTLDB_COPY_FLAG,
    AesLoader_MD_BTLDB_ACTIVE_1);

    /* Checking "Need to copy" flag in metadata#1 */
    if (0u != (needToCopyFlag & AesLoader_NEED_TO_COPY_MASK))
    {
        /* Note that this function never returns */
        AesLoader_Copier();
    }
#endif  /* (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER) */
#endif  /* (0u != AesLoader_COPIER_AVAIL) && (CY_PSOC4)*/

    /* Identify active Bootloadable/Combination application */
#if (0u != AesLoader_DUAL_APP_BOOTLOADER)
    app0Status = AesLoader_ValidateBootloadable(AesLoader_MD_BTLDB_ACTIVE_0);
    app1Status = AesLoader_ValidateBootloadable(AesLoader_MD_BTLDB_ACTIVE_1);

    /* Assumes no active Bootloadable application. Bootloader is active. */
    AesLoader_activeApp = AesLoader_MD_BTLDB_ACTIVE_NONE;

    /* Bootloadable app#1 is active */
    if (AesLoader_GetMetadata(AesLoader_GET_BTLDB_ACTIVE, AesLoader_MD_BTLDB_ACTIVE_0) == \
    AesLoader_MD_BTLDB_IS_ACTIVE)
    {
        /**********************************************************************************************************
        * ---------------------------------------------------------------------------------------------------------
        * |      |      app#1     |      app#2     |        Classic Dual-app       |     Launch-only Bootloader   |
        * | Case |---------------------------------|          Bootloader           |           (Launcher)         |
        * |      | Active | Valid | Active | Valid |                               |                              |
        * |-------------------------------------------------------------------------------------------------------|
        * |  9   |    1   |   0   |   0    |   0   | Bootloader                    | Launcher                     |
        * |  10  |    1   |   0   |   0    |   1   | app#2, except if auto-app     | app#2                        |
        * |      |        |       |        |       | switching is disabled, then   |                              |
        * |      |        |       |        |       | stay in Bootloader            |                              |
        * |  11  |    1   |   0   |   1    |   0   | Bootloader                    | Launcher                     |
        * |  12  |    1   |   0   |   1    |   1   | app#2                         | app#2                        |
        * |  13  |    1   |   1   |   0    |   0   | app#1                         | app#1                        |
        * |  14  |    1   |   1   |   0    |   1   | app#1                         | app#1                        |
        * |  15  |    1   |   1   |   1    |   0   | app#1                         | app#1                        |
        * |  16  |    1   |   1   |   1    |   1   | app#1                         | app#1                        |
        * ---------------------------------------------------------------------------------------------------------
        **********************************************************************************************************/
        if (CYRET_SUCCESS == app0Status)
        {
            /* Cases # 13,  14, 15, and 16 */
            AesLoader_activeApp = AesLoader_MD_BTLDB_ACTIVE_0;
            validApp = CYRET_SUCCESS;
        }
        else /*Bootloadable app#1 is not valid*/
        {
#if (((1u == AesLoader_AUTO_SWITCHING_AVAIL) &&  \
                      (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER)) || \
                      (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER))
            if (CYRET_SUCCESS == app1Status )
            {
                /* Cases # 10 and 12 */
                AesLoader_activeApp = AesLoader_MD_BTLDB_ACTIVE_1;
                validApp = CYRET_SUCCESS;
            }
#endif /*((1u == AesLoader_AUTO_SWITCHING_AVAIL) && 
                          (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER)) || 
                          (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER)*/
        }
    }

    /*  Active Bootloadable application is not identified */
    if (AesLoader_activeApp == AesLoader_MD_BTLDB_ACTIVE_NONE)
    {
        /**********************************************************************************************************
        * ---------------------------------------------------------------------------------------------------------
        * |      |       app#1    |       app#2    |        Classic Dual-app       |     Launch-only Bootloader   |
        * | Case |---------------------------------|          Bootloader           |           (Launcher)         |
        * |      | Active | Valid | Active | Valid |                               |                              |
        * |------|------------------------------------------------------------------------------------------------|
        * |  1   |    0   |   0   |   0    |   0   | Bootloader                    | Launcher                     |
        * |  2   |    0   |   0   |   0    |   1   | Bootloader                    | app#2                        |
        * |  3   |    0   |   0   |   1    |   0   | Bootloader                    | Launcher                     |
        * |  4   |    0   |   0   |   1    |   1   | app#2                         | app#2                        |
        * |  5   |    0   |   1   |   0    |   0   | Bootloader                    | app#1                        |
        * |  6   |    0   |   1   |   0    |   1   | Bootloader                    | app#1                        |
        * |  7   |    0   |   1   |   1    |   0   | app#1, except if auto-app     | app#1                        |
        * |      |        |       |        |       | switching is disabled, then   |                              |
        * |      |        |       |        |       | stay in Bootloader            |                              |
        * |  8   |    0   |   1   |   1    |   1   | app#2                         | app#2                        |
        * ---------------------------------------------------------------------------------------------------------
        **********************************************************************************************************/
        if (AesLoader_GetMetadata(AesLoader_GET_BTLDB_ACTIVE, 1u) ==
            AesLoader_MD_BTLDB_IS_ACTIVE)
        {
            /* Cases # 3, 4, 7, and 8 */
            if (CYRET_SUCCESS == app1Status)
            {
                /* Cases # 4 and 8 */
                AesLoader_activeApp = AesLoader_MD_BTLDB_ACTIVE_1;
                validApp = CYRET_SUCCESS;
            }
            else
            {
#if (((1u == AesLoader_AUTO_SWITCHING_AVAIL) && \
                       (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER)) \
                    || (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER))
                if (CYRET_SUCCESS == app0Status)
                {
                    /* Cases # 7 */
                    AesLoader_activeApp = AesLoader_MD_BTLDB_ACTIVE_0;
                    validApp = CYRET_SUCCESS;
                }
#endif /*(((1u == AesLoader_AUTO_SWITCHING_AVAIL) && \
                               (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER)) \
                            || (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER))*/
            }
        }
    }

#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER)
    /*None of applications is active*/
    if (AesLoader_MD_BTLDB_ACTIVE_NONE ==AesLoader_activeApp)
    {
        /*case#5, case#6: pass control to Bootloadable app#1*/
        if (CYRET_SUCCESS == app0Status)
        {
            AesLoader_activeApp = AesLoader_MD_BTLDB_ACTIVE_0;
            validApp = CYRET_SUCCESS;
        }
        /*case#2: pass control to Bootloadable app#2*/
        else if (CYRET_SUCCESS == app1Status)
        {
            AesLoader_activeApp = AesLoader_MD_BTLDB_ACTIVE_1;
            validApp = CYRET_SUCCESS;
        }
        else
        {
            /* case#1, case#3, case#9, case#11: very bad situation, no valid application */
            /* Do nothing, stay in Bootloader/Launcher */
        }
    }


    /*********************************************************************************
    * The next logic is an invalidation of the non-active application in the metadata
    * section, because there could be a situation due to some error, when both
    * applications are set active (cases #12,15,16). Only one application can be active.
    * This logic also stores the active application number in the metadata section.
    *********************************************************************************/
    if (CYRET_SUCCESS == validApp)
    {
        AesLoader_SetActiveAppInMetadata(AesLoader_activeApp);
    }
#endif /* CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LAUNCHER */
#else /*SINGLE_APP BOOTLOADER*/
    if (CYRET_SUCCESS == AesLoader_ValidateBootloadable(AesLoader_MD_BTLDB_ACTIVE_0))
    {
        validApp = CYRET_SUCCESS;
    }
#endif  /* (0u != AesLoader_DUAL_APP_BOOTLOADER) */


    /***********************************************************************
    * Bootloader Application Validation
    *
    * Halt the device if:
    *  - A calculated checksum does not match the one stored in the metadata
    *     section.
    *  - There is an invalid pointer to the place where the Bootloader
    *    application ends.
    *  - The flash subsystem was not initialized correctly.
    ***********************************************************************/
#if(0u != AesLoader_BOOTLOADER_APP_VALIDATION)

    /* Calculate Bootloader application checksum */
    calcedChecksum = AesLoader_Calc8BitSum(CY_FLASH_BASE,
                                           AesLoader_MD_BTLDR_ADDR_PTR,
                                           *AesLoader_SizeBytesAccess - AesLoader_MD_BTLDR_ADDR_PTR);

    /* We included checksum, so remove it */
    calcedChecksum -= *AesLoader_ChecksumAccess;
    calcedChecksum = ( uint8 )1u + ( uint8 )(~calcedChecksum);

    /* Checksum and pointer to Bootloader verification */
    if ((calcedChecksum != *AesLoader_ChecksumAccess) ||
        (0u == *AesLoader_SizeBytesAccess))
    {
        /***********************************************************************
        * If the Bootloader/Launcher application calculated checksum does not match
        * the checksum stored in special AesLoader_Checksum constant
        * (accessed through AesLoader_ChecksumAccess) constant or its size
        * stored in AesLoader_SizeBytes constant (accessed through
        * AesLoader_SizeBytesAccess) then CPU is halted, all software
        * processing stops.
        ***********************************************************************/
        CyHalt(0x00u);
    }

#endif /* 0u != AesLoader_BOOTLOADER_APP_VALIDATION */
#endif /* CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER*/

#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER)
    /***********************************************************************
    * If the active Bootloadable application is invalid or scheduled - do the following:
    *  - schedule the Bootloader application to be run after a software reset;
    *  - go to the communication subroutine. The HostLink() will wait for
    *    commands forever.
    ***********************************************************************/
    if ((AesLoader_GET_RUN_TYPE == AesLoader_SCHEDULE_BTLDR)
#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
        || (CYRET_SUCCESS != validApp)
#endif
       )
    {
        AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDR_INIT_STATE);

        AesLoader_HostLink(AesLoader_WAIT_FOR_COMMAND_FOREVER);
    }

    /* Go to communication subroutine. Will wait for commands for specified time */
#if(0u != AesLoader_WAIT_FOR_COMMAND)

    /* Timeout is in 100s of milliseconds */
    AesLoader_HostLink(AesLoader_WAIT_FOR_COMMAND_TIME);

#endif  /* (0u != AesLoader_WAIT_FOR_COMMAND) */
#else /* Launcher */
    /* If none Bootloadable application is valid */
    if (CYRET_SUCCESS != validApp)
    {
        CyHalt(0x00u);
    }
#endif /*CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER*/

#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
    /* Schedule Bootloadable application and perform software reset */
    AesLoader_LaunchApplication();
#endif /*(CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/
}

#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
/*******************************************************************************
* Function Name: AesLoader_LaunchApplication
****************************************************************************//**
* \internal
*
* \brief
*  Schedules the Bootloadable application and resets the device.
*
* \return
*  This method will never return.
*
* \endinternal
*******************************************************************************/
static void AesLoader_LaunchApplication(void) CYSMALL \

{
    /* Schedule Bootloadable to start after a reset */
    AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDB);

    CySoftwareReset();
}
#endif /*(CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/

/*******************************************************************************
* Function Name: AesLoader_Exit
****************************************************************************//**
*
*\brief
*  Schedules the specified application and performs a software reset to launch
*  a specified application.
*
*  If the specified application is not valid, the Bootloader (the result of the
*  ValidateBootloadable() function execution returns other than CYRET_SUCCESS,
*  the Bootloader application is launched.
*
* \param appId
*   The application to be started:
*   - AesLoader_EXIT_TO_BTLDR - The Bootloader application will be started on
*                                     a software reset.
*   - AesLoader_EXIT_TO_BTLDB;
*   - AesLoader_EXIT_TO_BTLDB_1 - Bootloadable application # 1 will be
*                                     started on a software reset.
*   - AesLoader_EXIT_TO_BTLDB_2 - Bootloadable application # 2 will be
*                                     started on a software reset. Available only
*                                     if the "Dual-application" option is enabled in
*                                     the component customizer.
* \return
*  This function never returns.
*
*******************************************************************************/
void AesLoader_Exit(uint8 appId) CYSMALL
{
    if (AesLoader_EXIT_TO_BTLDR == appId)
    {
        AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDR_INIT_STATE);
    }
    else
    {
        if (CYRET_SUCCESS == AesLoader_ValidateBootloadable(appId))
        {
            /* Set active application in metadata */
            uint8 CYDATA idx;
            for (idx = 0u; idx < AesLoader_MAX_NUM_OF_BTLDB; idx++)
            {
                AesLoader_SetFlashByte((uint32) AesLoader_MD_BTLDB_ACTIVE_OFFSET(idx),
                (uint8 )(idx == appId));
            }

#if((0u != AesLoader_DUAL_APP_BOOTLOADER) || \
            (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))
            AesLoader_activeApp = appId;
#endif /* (0u != AesLoader_DUAL_APP_BOOTLOADER) || 
                  (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))*/

            AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDB);
        }
        else
        {
            AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDR_INIT_STATE);
        }
    }

    CySoftwareReset();
}

#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
/*******************************************************************************
* Function Name: CyBtldr_CheckLaunch
****************************************************************************//**
* \internal
*
* \brief
*  This routine checks if the Bootloader or Bootloadable application has to
*  be run.  If the application has to be run, it will start executing.
*  If the Bootloader has to be run, it will return, so the Bootloader can
*  continue starting up.
*
* \return
*  It will not return if it determines that the Bootloadable application should
*  be run.
*
* \endinternal
*******************************************************************************/
void CyBtldr_CheckLaunch(void) CYSMALL
{

#if(CY_PSOC4)

    /*******************************************************************************
    * Set cyBtldrRunType to zero in the case of a non-software reset. This means
    * that the Bootloader application is scheduled - the initial clean state. The
    * value of cyBtldrRunType is valid only in the case of a software reset.
    *******************************************************************************/
    if (0u == (AesLoader_RES_CAUSE_REG & AesLoader_RES_CAUSE_RESET_SOFT))
    {
        cyBtldrRunType = AesLoader_SCHEDULE_BTLDR_INIT_STATE;
    }

#endif /* (CY_PSOC4) */


    if (AesLoader_GET_RUN_TYPE == AesLoader_SCHEDULE_BTLDB)
    {


#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER)
        /*******************************************************************************
        * Classic Single-app or Dual-app Bootloader use cases.
        * The active application number is taken from AesLoader_activeApp not
        * to break backward compatibility.
        *******************************************************************************/
        uint8 activeApp = AesLoader_activeApp;


        /*******************************************************************************
        * Classic Dual-app Bootloader with 2 Combination projects use case.
        * In this use case Classic Dual-app Bootloader is not the only one, who switches
        * between applications. So if there is no valid value for AesLoader_activeApp,
        * then we should additionally check the metadata if one of the Combination application
        * has set a value there.
        *******************************************************************************/
#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER)
        if (activeApp >= AesLoader_MD_BTLDB_ACTIVE_NONE)
        {
            activeApp = AesLoader_GetActiveAppFromMetadata();
        }
#endif /*(CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER)*/
#else
        /* Launcher use case.  The active application number is taken from metadata. */
        uint8 activeApp = AesLoader_GetActiveAppFromMetadata();
#endif /* (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER) */

        AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDR_INIT_STATE);

        /*******************************************************************************
        * Indicates that we have told ourselves to jump to the application because we have
        * already told ourselves to jump, we do not do any expensive verification of the
        * application. We just check to make sure that the value at CY_APP_ADDR_ADDRESS
        * is something other than 0 and active application is valid.
        *******************************************************************************/
        if ((0u != AesLoader_GetMetadata(AesLoader_GET_BTLDB_ADDR, activeApp))
#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_BOOTLOADER)
            && (activeApp < AesLoader_MD_BTLDB_ACTIVE_NONE)
#endif /* (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_BOOTLOADER) */
           )
        {
            /* Never returns from this method */
            AesLoader_LaunchBootloadable(AesLoader_GetMetadata(AesLoader_GET_BTLDB_ADDR,
                                         activeApp));
        }
    }
}


/* Moves argument appAddr (RO) into PC, moving execution to appAddr */
#if defined (__ARMCC_VERSION)

__asm static void AesLoader_LaunchBootloadable(uint32 appAddr)
{
    BX  R0
    ALIGN
}

#elif defined(__GNUC__)

__attribute__((noinline)) /* Workaround for GCC toolchain bug with inlining */
__attribute__((naked))
static void AesLoader_LaunchBootloadable(uint32 appAddr)
{
    __asm volatile("    BX  R0\n");
}

#elif defined (__ICCARM__)

static void AesLoader_LaunchBootloadable(uint32 appAddr)
{
    __asm volatile("    BX  R0\n");
}

#endif  /* (__ARMCC_VERSION) */
#endif /* (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/

/*******************************************************************************
* Function Name: AesLoader_ValidateBootloadable
****************************************************************************//**
* \brief
*  Performs the Bootloadable application validation by calculating the
*  application image checksum and comparing it with the checksum value stored
*  in the Bootloadable Application Checksum field of the metadata section.
*
*  If the "Fast bootloadable application validation" option is enabled in the
*  component customizer and Bootloadable application successfully passes
*  validation, the Bootloadable Application Verification Status field of the
*  metadata section is updated. Refer to the "Metadata Layout" section for the
*  details.
*
*  If the "Fast bootloadable application validation" option is enabled and
*  the Bootloadable Application Verification Status field of the metadata section
*  claims that the Bootloadable application is valid, the function returns
*  CYRET_SUCCESS without further checksum calculation.
*
*  \param appId
*  The number of the Bootloadable application should be 0 for the normal
*  bootloader and 0 or 1 for the dual-application bootloader.
*
* \return
*  CYRET_SUCCESS - If the specified the Bootloadable application is valid.
*  CYRET_BAD_DATA is returned if the input parameter is out of the specified range
*      or the calculated checksum does not match the stored checksum.
*******************************************************************************/
cystatus AesLoader_ValidateBootloadable(uint8 appId) CYSMALL \

{
    uint32 CYDATA idx;

    uint32 CYDATA end   = AesLoader_FIRST_APP_BYTE(appId) +
    AesLoader_GetMetadata(AesLoader_GET_BTLDB_LENGTH, appId);

    CYBIT  valid = 0u; /* Assume bad flash image */
    uint8  CYDATA calcedChecksum = 0u;


#if(0u != AesLoader_DUAL_APP_BOOTLOADER)

    if (appId > 1u)
    {
        return (CYRET_BAD_DATA);
    }

#endif  /* (0u != AesLoader_DUAL_APP_BOOTLOADER) */


#if(0u != AesLoader_FAST_APP_VALIDATION)
    if (AesLoader_GetMetadata(AesLoader_GET_BTLDB_STATUS, appId) ==
    AesLoader_MD_BTLDB_IS_VERIFIED)
    {
        return (CYRET_SUCCESS);
    }
#endif  /* (0u != AesLoader_FAST_APP_VALIDATION) */


    /* Calculate checksum of bootloadable image */
    for (idx = AesLoader_FIRST_APP_BYTE(appId); idx < end; ++idx)
    {
        uint8 CYDATA curByte = AesLoader_GET_CODE_BYTE(idx);

        if ((curByte != 0u) && (curByte != 0xFFu))
        {
            valid = 1u;
        }

        calcedChecksum += curByte;
    }


    /***************************************************************************
    * We do not compute a checksum over the meta data section, so no need to
    * subtract App Verified or App Active information here like we do when
    * verifying a row.
    ***************************************************************************/

#if((!CY_PSOC4) && (CYDEV_ECC_ENABLE == 0u))

    /* Add ECC data to checksum */
    idx = ((AesLoader_FIRST_APP_BYTE(appId)) >> 3u);

    /* Flash may run into meta data, so ECC does not use full row */
    end = (end == (CY_FLASH_SIZE - AesLoader_MD_SIZEOF))
          ? (CY_FLASH_SIZE >> 3u)
          : (end >> 3u);

    for (; idx < end; ++idx)
    {
        calcedChecksum += CY_GET_XTND_REG8((volatile uint8 *)(CYDEV_ECC_BASE + idx));
    }

#endif  /* ((!CY_PSOC4) && (CYDEV_ECC_ENABLE == 0u)) */

    calcedChecksum = ( uint8 )1u + ( uint8 )(~calcedChecksum);

    if ((calcedChecksum != AesLoader_GetMetadata(AesLoader_GET_BTLDB_CHECKSUM, appId)) ||
        (0u == valid))
    {
        return (CYRET_BAD_DATA);
    }


#if(0u != AesLoader_FAST_APP_VALIDATION)
    AesLoader_SetFlashByte((uint32) AesLoader_MD_BTLDB_VERIFIED_OFFSET(appId),
                           AesLoader_MD_BTLDB_IS_VERIFIED);
#endif  /* (0u != AesLoader_FAST_APP_VALIDATION) */


    return (CYRET_SUCCESS);
}

#if (((0u != AesLoader_DUAL_APP_BOOTLOADER) && (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER)) \
                                          || (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))
/*******************************************************************************
* Function Name: AesLoader_CheckImage
********************************************************************************
* \internal
*
* \brief
*   This API checks if there is a permission to write to a certain image.
*   It is used to check the active application as well as to check the Golden image.
*
* \param
*   appNumber - The application number.
* \param
*   arrayNumber - The number of the flash page to write to.
* \param
*   row - The row number to write to the flash.
* \param
*   rowNumInArray - The row number inside the array.
*
* \return
*   CYRET_SUCCESS - Writing to a specified image is permitted.
*   AesLoader_ERR_ACTIVE - Writing to this image is NOT permitted
*   because the specified image is active or Golden.
*
* \endinternal
*******************************************************************************/
static uint8 AesLoader_CheckImage(uint8 appNumber, uint8 arrayNumber, uint16 row, uint16 rowNumInArray) CYSMALL \

{
    uint16 CYDATA firstRow = 0xFFFFu;
    uint16 CYDATA lastRow = 0xFFFFu;
    uint8 CYDATA ackCode = CYRET_SUCCESS;

    if (appNumber < AesLoader_MAX_NUM_OF_BTLDB)
    {
        /*******************************************************************************
        * For the first Bootloadable application - gets the last flash row occupied by
        * the Bootloader application image:
        *  ---------------------------------------------------------------------------
        * | Bootloader   | Bootloadable # 1 |     | Bootloadable # 2 |     | M2 | M1 |
        *  ---------------------------------------------------------------------------
        * |<--firstRow---|>
        *
        * For the second Bootloadable application - gets the last flash row occupied by
        * the first Bootloadable application:
        *  ---------------------------------------------------------------------------
        * | Bootloader   | Bootloadable # 1 |     | Bootloadable # 2 |     | M2 | M1 |
        *  ---------------------------------------------------------------------------
        * |<-------------firstRow-----------------|>
        *
        * Incremented by 1 to get the first available row.
        *
        * NOTE M1 and M2 stand for metadata # 1 and metadata # 2, metadata
        * sections for the 1st and 2nd Bootloadable applications.
        *******************************************************************************/
        firstRow = (uint16) 1u +
        (uint16) AesLoader_GetMetadata(AesLoader_GET_BTLDR_LAST_ROW,  appNumber);


        /***********************************************************************************
        * If this is the Upgradable Stack use case, then Stack application and User application
        * do not occupy the half of flash each, as the other applications do for Classic Dual-app
        * and the general Launcher-Combination use cases. Another approach for calculation
        * lastRow is used for the Upgradable Stack use case. See Bootloader datasheet for more
        * details (use cases description).
        ***********************************************************************************/


#if ((CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) && \
         (0u != AesLoader_IS_STACK_APPLICATION))
        /*******************************************************************************
        * The Upgradable Stack application case implies that only this application can
        * perform a bootloading operation (the other one can't). So a verification
        * will be run to check if the Stack application is not overwritten.
        *
        * The Stack application is defined as the first application.
          * The User application is defined as the second application.
        *
        * This check is intended for the case when the Stack application is active
        * and performs bootloading for the User application:
        *  ---------------------------------------------------------------------------
        * |   Launcher   |       Stack     |           User app            | M2 | M1 |
        *  ---------------------------------------------------------------------------
        *                |<-------------------lastRow -------------------->|
        *******************************************************************************/
        lastRow = (uint16) AesLoader_GetMetadata(AesLoader_GET_BTLDR_LAST_ROW,
        AesLoader_USER_APPLICATION);

#else /* Classic Dual-app and the general Launcher-Combination use cases */
        /*******************************************************************************
        * The number of flash rows available for both Bootloadable applications:
        *
        * The first Bootloadable application is active:
        *  ---------------------------------------------------------------------------
        * | Bootloader   | Bootloadable # 1 |     | Bootloadable # 2 |     | M2 | M1 |
        *  ---------------------------------------------------------------------------
        *                |<-------------------lastRow -------------------->|
        *
        * The second Bootloadable application is active:
        *  ---------------------------------------------------------------------------
        * | Bootloader   | Bootloadable # 1 |     | Bootloadable # 2 |     | M2 | M1 |
        *  ---------------------------------------------------------------------------
        *                                         |<-------lastRow-------->|
        *******************************************************************************/
        lastRow = (uint16)(CY_FLASH_NUMBER_ROWS - AesLoader_NUMBER_OF_METADATA_ROWS - firstRow);

        /*******************************************************************************
        * The number of flash rows available for the active Bootloadable application:
        *
        * The first Bootloadable application is active: the number of flash rows available
        * for both Bootloadable applications should be divided by 2, meaning 2
        * Bootloadable applications should fit there.
        *
        * The second Bootloadable application is active: the number of flash rows available
        * for both Bootloadable applications should be divided by 1, meaning 1
        * Bootloadable application should fit there.
        *******************************************************************************/
        lastRow = (uint16)(lastRow / (uint8)(AesLoader_NUMBER_OF_BTLDBLE_APPS - appNumber));

        /*******************************************************************************
        * The last row equals the first row plus the number of rows available for
        * each Bootloadable application. That gives the flash row number right
        * beyond the valid range, so we subtract 1.
        *
        * The first Bootloadable application is active:
        *  ---------------------------------------------------------------------------
        * | Bootloader   | Bootloadable # 1 |     | Bootloadable # 2 |     | M2 | M1 |
        *  ---------------------------------------------------------------------------
        * |<----------------lastRow ------------->|
        *
        * The second Bootloadable application is active:
        *  ---------------------------------------------------------------------------
        * | Bootloader   | Bootloadable # 1 |     | Bootloadable # 2 |     | M2 | M1 |
        *  ---------------------------------------------------------------------------
        * |<-----------------------------lastRow-------------------------->|
        *******************************************************************************/
        lastRow = (firstRow + lastRow) - 1u;
#endif /*(0u == AesLoader_IS_STACK_APPLICATION)*/

        /*******************************************************************************
        * 1. Refuses to write a row within the range of the active application.
        *
        *  The first Bootloadable application is active:
        *   ---------------------------------------------------------------------------
        *  | Bootloader   | Bootloadable # 1 |     | Bootloadable # 2 |     | M2 | M1 |
        *   ---------------------------------------------------------------------------
        *  |<----------------lastRow ------------->|
        *  |<--firstRow---|>
        *                 |<-------protected------>|
        *
        *  the second Bootloadable application is active:
        *   ---------------------------------------------------------------------------
        *  | Bootloader   | Bootloadable # 1 |     | Bootloadable # 2 |     | M2 | M1 |
        *   ---------------------------------------------------------------------------
        *  |<-------------firstRow-----------------|>
        *  |<-----------------------------lastRow-------------------------->|
        *                                          |<-------protected------>|
        *
        * 2. Refuses to write to the row that contains metadata of the active
        *    Bootloadable application.
        *
        *******************************************************************************/
        if (((row >= firstRow) && (row <= lastRow)) || ((arrayNumber == AesLoader_MD_FLASH_ARRAY_NUM) && \
        (rowNumInArray == AesLoader_MD_ROW_NUM(appNumber))))
        {
            ackCode = AesLoader_ERR_ACTIVE;
        }
    }
    else /*(appNumber < AesLoader_MAX_NUM_OF_BTLDB)*/
    {
        ackCode = AesLoader_ERR_ACTIVE;
    }

    return ackCode;
}
#endif  /* (0u != AesLoader_DUAL_APP_BOOTLOADER) || \
           (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) */

#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER)
/*******************************************************************************
* Function Name: AesLoader_HostLink
****************************************************************************//**
*
* \brief
*  Causes the Bootloader to attempt to read data being transmitted by the
*  host application.  If data is sent from the host, this establishes the
*  communication interface to process all requests.
*
* \param
*  timeOut:
*   The amount of time to listen for data before giving up. The timeout is
*   measured in 10s of ms.  Use 0 for an infinite wait.
*
* \details
*  This function is public only for Launcher-Combination architecture. For
*  Classic Bootloader it is static, meaning private.
*******************************************************************************/
#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
    static void AesLoader_HostLink(uint8 timeOut) CYLARGE

#else
    void AesLoader_HostLink(uint8 timeOut) CYLARGE

#endif /*(CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/
{
    uint16    CYDATA numberRead;
    uint16    CYDATA rspSize;
    uint8     CYDATA ackCode;
    uint16    CYDATA pktChecksum;
    cystatus  CYDATA readStat;
    uint16    CYDATA pktSize    = 0u;
    uint8     CYDATA timeOutCnt = 10u;

#if(0u != AesLoader_FAST_APP_VALIDATION)
    uint8 CYDATA clearedMetaData = 0u;
#endif  /* (0u != AesLoader_FAST_APP_VALIDATION) */

#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
    static  CYBIT communicationState = AesLoader_COMMUNICATION_STATE_IDLE;
    static uint16 CYDATA dataOffset = 0u;
    uint16  CYDATA btldrLastRow = 0xFFFFu;
    uint8 needToCopyFlag = 0u;
    uint32 app2StartAddress = AesLoader_GetMetadata(AesLoader_GET_BTLDB_ADDR,
    AesLoader_MD_BTLDB_ACTIVE_1);
#else
    CYBIT     communicationState = AesLoader_COMMUNICATION_STATE_IDLE;
    uint16 CYDATA dataOffset = 0u;
#endif /*CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER*/

    uint8     packetBuffer[AesLoader_SIZEOF_COMMAND_BUFFER];
    uint8     dataBuffer  [AesLoader_SIZEOF_COMMAND_BUFFER];


#if(!CY_PSOC4)
#if(0u == AesLoader_FAST_APP_VALIDATION)
#if !defined(CY_BOOT_VERSION)

    /* Not required with cy_boot 4.20 */
    uint8 CYXDATA AesLoader_flashBuffer[AesLoader_FROW_SIZE];

#endif /* !defined(CY_BOOT_VERSION) */
#endif  /* (0u == AesLoader_FAST_APP_VALIDATION) */
#endif  /* (CY_PSOC4) */



#if(!CY_PSOC4)
#if((0u == AesLoader_FAST_APP_VALIDATION) || \
            (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))

    /* Initialize flash subsystem for non-PSoC 4 devices */
    if (CYRET_SUCCESS != CySetTemp())
    {
        CyHalt(0x00u);
    }

#endif  /* (0u == AesLoader_FAST_APP_VALIDATION) ||
                   (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) */
#endif  /* (CY_PSOC4) */

    /* Initialize communications channel. */
    CyBtldrCommStart();

    /* Enable global interrupts */
    CyGlobalIntEnable;

    do
    {
        ackCode = CYRET_SUCCESS;

        do
        {
            readStat = CyBtldrCommRead(packetBuffer,
            AesLoader_SIZEOF_COMMAND_BUFFER,
            &numberRead,
            (0u == timeOut) ? 0xFFu : timeOut);
            if (0u != timeOut)
            {
                timeOutCnt--;
            }

        }
        while ( (0u != timeOutCnt) && (readStat != CYRET_SUCCESS) );


        if ( readStat != CYRET_SUCCESS )
        {
            continue;
        }

        if ((numberRead < AesLoader_MIN_PKT_SIZE) ||
        (packetBuffer[AesLoader_SOP_ADDR] != AesLoader_SOP))
        {
            ackCode = AesLoader_ERR_DATA;
        }
        else
        {
            pktSize = ((uint16)((uint16)packetBuffer[AesLoader_SIZE_ADDR + 1u] << 8u)) |
            packetBuffer[AesLoader_SIZE_ADDR];


            /****************************************************************************************
            * If the whole packet length exceeds the number of bytes that have been read by the communication
            * component or the size of the buffer that is reserved for the packet, then give an error.
            **************************************************************************************/
            if (((pktSize + AesLoader_MIN_PKT_SIZE) > numberRead) ||
            ((pktSize + AesLoader_MIN_PKT_SIZE) > AesLoader_SIZEOF_COMMAND_BUFFER))
            {
                ackCode = AesLoader_ERR_LENGTH;
            }
            else /* Packet length is OK*/
            {
                pktChecksum = ((uint16)((uint16)packetBuffer[AesLoader_CHK_ADDR(pktSize) + 1u] << 8u)) |
                packetBuffer[AesLoader_CHK_ADDR(pktSize)];

                if (packetBuffer[AesLoader_EOP_ADDR(pktSize)] != AesLoader_EOP)
                {
                    ackCode = AesLoader_ERR_DATA;
                }
                else if (pktChecksum != AesLoader_CalcPacketChecksum(packetBuffer,
                pktSize + AesLoader_DATA_ADDR))
                {
                    ackCode = AesLoader_ERR_CHECKSUM;
                }
                else
                {
                    /* Empty section */
                }
            }
        }

        rspSize = AesLoader_RSP_SIZE_0;
        if (ackCode == CYRET_SUCCESS)
        {
            uint8 CYDATA btldrData = packetBuffer[AesLoader_DATA_ADDR];

            ackCode = AesLoader_ERR_DATA;
            switch (packetBuffer[AesLoader_CMD_ADDR])
            {


                    /***************************************************************************
                    *   Get metadata (0x3C)
                    ***************************************************************************/
#if(0u != AesLoader_CMD_GET_METADATA)

                case AesLoader_COMMAND_GET_METADATA:

                    if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (pktSize == 1u))
                    {
                        if (btldrData >= AesLoader_MAX_NUM_OF_BTLDB)
                        {
                            ackCode = AesLoader_ERR_APP;
                        }
                        else if (CYRET_SUCCESS == AesLoader_ValidateBootloadable(btldrData))
                        {
#if(CY_PSOC3)
                            (void) memcpy(&packetBuffer[AesLoader_DATA_ADDR],
                                          ((uint8  CYCODE *) (AesLoader_MD_BASE_ADDR(btldrData))),
                                          AesLoader_GET_METADATA_RESPONSE_SIZE);
#else
                            (void) memcpy(&packetBuffer[AesLoader_DATA_ADDR],
                                          (uint8 *) AesLoader_MD_BASE_ADDR(btldrData),
                                          AesLoader_GET_METADATA_RESPONSE_SIZE);
#endif  /* (CY_PSOC3) */

                            rspSize = AesLoader_RSP_SIZE_GET_METADATA;
                            ackCode = CYRET_SUCCESS;
                        }
                        else
                        {
                            ackCode = AesLoader_ERR_APP;
                        }
                    }
                    break;

#endif  /* (0u != AesLoader_CMD_GET_METADATA) */


                    /***************************************************************************
                    *   Verify application checksum (0x31)
                    ***************************************************************************/
#if (0u != AesLoader_CMD_VERIFY_APP_CHKSUM_AVAIL)
                case AesLoader_COMMAND_CHECKSUM:

                    if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (pktSize == 0u))
                    {
                        packetBuffer[AesLoader_DATA_ADDR] =
                            (uint8)(AesLoader_ValidateBootloadable(AesLoader_activeApp) == CYRET_SUCCESS);

                        rspSize = AesLoader_RSP_SIZE_VERIFY_CHKSM;
                        ackCode = CYRET_SUCCESS;
                    }
                    break;
#endif /* (0u != AesLoader_CMD_VERIFY_APP_CHKSUM_AVAIL) */

                    /*****************************************************************************
                    *   Verify row (0x45u)
                    ***************************************************************************/
#if(0u != AesLoader_CMD_VERIFY_FLS_ROW_AVAIL)
                case AesLoader_COMMAND_VERIFY_FLS_ROW:

                    /* Packet size is either 3 (data is already in buffer) or (3u + AesLoader_FROW_SIZE),
                     * then data is in packetBuffer[]*/
                    if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && ((3u == pktSize) ||
                            ((3u + AesLoader_FROW_SIZE) == pktSize)))
                    {
                        uint16 CYDATA rowNum = ((uint16)((uint16)packetBuffer[AesLoader_DATA_ADDR + 2u] << 8u)) |
                                               packetBuffer[AesLoader_DATA_ADDR + 1u];

                        uint32 CYDATA startAddr;
                        uint16 upperRange = 0u;
#if (!CY_PSOC4)
                        uint16 CYDATA rowSize;
#endif /*(!CY_PSOC4)*/

                        ackCode = CYRET_SUCCESS;
                        rspSize = AesLoader_RSP_SIZE_0;

                        if (3u == pktSize)
                        {
                            /* Do nothing, data is already in dataBuffer[] */
                        }
                        else
                        {
#if (CY_PSOC3)
                            /*Data is in packetBuffer[], needs to be copied to dataBuffer[]*/
                            (void) memcpy(dataBuffer, &packetBuffer[AesLoader_DATA_ADDR + 3u],
                                          (int16)pktSize - 3);
#else
                            /*Data is in packetBuffer[], needs to be copied to dataBuffer[]*/
                            (void) memcpy(dataBuffer, &packetBuffer[AesLoader_DATA_ADDR + 3u],
                                          (uint32)pktSize - 3u);
#endif /*CY_PSOC3)*/
                        }

#if (!CY_PSOC4)
                        if ((btldrData >= AesLoader_FIRST_EE_ARRAYID) &&
                            (btldrData <= AesLoader_LAST_EE_ARRAYID))
                        {
                            /* EEPROM */
                            /* Both PSoC 3 and PSoC 5LP architecture have one EEPROM array. */
                            startAddr = CY_EEPROM_BASE + ((uint32)rowNum * CYDEV_EEPROM_ROW_SIZE);
                            rowSize = CYDEV_EEPROM_ROW_SIZE;
                            upperRange = AesLoader_NUMBER_OF_EEPROM_SECTORS * AesLoader_NUMBER_OF_ROWS_IN_EEPROM_SECTOR;
                        }
                        else if (btldrData < CY_FLASH_NUMBER_ARRAYS)
                        {
                            startAddr = CY_FLASH_BASE + ((uint32)btldrData * CYDEV_FLS_SECTOR_SIZE) +
                                        ((uint32)rowNum * CYDEV_FLS_ROW_SIZE);
                            rowSize = CYDEV_FLS_ROW_SIZE;
                            upperRange = AesLoader_NUMBER_OF_ROWS_IN_ARRAY;
                        }
                        else
                        {
                            ackCode = AesLoader_ERR_ARRAY;
                            break;
                        }

                        /*Checking if row number is within array address range*/
                        ackCode = AesLoader_CHECK_ROW_NUMBER(rowNum, upperRange);

                        if (CYRET_SUCCESS != ackCode)
                        {
                            break;
                        }

                        ackCode = AesLoader_VerifyRow(startAddr, dataBuffer, rowSize);

                        /*******************************************************************************
                        * If flash row check has completed successfully and ECC is disabled and there is
                        * data in the buffer for ECC row.
                        *******************************************************************************/
#if (CYDEV_ECC_ENABLE == 0u)
                        if ((CYRET_SUCCESS == ackCode) && (btldrData < CY_FLASH_NUMBER_ARRAYS) && (dataOffset > rowSize))
                        {
                            startAddr = CYDEV_ECC_BASE + ((uint32)btldrData * (CYDEV_FLS_SECTOR_SIZE / 8u)) +
                                        ((uint32)rowNum * CYDEV_ECC_ROW_SIZE);

                            ackCode = AesLoader_VerifyRow(startAddr, &dataBuffer[rowSize], (uint16)CYDEV_ECC_ROW_SIZE);
                        }
#endif /*(CYDEV_ECC_ENABLE == 0u)*/

#else /*CY_PSOC4*/
                        if (btldrData < CY_FLASH_NUMBER_ARRAYS)
                        {
                            startAddr = CY_FLASH_BASE + ((uint32)btldrData * CYDEV_FLS_SECTOR_SIZE)
                                        + ((uint32)rowNum * CYDEV_FLS_ROW_SIZE);

                            upperRange = AesLoader_NUMBER_OF_ROWS_IN_ARRAY;

                            /*Checking if row number is within array address range*/
                            ackCode = AesLoader_CHECK_ROW_NUMBER(rowNum, upperRange);

                            if (CYRET_SUCCESS != ackCode)
                            {
                                break;
                            }

                            ackCode = AesLoader_VerifyRow(startAddr, dataBuffer, (uint16)CYDEV_FLS_ROW_SIZE);
                        }
                        else
                        {
                            ackCode = AesLoader_ERR_ARRAY;
                            break;
                        }
#endif  /* (!CY_PSOC4) */

                    }
                    break;
#endif /*(0u != AesLoader_CMD_VERIFY_ROW_FLS_AVAIL)*/

                    /***************************************************************************
                    *   Get flash size (0x32)
                    ***************************************************************************/
#if(0u != AesLoader_CMD_GET_FLASH_SIZE_AVAIL)

                case AesLoader_COMMAND_REPORT_SIZE:

                    /* btldrData - holds flash array ID sent by host */
                    if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (pktSize == 1u))
                    {
                        if (btldrData < CY_FLASH_NUMBER_ARRAYS)
                        {
                            uint16 CYDATA startRow;
                            uint8  CYDATA arrayIdBtlderEnds;

                            /*******************************************************************************
                            * - For the flash array where the Bootloader application ends, returns the first
                            *   full row after the Bootloader application.
                            *
                            * - For a fully occupied flash array, the number of rows in the array is returned
                            *   because there is no space for the Bootloadable application in this array.
                            *
                            * - For the arrays next to the occupied array, zero is returned.
                            *   The Bootloadable application can be written from those arrays beginning.
                            *
                            *   If this is a Bootloader that is located in an application (Combination project type),
                            *   then we do not use the AesLoader_SizeBytes and AesLoader_SizeBytesAccess
                            *   variables, instead we take the Launcher's last row from metadata.
                            *******************************************************************************/
#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
                            arrayIdBtlderEnds = (uint8) (*AesLoader_SizeBytesAccess / CY_FLASH_SIZEOF_ARRAY);

                            if (btldrData == arrayIdBtlderEnds)
                            {
                                startRow = (uint16) (*AesLoader_SizeBytesAccess / CY_FLASH_SIZEOF_ROW) %
                                           AesLoader_NUMBER_OF_ROWS_IN_ARRAY;
                            }
#else /*Combination project type*/
                            btldrLastRow = (uint16)(AesLoader_GetMetadata(AesLoader_GET_BTLDR_LAST_ROW, \
                                                                          AesLoader_MD_BTLDB_ACTIVE_0));
                            arrayIdBtlderEnds = (uint8) (((uint32)btldrLastRow * CY_FLASH_SIZEOF_ROW) / (uint32)CY_FLASH_SIZEOF_ARRAY);
                            if (btldrData == arrayIdBtlderEnds)
                            {
                                startRow = (uint16)((btldrLastRow) % AesLoader_NUMBER_OF_ROWS_IN_ARRAY);
                            }
#endif /*(CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/

                            else if (btldrData > arrayIdBtlderEnds)
                            {
                                startRow = AesLoader_FIRST_ROW_IN_ARRAY;
                            }
                            else /* (btldrData < ArrayIdBtlderEnds) */
                            {
                                startRow = AesLoader_NUMBER_OF_ROWS_IN_ARRAY;
                            }

                            packetBuffer[AesLoader_DATA_ADDR]      = LO8(startRow);
                            packetBuffer[AesLoader_DATA_ADDR + 1u] = HI8(startRow);

                            packetBuffer[AesLoader_DATA_ADDR + 2u] =
                                LO8(AesLoader_NUMBER_OF_ROWS_IN_ARRAY - 1u);

                            packetBuffer[AesLoader_DATA_ADDR + 3u] =
                                HI8(AesLoader_NUMBER_OF_ROWS_IN_ARRAY - 1u);

                            rspSize = AesLoader_RSP_SIZE_GET_FLASH_SIZE;
                            ackCode = CYRET_SUCCESS;
                        }

                    }
                    break;

#endif  /* (0u != AesLoader_CMD_GET_FLASH_SIZE_AVAIL) */


                    /***************************************************************************
                    *   Get application status (0x33)
                    ***************************************************************************/
#if ((0u != AesLoader_DUAL_APP_BOOTLOADER) || (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))

#if(0u != AesLoader_CMD_GET_APP_STATUS_AVAIL)

                case AesLoader_COMMAND_APP_STATUS:

                    if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (1u == pktSize))
                    {

                        packetBuffer[AesLoader_DATA_ADDR] =
                            (uint8)AesLoader_ValidateBootloadable(btldrData);

                        packetBuffer[AesLoader_DATA_ADDR + 1u] =
                            (uint8) AesLoader_GetMetadata(AesLoader_GET_BTLDB_ACTIVE, btldrData);

                        rspSize = AesLoader_RSP_SIZE_GET_APP_STATUS;
                        ackCode = CYRET_SUCCESS;
                    }
                    break;

#endif  /* (0u != AesLoader_CMD_GET_APP_STATUS_AVAIL) */

#endif  /* (0u != AesLoader_DUAL_APP_BOOTLOADER || (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)) */

                /***************************************************************************
                *   Program / Erase row (0x39 / 0x34)
                ***************************************************************************/
                case AesLoader_COMMAND_PROGRAM:

                    /* btldrData variable holds Flash Array ID */

#if (0u != AesLoader_CMD_ERASE_ROW_AVAIL)

                case AesLoader_COMMAND_ERASE:
                    if (AesLoader_COMMAND_ERASE == packetBuffer[AesLoader_CMD_ADDR])
                    {
                        if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (pktSize == 3u))
                        {
#if(!CY_PSOC4)
                            if ((btldrData >= AesLoader_FIRST_EE_ARRAYID) &&
                                (btldrData <= AesLoader_LAST_EE_ARRAYID))
                            {
                                /* Size of EEPROM row */
                                dataOffset = CY_EEPROM_SIZEOF_ROW;
                            }
                            else
                            {
                                /* Size of flash row (depends on ECC configuration) */
                                dataOffset = AesLoader_FROW_SIZE;
                            }
#else
                            /* Size of flash row (no ECC available) */
                            dataOffset = AesLoader_FROW_SIZE;
#endif  /* (!CY_PSOC4) */

#if(CY_PSOC3)
                            (void) memset(dataBuffer, (char8) 0, (int16) dataOffset);
#else
                            (void) memset(dataBuffer, 0, (uint32) dataOffset);
#endif  /* (CY_PSOC3) */
                        }
                        else
                        {
                            break;
                        }
                    }

#endif  /* (0u != AesLoader_CMD_ERASE_ROW_AVAIL) */


                    if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (pktSize >= 3u))
                    {
                        uint16 upperRange;

                        /* Command may be sent along with last block of data, to program row. */
#if(CY_PSOC3)
                        (void) memcpy(&dataBuffer[dataOffset],
                                      &packetBuffer[AesLoader_DATA_ADDR + 3u],
                                      (int16) pktSize - 3);
#else
                        (void) memcpy(&dataBuffer[dataOffset],
                                      &packetBuffer[AesLoader_DATA_ADDR + 3u],
                                      (uint32) pktSize - 3u);
#endif  /* (CY_PSOC3) */

                        dataOffset += (pktSize - 3u);

#if(!CY_PSOC4)
                        if ((btldrData >= AesLoader_FIRST_EE_ARRAYID) &&
                            (btldrData <= AesLoader_LAST_EE_ARRAYID))
                        {
                            CyEEPROM_Start();

                            /* Size of EEPROM row */
                            pktSize = CY_EEPROM_SIZEOF_ROW;
                            upperRange = AesLoader_NUMBER_OF_EEPROM_SECTORS * AesLoader_NUMBER_OF_ROWS_IN_EEPROM_SECTOR;
                        }
                        else
                        {
                            /* Size of flash row (depends on ECC configuration) */
                            pktSize = AesLoader_FROW_SIZE;
                            upperRange = AesLoader_NUMBER_OF_ROWS_IN_ARRAY;
                        }
#else
                        /* Size of flash row (no ECC available) */
                        pktSize = AesLoader_FROW_SIZE;
                        upperRange = AesLoader_NUMBER_OF_ROWS_IN_ARRAY;
#endif  /* (!CY_PSOC4) */

                        /* Check if we have all data to program */
                        if (dataOffset == pktSize)
                        {
                            uint16 row;
                            uint16 firstRow;
#if ((0u != AesLoader_DUAL_APP_BOOTLOADER) && \
                          (1u == AesLoader_GOLDEN_IMAGE_AVAIL))
                            cystatus goldenImageValid;
#endif /*(((0u != AesLoader_DUAL_APP_BOOTLOADER) &&
                               (1u == AesLoader_GOLDEN_IMAGE_AVAIL))*/

                            /* Get flash/EEPROM row number inside array */
                            dataOffset = ((uint16)((uint16)packetBuffer[AesLoader_DATA_ADDR + 2u] << 8u)) |
                                         packetBuffer[AesLoader_DATA_ADDR + 1u];

                            /*Checking if row number is within array address range*/
                            ackCode = AesLoader_CHECK_ROW_NUMBER(dataOffset, upperRange);

                            if (CYRET_SUCCESS != ackCode)
                            {
                                break;
                            }

                            /* Metadata section resides in flash (cannot be in EEPROM). */
#if(!CY_PSOC4)
                            if (btldrData <= AesLoader_LAST_FLASH_ARRAYID)
                            {
#endif  /* (!CY_PSOC4) */

                                /* btldrData  - holds flash array Id sent by host */
                                /* dataOffset - holds flash row Id sent by host   */
                                row = (uint16)(btldrData * AesLoader_NUMBER_OF_ROWS_IN_ARRAY) + dataOffset;

                                /*******************************************************************************
                                * Refuse to write to the row within range of the bootloader application
                                *******************************************************************************/

                                /* First empty flash row after Bootloader application */
#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
                                firstRow = (uint16) (*AesLoader_SizeBytesAccess / CYDEV_FLS_ROW_SIZE);
                                if ((*AesLoader_SizeBytesAccess % CYDEV_FLS_ROW_SIZE) != 0u)
                                {
                                    firstRow++;
                                }
#else /*Combination project type*/

                                if (AesLoader_RUNNING_APPLICATION_0 == AesLoader_runningApp)
                                {
                                    btldrLastRow = (uint16)AesLoader_GetMetadata(AesLoader_GET_BTLDR_LAST_ROW,
                                                                                 AesLoader_MD_BTLDB_ACTIVE_1);
                                }
                                else /*(AesLoader_RUNNING_APPLICATION_1 == AesLoader_runningApp)*/
                                {
                                    btldrLastRow = (uint16)AesLoader_GetMetadata(AesLoader_GET_BTLDR_LAST_ROW,
                                                                                 AesLoader_MD_BTLDB_ACTIVE_0);
                                }

                                firstRow = (uint16)(btldrLastRow + 1u);
#endif /*(CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/

                                /* Check to see if a row to be programmed will not corrupt the Bootloader application */
                                if (row < firstRow)
                                {
                                    ackCode = AesLoader_ERR_ROW;
                                    dataOffset = 0u;
                                    break;
                                }

#if ((0u != AesLoader_DUAL_APP_BOOTLOADER) || \
                             (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))

                                /************************************************************************************
                                * No check is required in case when there is no app#1 present in Upgradable Stack
                                * use case (no user application downloaded yet).
                                ************************************************************************************/
#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
                                if (0u != app2StartAddress)
                                {
#endif /*(CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/

                                    if (AesLoader_activeApp < AesLoader_MD_BTLDB_ACTIVE_NONE)
                                    {
                                        /* Refuse to write to active image */
                                        ackCode = AesLoader_CheckImage(AesLoader_activeApp,
                                                                       btldrData, /*array number*/
                                                                       row,  /*row number*/
                                                                       dataOffset); /*row number in scope of array*/

                                        if (AesLoader_ERR_ACTIVE == ackCode)
                                        {
                                            dataOffset = 0u;
                                            break;
                                        }
                                    }
#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
                                }
#endif /*(CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/

                                /*This option is available for classic Dual-app bootloader only*/
#if ((0u != AesLoader_DUAL_APP_BOOTLOADER) && \
                                 (1u == AesLoader_GOLDEN_IMAGE_AVAIL))
                                /* Check if this is an attempt to overwrite Golden image */
                                ackCode = AesLoader_CheckImage(AesLoader_GOLDEN_IMAGE,
                                                               btldrData, /*array number*/
                                                               row, /*row number*/
                                                               dataOffset); /*row number in scope of array*/


                                /************************************************************************
                                * If an image to write is intended to update the Golden image and Golden image
                                * itself is valid, then give an error that it is active/Golden, otherwise
                                * (invalid Golden image) let the Golden image to be overwritten.
                                *************************************************************************/
                                goldenImageValid = AesLoader_ValidateBootloadable(AesLoader_GOLDEN_IMAGE);
                                if ((AesLoader_ERR_ACTIVE == ackCode) && (CYRET_SUCCESS == goldenImageValid))
                                {
                                    dataOffset = 0u;
                                    break;
                                }
                                else /* It is OK to overwrite corrupted Golden image */
                                {
                                    ackCode = CYRET_SUCCESS;
                                }
#endif /*((0u != AesLoader_DUAL_APP_BOOTLOADER) &&
                                      (1u == AesLoader_GOLDEN_IMAGE_AVAIL)) */
#endif /*(0u != AesLoader_DUAL_APP_BOOTLOADER) ||
                                 (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/


                                /*******************************************************************************
                                * Clear a row that contains metadata, when the "Fast bootloadable application
                                * validation" option is enabled.
                                *
                                * If the "Fast bootloadable application validation" option is enabled, the
                                * bootloader only computes the checksum the first time and assumes that it
                                * remains valid in each future startup. The metadata row is cleared because the
                                * Bootloadable application might become corrupted during update, while
                                * the "Bootloadable Application Verification Status" field will still report that
                                * the application is valid.
                                *******************************************************************************/
#if(0u != AesLoader_FAST_APP_VALIDATION)

                                if (0u == clearedMetaData)
                                {
                                    /* Metadata section must be filled with zeros */

                                    uint8 erase[AesLoader_FROW_SIZE];
                                    uint8 AesLoader_notActiveApp;


#if(CY_PSOC3)
                                    (void) memset(erase, (char8) 0, (int16) AesLoader_FROW_SIZE);
#else
                                    (void) memset(erase, 0, AesLoader_FROW_SIZE);
#endif  /* (CY_PSOC3) */


#if ((0u != AesLoader_DUAL_APP_BOOTLOADER) || \
                                     (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))
                                    if (AesLoader_MD_BTLDB_ACTIVE_0 == AesLoader_activeApp)
                                    {
                                        AesLoader_notActiveApp = AesLoader_MD_BTLDB_ACTIVE_1;
                                    }
                                    else
                                    {
                                        AesLoader_notActiveApp = AesLoader_MD_BTLDB_ACTIVE_0;
                                    }
#else
                                    AesLoader_notActiveApp = AesLoader_MD_BTLDB_ACTIVE_0;
#endif /* (0u != AesLoader_DUAL_APP_BOOTLOADER) */


#if(CY_PSOC4)
                                    (void) CySysFlashWriteRow(
                                        AesLoader_MD_ROW_NUM(AesLoader_notActiveApp),
                                        erase);
#else
                                    (void) CyWriteRowFull(
                                        (uint8)  AesLoader_MD_FLASH_ARRAY_NUM,
                                        (uint16) AesLoader_MD_ROW_NUM(AesLoader_notActiveApp),
                                        erase,
                                        AesLoader_FROW_SIZE);
#endif  /* (CY_PSOC4) */

                                    /* PSoC 5: Do not care about flushing cache as flash row has been erased. */

                                    /* Set up flag that metadata was cleared */
                                    clearedMetaData = 1u;
                                }

#endif  /* (0u != AesLoader_FAST_APP_VALIDATION) */


#if(!CY_PSOC4)
                            }   /* (btldrData <= AesLoader_LAST_FLASH_ARRAYID) */
#endif  /* (!CY_PSOC4) */


#if(CY_PSOC4)
                            ackCode = (CYRET_SUCCESS != CySysFlashWriteRow((uint32) row, dataBuffer)) \
                                      ? AesLoader_ERR_ROW \
                                      : CYRET_SUCCESS;
#else
                            ackCode = (CYRET_SUCCESS != CyWriteRowFull(btldrData, dataOffset, dataBuffer, pktSize)) \
                                      ? AesLoader_ERR_ROW \
                                      : CYRET_SUCCESS;
#endif  /* (CY_PSOC4) */


#if(CY_PSOC5)
                            /***************************************************************************
                            * When writing to flash, data in the instruction cache can become stale.
                            * Therefore, the cache data does not correlate to the data just written to
                            * flash. A call to CyFlushCache() is required to invalidate the data in the
                            * cache and force fresh information to be loaded from flash.
                            ***************************************************************************/
                            CyFlushCache();
#endif /* (CY_PSOC5) */

                        }
                        else
                        {
                            ackCode = AesLoader_ERR_LENGTH;
                        }

                        dataOffset = 0u;
                    }
                    break;


                    /***************************************************************************
                    *   Sync bootloader (0x35)
                    ***************************************************************************/
#if(0u != AesLoader_CMD_SYNC_BOOTLOADER_AVAIL)

                case AesLoader_COMMAND_SYNC:

                    if (AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState)
                    {
                        /* If something failed, Host would send this command to reset Bootloader. */
                        dataOffset = 0u;

                        /* Don't acknowledge the packet, just get ready to accept the next one */
                        continue;
                    }
                    break;

#endif  /* (0u != AesLoader_CMD_SYNC_BOOTLOADER_AVAIL) */


                    /***************************************************************************
                    *   Set an active application (0x36)
                    ***************************************************************************/
#if ((0u != AesLoader_DUAL_APP_BOOTLOADER) || \
                 (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER))

                case AesLoader_COMMAND_APP_ACTIVE:

                    if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (pktSize == 1u))
                    {
                        if (CYRET_SUCCESS == AesLoader_ValidateBootloadable(btldrData))
                        {
                            AesLoader_SetActiveAppInMetadata(btldrData);
                            AesLoader_activeApp = btldrData;
                            ackCode = CYRET_SUCCESS;
                        }
                        else
                        {
                            ackCode = AesLoader_ERR_APP;
                        }
                    }
                    break;

#endif  /* (0u != AesLoader_DUAL_APP_BOOTLOADER) || 
                       (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) */


                    /***************************************************************************
                    *   Send data (0x37)
                    ***************************************************************************/
#if (0u != AesLoader_CMD_SEND_DATA_AVAIL)

                case AesLoader_COMMAND_DATA:

                    if (AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState)
                    {
                        /*  Make sure that dataOffset is valid before copying data */
                        if ((dataOffset + pktSize) <= AesLoader_SIZEOF_COMMAND_BUFFER)
                        {
                            ackCode = CYRET_SUCCESS;

#if(CY_PSOC3)
                            (void) memcpy(&dataBuffer[dataOffset],
                                          &packetBuffer[AesLoader_DATA_ADDR],
                                          ( int16 )pktSize);
#else
                            (void) memcpy(&dataBuffer[dataOffset],
                                          &packetBuffer[AesLoader_DATA_ADDR],
                                          (uint32) pktSize);
#endif  /* (CY_PSOC3) */

                            dataOffset += pktSize;
                        }
                        else
                        {
                            ackCode = AesLoader_ERR_LENGTH;
                        }
                    }

                    break;

#endif  /* (0u != AesLoader_CMD_SEND_DATA_AVAIL) */


                /***************************************************************************
                *   Enter bootloader (0x38)
                ***************************************************************************/
                case AesLoader_COMMAND_ENTER:

#if (AesLoader_SECURITY_KEY_AVAIL == 1u)
                    if (pktSize == 6u) /*Security key*/
#else
                    if (pktSize == 0u) /*Security key is not demanded*/
#endif /*(AesLoader_SECURITY_KEY_AVAIL == 1u)*/
                    {
#if(CY_PSOC3)

                        AesLoader_ENTER CYDATA BtldrVersion =
                        {CYSWAP_ENDIAN32(CYDEV_CHIP_JTAG_ID), CYDEV_CHIP_REV_EXPECT, AesLoader_VERSION};

#else

                        AesLoader_ENTER CYDATA BtldrVersion =
                        {CYDEV_CHIP_JTAG_ID, CYDEV_CHIP_REV_EXPECT, AesLoader_VERSION};
#endif  /* (CY_PSOC3) */

#if (AesLoader_SECURITY_KEY_AVAIL == 1u)

                        /*Security key checking*/
                        uint8 idx = AesLoader_SECURITY_KEY_LENGTH;
                        ackCode = CYRET_SUCCESS;

                        do
                        {
                            idx--;
                            if (AesLoader_securityKey[idx] != packetBuffer[AesLoader_DATA_ADDR + idx])
                            {
                                ackCode = AesLoader_ERR_DATA;
                                break;
                            }

                        }
                        while (0u != idx);

                        if (AesLoader_ERR_DATA == ackCode)
                        {
                            break;
                        }
#endif /*(AesLoader_SECURITY_KEY_AVAIL == 1u)*/

#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
                        AesLoader_isBootloading = AesLoader_BOOTLOADING_IN_PROGRESS;
#endif /*(CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)*/

                        communicationState = AesLoader_COMMUNICATION_STATE_ACTIVE;

                        rspSize = sizeof(AesLoader_ENTER);
                        dataOffset = 0u;

#if(CY_PSOC3)
                        (void) memcpy(&packetBuffer[AesLoader_DATA_ADDR],
                                      &BtldrVersion,
                                      ( int16 )rspSize);
#else
                        (void) memcpy(&packetBuffer[AesLoader_DATA_ADDR],
                                      &BtldrVersion,
                                      (uint32) rspSize);
#endif  /* (CY_PSOC3) */

                        ackCode = CYRET_SUCCESS;
                    }
                    else /* Packet length does not match*/
                    {
                        ackCode = AesLoader_ERR_DATA;
                    }
                    break;


                    /***************************************************************************
                    *   Get row checksum (0x3A)
                    ***************************************************************************/
#if (0u != AesLoader_CMD_GET_ROW_CHKSUM_AVAIL)

                case AesLoader_COMMAND_GET_ROW_CHKSUM:

                    if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (pktSize == 3u))
                    {
                        /* Get flash/EEPROM row number */
                        uint16 CYDATA rowNum = ((uint16)((uint16)packetBuffer[AesLoader_DATA_ADDR + 2u] << 8u)) |
                                               packetBuffer[AesLoader_DATA_ADDR + 1u];

                        uint16 CYDATA tmpRowNum;
#if(!CY_PSOC4)

                        uint32 CYDATA rowAddr;
                        uint8 CYDATA checksum;

                        if ((btldrData >= AesLoader_FIRST_EE_ARRAYID) &&
                            (btldrData <= AesLoader_LAST_EE_ARRAYID))
                        {
                            /* EEPROM */
                            /* Both PSoC 3 and PSoC 5LP architecture have one EEPROM array. */
                            rowAddr = (uint32)rowNum * CYDEV_EEPROM_ROW_SIZE;

                            /* Checking if row number is within array address range */
                            ackCode = AesLoader_CHECK_ROW_NUMBER(rowNum,
                                                                 (AesLoader_NUMBER_OF_EEPROM_SECTORS * AesLoader_NUMBER_OF_ROWS_IN_EEPROM_SECTOR));

                            if (CYRET_SUCCESS != ackCode)
                            {
                                break;
                            }

                            checksum = AesLoader_Calc8BitSum(CY_EEPROM_BASE, rowAddr, CYDEV_EEPROM_ROW_SIZE);
                        }
                        else
                        {
                            /* flash */
                            rowAddr = ((uint32)btldrData * CYDEV_FLS_SECTOR_SIZE)
                                      + ((uint32)rowNum * CYDEV_FLS_ROW_SIZE);

                            /* Checking if row number is within array address range */
                            ackCode = AesLoader_CHECK_ROW_NUMBER(rowNum,
                                                                 AesLoader_NUMBER_OF_ROWS_IN_ARRAY);

                            if (CYRET_SUCCESS != ackCode)
                            {
                                break;
                            }

                            checksum = AesLoader_Calc8BitSum(CY_FLASH_BASE, rowAddr, CYDEV_FLS_ROW_SIZE);
                        }

#else

                        uint32 CYDATA rowAddr = ((uint32)btldrData * CYDEV_FLS_SECTOR_SIZE)
                                                + ((uint32)rowNum * CYDEV_FLS_ROW_SIZE);

                        uint8 CYDATA checksum;

                        /* Checking if row number is within array address range */
                        ackCode = AesLoader_CHECK_ROW_NUMBER(rowNum,
                                                             AesLoader_NUMBER_OF_ROWS_IN_ARRAY);

                        if (CYRET_SUCCESS != ackCode)
                        {
                            break;
                        }

                        checksum = AesLoader_Calc8BitSum(CY_FLASH_BASE, rowAddr, CYDEV_FLS_ROW_SIZE);

#endif  /* (!CY_PSOC4) */


                        /* Calculate checksum on data from ECC */
#if(!CY_PSOC4) && (CYDEV_ECC_ENABLE == 0u)

                        if (btldrData <= AesLoader_LAST_FLASH_ARRAYID)
                        {
                            uint16 CYDATA tmpIndex;

                            rowAddr = CYDEV_ECC_BASE + ((uint32)btldrData * (CYDEV_FLS_SECTOR_SIZE / 8u))
                                      + ((uint32)rowNum * CYDEV_ECC_ROW_SIZE);

                            for (tmpIndex = 0u; tmpIndex < CYDEV_ECC_ROW_SIZE; tmpIndex++)
                            {
                                checksum += CY_GET_XTND_REG8((uint8 CYFAR *)(rowAddr + tmpIndex));
                            }
                        }

#endif  /* (!CY_PSOC4) && (CYDEV_ECC_ENABLE == 0u) */


                        /*******************************************************************************
                        * App Verified & App Active are information updated in flash at the runtime.
                        * Remove these items from the checksum to allow the host to verify if everything is
                        * correct.
                         ******************************************************************************/
#if(!CY_PSOC4)
                        tmpRowNum = rowNum;
#else
                        tmpRowNum = rowNum + ((uint16)(AesLoader_NUMBER_OF_ROWS_IN_ARRAY * btldrData));
#endif

                        if ((AesLoader_MD_FLASH_ARRAY_NUM == btldrData) && (AesLoader_CONTAIN_METADATA(tmpRowNum)))
                        {

                            checksum -= (uint8)AesLoader_GetMetadata(AesLoader_GET_BTLDB_ACTIVE,
                                                                     AesLoader_GET_APP_ID(tmpRowNum));

                            checksum -= (uint8)AesLoader_GetMetadata(AesLoader_GET_BTLDB_STATUS,
                                                                     AesLoader_GET_APP_ID(tmpRowNum));
                        }

                        packetBuffer[AesLoader_DATA_ADDR] = (uint8)1u + (uint8)(~checksum);
                        ackCode = CYRET_SUCCESS;
                        rspSize = AesLoader_RSP_SIZE_VERIFY_ROW_CHKSM;
                    }
                    break;

#endif /* (0u != AesLoader_CMD_GET_ROW_CHKSUM_AVAIL) */


                /***************************************************************************
                *   Exit bootloader (0x3B)
                ***************************************************************************/
                case AesLoader_COMMAND_EXIT:

                    /*******************************************************************************
                    * Currently the copy flag is checked in the metadata for the second application.
                    * If it is set, then the copy operation is required and Launcher (BTLDR) should
                    * be scheduled.
                    *******************************************************************************/
#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
                    needToCopyFlag = AesLoader_GetMetadata(AesLoader_GET_BTLDB_COPY_FLAG,
                                                           AesLoader_MD_BTLDB_ACTIVE_1);

                    /* Checking "Need to copy" flag in metadata#1 */
                    if (0u != (needToCopyFlag & AesLoader_NEED_TO_COPY_MASK))
                    {
                        AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDR);
                    }
                    else
#endif /* (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER) */

                        if (CYRET_SUCCESS == AesLoader_ValidateBootloadable(AesLoader_activeApp))
                        {
                            AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDB);
                        }

#if (CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
                    AesLoader_isBootloading = AesLoader_BOOTLOADING_COMPLETED;
#endif /* CYDEV_PROJ_TYPE == CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER */

                    CyBtldrCommStop();

                    CySoftwareReset();

                    /* Will never get here */
                    break;


                /***************************************************************************
                *   Unsupported command
                ***************************************************************************/
                default:

#if (!CY_PSOC3)
                    if ((NULL != AesLoader_callback) && (AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState))
                    {
                        AesLoader_in_packet_type AesLoader_inPacket;
                        AesLoader_out_packet_type AesLoader_outPacket;

                        /* Initializing input packet */
                        AesLoader_inPacket.command = packetBuffer[AesLoader_CMD_ADDR];
                        AesLoader_inPacket.packetLength = pktSize;
                        AesLoader_inPacket.pInputBuffer = &packetBuffer[AesLoader_DATA_ADDR];

                        /* Clearing and setting output packet */
                        (void) memset(&AesLoader_outPacket, 0, sizeof(AesLoader_outPacket));
                        AesLoader_outPacket.pOutputBuffer = &packetBuffer[AesLoader_DATA_ADDR];

                        /* Callback */
                        (void) AesLoader_callback(&AesLoader_inPacket, &AesLoader_outPacket);

                        rspSize = AesLoader_outPacket.packetLength;
                        if (((rspSize > (AesLoader_SIZEOF_COMMAND_BUFFER - AesLoader_MIN_PKT_SIZE)) &&
                             (AesLoader_outPacket.pOutputBuffer == &packetBuffer[AesLoader_DATA_ADDR])) ||
                            ((NULL == AesLoader_outPacket.pOutputBuffer) && (0u != rspSize)))
                        {
                            /* One of returned parameters is wrong */
                            ackCode = AesLoader_ERR_CALLBACK;
                            rspSize = 0u;
                        }
                        else /* Everything is OK */
                        {
                            ackCode = (uint8)AesLoader_outPacket.status;
                        }
                    }
                    else /* No callback function defined, so return "Error Command" status */
#endif /* (!CY_PSOC3) */
                    {
                        ackCode = AesLoader_ERR_CMD;
                    }
                    break;
            }
        }

        /* Reply with acknowledge or not acknowledge packet */
        (void) AesLoader_WritePacket(ackCode, packetBuffer, rspSize);

    }
    while ((0u == timeOut) || (AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState));
}


/*******************************************************************************
* Function Name: AesLoader_WritePacket
****************************************************************************//**
* \internal
*
* \brief
*  Creates a bootloader response packet and transmits it back to the bootloader
*  host application over the already established communications protocol.
*
*  \param status
*      The status code to pass back as the second byte of the packet.
*  \param buffer
*      The buffer containing the data portion of the packet.
*  \param size
*      The number of bytes contained within the buffer to pass back.
*
* \return
*   CYRET_SUCCESS if successful. Any other non-zero value if a failure occurred.
*
* \endinternal
*******************************************************************************/
static cystatus AesLoader_WritePacket(uint8 status, uint8 buffer[], uint16 size) CYSMALL \

{
    uint16 CYDATA checksum;

    /* Start of packet. */
    buffer[AesLoader_SOP_ADDR]      = AesLoader_SOP;
    buffer[AesLoader_CMD_ADDR]      = status;
    buffer[AesLoader_SIZE_ADDR]     = LO8(size);
    buffer[AesLoader_SIZE_ADDR + 1u] = HI8(size);

    /* Compute checksum. */
    checksum = AesLoader_CalcPacketChecksum(buffer, size + AesLoader_DATA_ADDR);

    buffer[AesLoader_CHK_ADDR(size)]     = LO8(checksum);
    buffer[AesLoader_CHK_ADDR(1u + size)] = HI8(checksum);
    buffer[AesLoader_EOP_ADDR(size)]     = AesLoader_EOP;

    /* Start packet transmit. */
    return (CyBtldrCommWrite(buffer, size + AesLoader_MIN_PKT_SIZE, &size, 150u));
}
#endif /*(CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LAUNCHER)*/

/*******************************************************************************
* Function Name: AesLoader_SetFlashByte
****************************************************************************//**
*
* \brief
*  Writes a byte to the specified flash memory location.
*
* \param address
*      The address in flash memory where data will be written.
* \param runType:
*      The byte to be written.
*
*******************************************************************************/
void AesLoader_SetFlashByte(uint32 address, uint8 runType)
{
    uint32 flsAddr = address - CYDEV_FLASH_BASE;
    uint8  rowData[CYDEV_FLS_ROW_SIZE];

#if !(CY_PSOC4)
    uint8 arrayId = ( uint8 )(flsAddr / CYDEV_FLS_SECTOR_SIZE);
#endif  /* !(CY_PSOC4) */

#if (CY_PSOC4)
    uint16 rowNum = ( uint16 )(flsAddr / CYDEV_FLS_ROW_SIZE);
#else
    uint16 rowNum = ( uint16 )((flsAddr % CYDEV_FLS_SECTOR_SIZE) / CYDEV_FLS_ROW_SIZE);
#endif  /* (CY_PSOC4) */

    uint32 baseAddr = address - (address % CYDEV_FLS_ROW_SIZE);
    uint16 idx;

    for (idx = 0u; idx < CYDEV_FLS_ROW_SIZE; idx++)
    {
        rowData[idx] = AesLoader_GET_CODE_BYTE(baseAddr + idx);
    }

    rowData[address % CYDEV_FLS_ROW_SIZE] = runType;

#if(CY_PSOC4)
    (void) CySysFlashWriteRow((uint32) rowNum, rowData);
#else
    (void) CyWriteRowData(arrayId, rowNum, rowData);
#endif  /* (CY_PSOC4) */

#if(CY_PSOC5)
    /***************************************************************************
    * When writing to flash, data in the instruction cache can become stale.
    * Therefore, the cache data does not correlate to the data just written to
    * flash. A call to CyFlushCache() is required to invalidate the data in the
    * cache and force fresh information to be loaded from flash.
    ***************************************************************************/
    CyFlushCache();
#endif /* (CY_PSOC5) */
}


/*******************************************************************************
* Function Name: AesLoader_GetMetadata
****************************************************************************//**
*
* \brief
*    Returns the value of the specified field of the metadata section.
*
* \param field \ref group_metadataFields
*    Identifies the specific field of metadata.
*
* \param appId
*    The number of the Bootloadable/Combination application. Should be 0 for
*    the normal bootloader and 0 or 1 for the Dual-application bootloader.
*
* \return
*    The value of the specified field of the specified application.
*
*******************************************************************************/
uint32 AesLoader_GetMetadata(uint8 field, uint8 appId)
{
    uint32 fieldPtr;
    uint8  fieldSize = 2u;
    uint32 result = 0u;

    switch (field)
    {
        case AesLoader_GET_BTLDB_CHECKSUM:
            fieldPtr  = AesLoader_MD_BTLDB_CHECKSUM_OFFSET(appId);
            fieldSize = 1u;
            break;

        case AesLoader_GET_BTLDB_ADDR:
            fieldPtr  = AesLoader_MD_BTLDB_ADDR_OFFSET(appId);
#if(!CY_PSOC3)
            fieldSize = 4u;
#endif  /* (!CY_PSOC3) */
            break;

        case AesLoader_GET_BTLDR_LAST_ROW:
            fieldPtr  = AesLoader_MD_BTLDR_LAST_ROW_OFFSET(appId);
            break;

        case AesLoader_GET_BTLDB_LENGTH:
            fieldPtr  = AesLoader_MD_BTLDB_LENGTH_OFFSET(appId);
#if(!CY_PSOC3)
            fieldSize = 4u;
#endif  /* (!CY_PSOC3) */
            break;

        case AesLoader_GET_BTLDB_ACTIVE:
            fieldPtr  = AesLoader_MD_BTLDB_ACTIVE_OFFSET(appId);
            fieldSize = 1u;
            break;

        case AesLoader_GET_BTLDB_STATUS:
            fieldPtr  = AesLoader_MD_BTLDB_VERIFIED_OFFSET(appId);
            fieldSize = 1u;
            break;

        case AesLoader_GET_BTLDB_APP_VERSION:
            fieldPtr  = AesLoader_MD_BTLDB_APP_VERSION_OFFSET(appId);
            break;

        case AesLoader_GET_BTLDR_APP_VERSION:
            fieldPtr  = AesLoader_MD_BTLDR_APP_VERSION_OFFSET(appId);
            break;

        case AesLoader_GET_BTLDB_APP_ID:
            fieldPtr  = AesLoader_MD_BTLDB_APP_ID_OFFSET(appId);
            break;

        case AesLoader_GET_BTLDB_APP_CUST_ID:
            fieldPtr  = AesLoader_MD_BTLDB_APP_CUST_ID_OFFSET(appId);
            fieldSize = 4u;
            break;

        case AesLoader_GET_BTLDB_COPY_FLAG:
            fieldPtr = AesLoader_MD_BTLDB_COPY_FLAG_OFFSET(appId);
            fieldSize = 1u;
            break;

        case AesLoader_GET_BTLDB_USER_DATA:
            fieldPtr = AesLoader_MD_BTLDB_USER_DATA_OFFSET(appId);
            fieldSize = 4u;
            break;

        default:
            /* Should never be here */
            CYASSERT(0u != 0u);
            fieldPtr  = 0u;
            break;
    }


    if (1u == fieldSize)
    {
        result =  (uint32) CY_GET_XTND_REG8((volatile uint8 *)fieldPtr);
    }

#if(CY_PSOC3)   /* Big-endian */

    if (2u == fieldSize)
    {
        result =  (uint32) CY_GET_XTND_REG8((volatile uint8 *)(fieldPtr + 1u));
        result |= (uint32) CY_GET_XTND_REG8((volatile uint8 *)(fieldPtr     )) <<  8u;
    }

    if (4u == fieldSize)
    {
        result =  (uint32) CY_GET_XTND_REG8((volatile uint8 *)(fieldPtr + 3u));
        result |= (uint32) CY_GET_XTND_REG8((volatile uint8 *)(fieldPtr + 2u)) <<  8u;
        result |= (uint32) CY_GET_XTND_REG8((volatile uint8 *)(fieldPtr + 1u)) << 16u;
        result |= (uint32) CY_GET_XTND_REG8((volatile uint8 *)(fieldPtr     )) << 24u;
    }

#else   /* PSoC 4 and PSoC 5: Little-endian */

    if (2u == fieldSize)
    {
        result =  (uint32) CY_GET_XTND_REG8((volatile uint8 *) (fieldPtr     ));
        result |= (uint32) CY_GET_XTND_REG8((volatile uint8 *) (fieldPtr + 1u)) <<  8u;
    }

    if (4u == fieldSize)
    {
        result =  (uint32) CY_GET_XTND_REG8((volatile uint8 *)(fieldPtr     ));
        result |= (uint32) CY_GET_XTND_REG8((volatile uint8 *)(fieldPtr + 1u)) <<  8u;
        result |= (uint32) CY_GET_XTND_REG8((volatile uint8 *)(fieldPtr + 2u)) << 16u;
        result |= (uint32) CY_GET_XTND_REG8((volatile uint8 *)(fieldPtr + 3u)) << 24u;
    }

#endif /* (CY_PSOC3) */

    return (result);
}


/* @} [] END OF FILE */
