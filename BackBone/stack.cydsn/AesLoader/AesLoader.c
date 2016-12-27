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
#include "watchdog.h"

/**
 \defgroup variables_group Variables
 @{
*/

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

/**
*  This variable is intended to keep the active application number. It applies
*  to Classic Dual-app Bootloader and in-application bootloading.
*/
volatile uint8 AesLoader_activeApp = AesLoader_MD_BTLDB_ACTIVE_NONE;

/**
 *  This variable holds the pointer on the user's callback-function that implements the custom
 *  bootloader command processing.
 */
static AesLoader_callback_type AesLoader_callback = NULL;

/** @}*/

/***************************************
*     Function Prototypes
***************************************/
static cystatus AesLoader_WritePacket(uint8 status, uint8 buffer[], uint16 size) CYSMALL;
static uint16   AesLoader_CalcPacketChecksum(const uint8 buffer[], uint16 size) CYSMALL;
static uint8 AesLoader_VerifyRow(uint32 flashStart, const uint8* ramStart, uint16 size) CYSMALL;
static uint8 AesLoader_CheckImage(uint8 appNumber, uint8 arrayNumber, uint16 row, uint16 rowNumInArray) CYSMALL;
static void AesLoader_SetActiveAppInMetadata(uint8 activeApp) CYSMALL;
static uint8 AesLoader_GetActiveAppFromMetadata(void) CYSMALL;

/**
 \defgroup functions_group Functions
 @{
*/
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
    if (AesLoader_BOOTLOADING_NOT_INITIALIZED == AesLoader_initVar)
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
    
#if (CY_BOOT_VERSION < CY_BOOT_5_0)
    #error Component Bootloader_v1_50 requires cy_boot v5.00 or later
#endif

#if (CYDEV_PROJ_TYPE != CYDEV_PROJ_TYPE_LOADABLEANDBOOTLOADER)
    #error This version of the bootloader only designed to work with hybrid bootloader
#endif

#if ((CY_PSOC3) || (!CY_PSOC4) || (CY_PSOC5))
    #error This version of the bootloader only designed to work with PSoC4
#endif
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
    uint16 CYDATA sum = 0u;

    while (size > 0u)
    {
        sum += buffer[size - 1u];
        size--;
    }

    return (( uint16 )1u + ( uint16 )(~sum));
}

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
    CYASSERT(baseAddr == CY_FLASH_BASE);

    while (size > 0u)
    {
        size--;
        sum += (*((uint8  *)(baseAddr + start + size)));
    }

    return (sum);
}


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
    /***********************************************************************
    * If the active Bootloadable application is invalid or scheduled - do the following:
    *  - schedule the Bootloader application to be run after a software reset;
    *  - go to the communication subroutine. The HostLink() will wait for
    *    commands forever.
    ***********************************************************************/
    if (AesLoader_GET_RUN_TYPE == AesLoader_SCHEDULE_BTLDR)
    {
        AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDR_INIT_STATE);
        AesLoader_HostLink(AesLoader_WAIT_FOR_COMMAND_FOREVER);
    }

    /* Go to communication subroutine. Will wait for commands for specified time */
    /* Timeout is in 100s of milliseconds */
    AesLoader_HostLink(AesLoader_WAIT_FOR_COMMAND_TIME);
}

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

            AesLoader_activeApp = appId;
            AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDB);
        }
        else
        {
            AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDR_INIT_STATE);
        }
    }

    CySoftwareReset();
}

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

    calcedChecksum = ( uint8 )1u + ( uint8 )(~calcedChecksum);

    if ((calcedChecksum != AesLoader_GetMetadata(AesLoader_GET_BTLDB_CHECKSUM, appId)) ||
        (0u == valid))
    {
        return (CYRET_BAD_DATA);
    }

    return (CYRET_SUCCESS);
}

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

void AesLoader_HostLink(uint8 timeOut) CYLARGE
{
    uint16    CYDATA numberRead;
    uint16    CYDATA rspSize;
    uint8     CYDATA ackCode;
    uint16    CYDATA pktChecksum;
    cystatus  CYDATA readStat;
    uint16    CYDATA pktSize    = 0u;
    uint8     CYDATA timeOutCnt = 10u;

    static  CYBIT communicationState = AesLoader_COMMUNICATION_STATE_IDLE;
    static uint16 CYDATA dataOffset = 0u;
    uint16  CYDATA btldrLastRow = 0xFFFFu;
    uint8 needToCopyFlag = 0u;
    uint32 app2StartAddress = AesLoader_GetMetadata(AesLoader_GET_BTLDB_ADDR,
    AesLoader_MD_BTLDB_ACTIVE_1);

    uint8     packetBuffer[AesLoader_SIZEOF_COMMAND_BUFFER];
    uint8     dataBuffer  [AesLoader_SIZEOF_COMMAND_BUFFER];

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

            if (watchdog_is_clear_requested())
            {
                watchdog_clear();
            }

        }
        while ( (0u != timeOutCnt) && (readStat != CYRET_SUCCESS) );

        if ( readStat != CYRET_SUCCESS )
        {
            continue;
        }

        if ((numberRead < AesLoader_MIN_PKT_SIZE) || (packetBuffer[AesLoader_SOP_ADDR] != AesLoader_SOP))
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
                case AesLoader_COMMAND_GET_METADATA:
                    if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (pktSize == 1u))
                    {
                        if (btldrData >= AesLoader_MAX_NUM_OF_BTLDB)
                        {
                            ackCode = AesLoader_ERR_APP;
                        }
                        else if (CYRET_SUCCESS == AesLoader_ValidateBootloadable(btldrData))
                        {
                            (void) memcpy(&packetBuffer[AesLoader_DATA_ADDR],
                                          (uint8 *) AesLoader_MD_BASE_ADDR(btldrData),
                                          AesLoader_GET_METADATA_RESPONSE_SIZE);

                            rspSize = AesLoader_RSP_SIZE_GET_METADATA;
                            ackCode = CYRET_SUCCESS;
                        }
                        else
                        {
                            ackCode = AesLoader_ERR_APP;
                        }
                    }
                    break;

                /***************************************************************************
                *   Verify application checksum (0x31)
                ***************************************************************************/
                case AesLoader_COMMAND_CHECKSUM:
                    if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (pktSize == 0u))
                    {
                        packetBuffer[AesLoader_DATA_ADDR] =
                            (uint8)(AesLoader_ValidateBootloadable(AesLoader_activeApp) == CYRET_SUCCESS);

                        rspSize = AesLoader_RSP_SIZE_VERIFY_CHKSM;
                        ackCode = CYRET_SUCCESS;
                    }
                    break;

                /*****************************************************************************
                *   Verify row (0x45u)
                ***************************************************************************/
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

                        ackCode = CYRET_SUCCESS;
                        rspSize = AesLoader_RSP_SIZE_0;

                        if (3u == pktSize)
                        {
                            /* Do nothing, data is already in dataBuffer[] */
                        }
                        else
                        {

                            /*Data is in packetBuffer[], needs to be copied to dataBuffer[]*/
                            (void) memcpy(dataBuffer, &packetBuffer[AesLoader_DATA_ADDR + 3u],
                                          (uint32)pktSize - 3u);
                        }

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
                    }
                    break;

                /***************************************************************************
                *   Get flash size (0x32)
                ***************************************************************************/
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

                            btldrLastRow = (uint16)(AesLoader_GetMetadata(AesLoader_GET_BTLDR_LAST_ROW, \
                                                                          AesLoader_MD_BTLDB_ACTIVE_0));
                            arrayIdBtlderEnds = (uint8) (((uint32)btldrLastRow * CY_FLASH_SIZEOF_ROW) / (uint32)CY_FLASH_SIZEOF_ARRAY);
                            if (btldrData == arrayIdBtlderEnds)
                            {
                                startRow = (uint16)((btldrLastRow) % AesLoader_NUMBER_OF_ROWS_IN_ARRAY);
                            }
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

                /***************************************************************************
                *   Get application status (0x33)
                ***************************************************************************/
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

                /***************************************************************************
                *   Program / Erase row (0x39 / 0x34)
                ***************************************************************************/
                case AesLoader_COMMAND_PROGRAM: // Fall through
                case AesLoader_COMMAND_ERASE:
                    if (AesLoader_COMMAND_ERASE == packetBuffer[AesLoader_CMD_ADDR])
                    {
                        if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (pktSize == 3u))
                        {
                            /* Size of flash row (no ECC available) */
                            dataOffset = AesLoader_FROW_SIZE;

                            (void) memset(dataBuffer, 0, (uint32) dataOffset);
                        }
                        else
                        {
                            break;
                        }
                    }

                    if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (pktSize >= 3u))
                    {
                        uint16 upperRange;

                        /* Command may be sent along with last block of data, to program row. */
                        (void) memcpy(&dataBuffer[dataOffset],
                                      &packetBuffer[AesLoader_DATA_ADDR + 3u],
                                      (uint32) pktSize - 3u);

                        dataOffset += (pktSize - 3u);

                        /* Size of flash row (no ECC available) */
                        pktSize = AesLoader_FROW_SIZE;
                        upperRange = AesLoader_NUMBER_OF_ROWS_IN_ARRAY;

                        /* Check if we have all data to program */
                        if (dataOffset == pktSize)
                        {
                            uint16 row;
                            uint16 firstRow;

                            /* Get flash/EEPROM row number inside array */
                            dataOffset = ((uint16)((uint16)packetBuffer[AesLoader_DATA_ADDR + 2u] << 8u)) |
                                         packetBuffer[AesLoader_DATA_ADDR + 1u];

                            /*Checking if row number is within array address range*/
                            ackCode = AesLoader_CHECK_ROW_NUMBER(dataOffset, upperRange);

                            if (CYRET_SUCCESS != ackCode)
                            {
                                break;
                            }

                            /* btldrData  - holds flash array Id sent by host */
                            /* dataOffset - holds flash row Id sent by host   */
                            row = (uint16)(btldrData * AesLoader_NUMBER_OF_ROWS_IN_ARRAY) + dataOffset;

                            /*******************************************************************************
                            * Refuse to write to the row within range of the bootloader application
                            *******************************************************************************/

                            /* First empty flash row after Bootloader application */
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

                            /* Check to see if a row to be programmed will not corrupt the Bootloader application */
                            if (row < firstRow)
                            {
                                ackCode = AesLoader_ERR_ROW;
                                dataOffset = 0u;
                                break;
                            }

                            /************************************************************************************
                            * No check is required in case when there is no app#1 present in Upgradable Stack
                            * use case (no user application downloaded yet).
                            ************************************************************************************/
                            if (0u != app2StartAddress)
                            {
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
                            }

                            ackCode = (CYRET_SUCCESS != CySysFlashWriteRow((uint32) row, dataBuffer)) \
                                      ? AesLoader_ERR_ROW \
                                      : CYRET_SUCCESS;

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
                case AesLoader_COMMAND_SYNC:
                    if (AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState)
                    {
                        /* If something failed, Host would send this command to reset Bootloader. */
                        dataOffset = 0u;

                        /* Don't acknowledge the packet, just get ready to accept the next one */
                        continue;
                    }
                    break;

                /***************************************************************************
                *   Set an active application (0x36)
                ***************************************************************************/
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

                /***************************************************************************
                *   Send data (0x37)
                ***************************************************************************/
                case AesLoader_COMMAND_DATA:
                    if (AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState)
                    {
                        /*  Make sure that dataOffset is valid before copying data */
                        if ((dataOffset + pktSize) <= AesLoader_SIZEOF_COMMAND_BUFFER)
                        {
                            ackCode = CYRET_SUCCESS;

                            (void) memcpy(&dataBuffer[dataOffset],
                                          &packetBuffer[AesLoader_DATA_ADDR],
                                          (uint32) pktSize);

                            dataOffset += pktSize;
                        }
                        else
                        {
                            ackCode = AesLoader_ERR_LENGTH;
                        }
                    }

                    break;

                /***************************************************************************
                *   Enter bootloader (0x38)
                ***************************************************************************/
                case AesLoader_COMMAND_ENTER:
                    if (pktSize == 0u) /*Security key is not demanded*/
                    {
                        AesLoader_ENTER CYDATA BtldrVersion =
                        {CYDEV_CHIP_JTAG_ID, CYDEV_CHIP_REV_EXPECT, AesLoader_VERSION};

                        AesLoader_isBootloading = AesLoader_BOOTLOADING_IN_PROGRESS;


                        communicationState = AesLoader_COMMUNICATION_STATE_ACTIVE;

                        rspSize = sizeof(AesLoader_ENTER);
                        dataOffset = 0u;

                        (void) memcpy(&packetBuffer[AesLoader_DATA_ADDR],
                                      &BtldrVersion,
                                      (uint32) rspSize);

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
                case AesLoader_COMMAND_GET_ROW_CHKSUM:
                    if ((AesLoader_COMMUNICATION_STATE_ACTIVE == communicationState) && (pktSize == 3u))
                    {
                        /* Get flash/EEPROM row number */
                        uint16 CYDATA rowNum = ((uint16)((uint16)packetBuffer[AesLoader_DATA_ADDR + 2u] << 8u)) |
                                               packetBuffer[AesLoader_DATA_ADDR + 1u];

                        uint16 CYDATA tmpRowNum;

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

                        /*******************************************************************************
                        * App Verified & App Active are information updated in flash at the runtime.
                        * Remove these items from the checksum to allow the host to verify if everything is
                        * correct.
                         ******************************************************************************/

                        tmpRowNum = rowNum + ((uint16)(AesLoader_NUMBER_OF_ROWS_IN_ARRAY * btldrData));

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

                /***************************************************************************
                *   Exit bootloader (0x3B)
                ***************************************************************************/
                case AesLoader_COMMAND_EXIT:
                    /*******************************************************************************
                    * Currently the copy flag is checked in the metadata for the second application.
                    * If it is set, then the copy operation is required and Launcher (BTLDR) should
                    * be scheduled.
                    *******************************************************************************/

                    needToCopyFlag = AesLoader_GetMetadata(AesLoader_GET_BTLDB_COPY_FLAG,
                                                           AesLoader_MD_BTLDB_ACTIVE_1);

                    /* Checking "Need to copy" flag in metadata#1 */
                    if (0u != (needToCopyFlag & AesLoader_NEED_TO_COPY_MASK))
                    {
                        AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDR);
                    }
                    else
                    {
                        if (CYRET_SUCCESS == AesLoader_ValidateBootloadable(AesLoader_activeApp))
                        {
                            AesLoader_SET_RUN_TYPE(AesLoader_SCHEDULE_BTLDB);
                        }
                    }

                    AesLoader_isBootloading = AesLoader_BOOTLOADING_COMPLETED;
                    CyBtldrCommStop();
                    CySoftwareReset();

                    /* Will never get here */
                    break;

                /***************************************************************************
                *   Unsupported command
                ***************************************************************************/
                default:
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
    uint16 rowNum = ( uint16 )(flsAddr / CYDEV_FLS_ROW_SIZE);
    uint32 baseAddr = address - (address % CYDEV_FLS_ROW_SIZE);
    uint16 idx;

    for (idx = 0u; idx < CYDEV_FLS_ROW_SIZE; idx++)
    {
        rowData[idx] = AesLoader_GET_CODE_BYTE(baseAddr + idx);
    }

    rowData[address % CYDEV_FLS_ROW_SIZE] = runType;
    (void) CySysFlashWriteRow((uint32) rowNum, rowData);
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
            fieldSize = 4u;
            break;

        case AesLoader_GET_BTLDR_LAST_ROW:
            fieldPtr  = AesLoader_MD_BTLDR_LAST_ROW_OFFSET(appId);
            break;

        case AesLoader_GET_BTLDB_LENGTH:
            fieldPtr  = AesLoader_MD_BTLDB_LENGTH_OFFSET(appId);
            fieldSize = 4u;
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

    return (result);
}

/* @} [] END OF FILE */
