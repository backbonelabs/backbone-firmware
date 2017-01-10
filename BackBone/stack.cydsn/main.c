/*******************************************************************************
* File Name: main.c
*
* Version: 1.10
*
* Description:
*  This example shows how to build a Stack only project for a BLE design.
*  This project is required for designs that need Over the Air upgrade
*  capability for both Application and the BLE stack. The Design has a BLE
*  component configured as a "Stack Only". The BLE component initialized with
*  a Bootloader service is used as the transport for the bootloader packets
*  from the host. The project also has a bootloader component that uses the
*  packets received from the host to conduct the bootload process.
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

#define CY_BOOTLOADER_Loader_H
#include "project.h"
#include "main.h"
#include "AesLoader\AesLoader.h"
#include "debug.h"
#include "watchdog.h"

CYBLE_CONN_HANDLE_T connHandle;

#if defined(__ARMCC_VERSION)
    static unsigned long keepMe __attribute__((used));
    __attribute__ ((section(".bootloaderruntype"), zero_init))
    extern volatile uint32 CyReturnToBootloaddableAddress;
#endif /*__ARMCC_VERSION*/

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  Starts BLE component and performs all configuration changes required for
*  BLE and Bootloader components operation.
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
    CYBLE_API_RESULT_T apiResult;
    CYBLE_STACK_LIB_VERSION_T stackVersion;

#if defined(__ARMCC_VERSION)
    keepMe = Image$$DATA$$ZI$$Limit;
    CyReturnToBootloaddableAddress = 0u;
#endif /*__ARMCC_VERSION*/

    watchdog_init();
    CyGlobalIntEnable;

    DBG_PRINT_TEXT("> Backbone Bootloader\r\n");
    DBG_PRINT_TEXT("> Compile Date and Time: " __DATE__ " " __TIME__ "\r\n\r\n");

    /* Checks if Self Project Image is updated and Runs for the First time */
    AfterImageUpdate();

    /* If application image is present and valid but no metadata flag is set.
     * Assuming it is first launch.
     */
    if ((CYRET_SUCCESS == AesLoader_ValidateApp(1u)) && \
        (0u == (uint32) CY_GET_XTND_REG8((volatile uint8 *)APP_UPDATE_FLAG_OFFSET)))
    {
        Bootloadable_SetActiveApplication(1u);
        CySoftwareReset();
    }

    apiResult = CyBle_Start(AppCallBack);

    if (apiResult != CYBLE_ERROR_OK)
    {
        if (apiResult == CYBLE_ERROR_MEMORY_ALLOCATION_FAILED)
        {
            /* Receiving this error is expected if system heap size is too small for BLE component to start.
             * Please read BLE component datasheet for more details on BLE component stack export mode
             */
            while (1u == 1u)
            {
                /* Will stay here */
            }
        }
    }
    else
    {
        CyBle_GetStackLibraryVersion(&stackVersion);
    }

    /* Start Bootloader component in non-blocking mode. */
    AesLoader_Initialize();

    for (;;)
    {
        CyBle_ProcessEvents();
        AesLoader_HostLink(5u);

        /* To have predictable timeout (~40 seconds). */
        CyDelay(5u);

        TimeoutImplementation();

        if (watchdog_is_clear_requested())
        {
            watchdog_clear();
        }
    }
}

static void indicate_services_changed()
{
    CYBLE_GATTS_HANDLE_VALUE_IND_T indication;

    indication.attrHandle = CYBLE_UUID_CHAR_SERVICE_CHANGED;
    indication.value.val = 0;
    indication.value.len = 0;
    CyBle_GattsIndication(connHandle, &indication);
}

/*******************************************************************************
* Function Name: AppCallBack()
********************************************************************************
*
* Summary:
*   This function handles events that are generated by BLE stack.
*
* Parameters:
*   None
*
* Return:
*   None
*
*******************************************************************************/
void AppCallBack(uint32 event, void* eventParam)
{
    CYBLE_GAP_BD_ADDR_T localAddr;
    CYBLE_GAP_CONN_UPDATE_PARAM_T connUpdateParam;

    switch (event)
    {
        /**********************************************************
        *                       General Events
        ***********************************************************/
        case CYBLE_EVT_STACK_ON: /* This event received when component is Started */
            /* Enter into discoverable mode so that remote can search it. */
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            localAddr.type = 0u;
            CyBle_GetDeviceAddress(&localAddr);
            break;
        case CYBLE_EVT_HARDWARE_ERROR:    /* This event indicates that some internal HW error has occurred. */
            break;


        /**********************************************************
        *                       GAP Events
        ***********************************************************/
        case CYBLE_EVT_GAP_AUTH_REQ:
            break;
        case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
            break;
        case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
            break;
        case CYBLE_EVT_GAP_NUMERIC_COMPARISON_REQUEST:
            CyBle_GapAuthPassKeyReply(cyBle_connHandle.bdHandle, 0u, CYBLE_GAP_ACCEPT_PASSKEY_REQ);
            break;
        case CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT:
            break;
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            break;
        case CYBLE_EVT_GAP_AUTH_FAILED:
            break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            if ((*(CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T *)eventParam).connIntv > CYBLE_GAPP_CONNECTION_INTERVAL_MAX)
            {
                /* If connection settings do not match expected ones - request parameter update */
                connUpdateParam.connIntvMin = CYBLE_GAPP_CONNECTION_INTERVAL_MIN;
                connUpdateParam.connIntvMax = CYBLE_GAPP_CONNECTION_INTERVAL_MAX;
                connUpdateParam.connLatency = CYBLE_GAPP_CONNECTION_SLAVE_LATENCY;
                connUpdateParam.supervisionTO = CYBLE_GAPP_CONNECTION_TIME_OUT;
                CyBle_L2capLeConnectionParamUpdateRequest(cyBle_connHandle.bdHandle, &connUpdateParam);
            }
            break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;
        case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
            break;
        case CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE:
            break;
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            if (CYBLE_STATE_DISCONNECTED == CyBle_GetState())
            {
                /* Fast and slow advertising period complete, go to low power
                 * mode (Hibernate mode) and wait for an external
                 * user event to wake up the device again */

                /* NOTE: Backbone does not have an external user event.
                 * It would be possible to use the accelerometer to wakeup the
                 * processor but that requires pulling in the I2C component and
                 * accelerometer driver.  So for now just go back to fast
                 * advertising. */
                CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            }
            break;
        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CYBLE_EVT_GATT_CONNECT_IND:
            connHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;
            indicate_services_changed();
            break;
        case CYBLE_EVT_GATT_DISCONNECT_IND:
            connHandle.bdHandle = 0;
            break;
        case CYBLE_EVT_GATTS_PREP_WRITE_REQ:
            (void)CyBle_GattsPrepWriteReqSupport(CYBLE_GATTS_PREP_WRITE_NOT_SUPPORT);
            break;
        case CYBLE_EVT_HCI_STATUS:
            break;
        default:
            break;
    }
}


/* [] END OF FILE */
