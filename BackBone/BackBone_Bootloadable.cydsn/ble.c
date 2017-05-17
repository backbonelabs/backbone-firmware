/* ===========================================
 *
 * Copyright BACKBONE LABS INC, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION,
 * WHICH IS THE PROPERTY OF BACKBONE LABS INC.
 *
 * ===========================================
 */

#include "ble.h"
#include "version.h"
#include "OTAMandatory.h"
#include "backbone.h"
#include "debug.h"

/* Value is from From the CYBLE-222005-00 Data Sheet.  Table 48 (Page 24).
 * http://www.cypress.com/documentation/datasheets/cyble-222005-00-ez-bletm-proctm-module*/
#define CAPACITOR_TRIM_VALUE       0x0000A0A0

/* Minimum connection interval = CONN_PARAM_UPDATE_MIN_CONN_INTERVAL * 1.25 ms*/
#define CONN_PARAM_UPDATE_MIN_CONN_INTERVAL 92

/* Maximum connection interval = CONN_PARAM_UPDATE_MAX_CONN_INTERVAL * 1.25 ms */
#define CONN_PARAM_UPDATE_MAX_CONN_INTERVAL 108

/* Slave latency = Number of connection events */
#define CONN_PARAM_UPDATE_SLAVE_LATENCY     0

/* Supervision timeout = CONN_PARAM_UPDATE_SUPRV_TIMEOUT * 10*/
#define CONN_PARAM_UPDATE_SUPRV_TIMEOUT     2000

static CYBLE_GAP_CONN_UPDATE_PARAM_T ConnectionParam =
{
    CONN_PARAM_UPDATE_MIN_CONN_INTERVAL,
    CONN_PARAM_UPDATE_MAX_CONN_INTERVAL,
    CONN_PARAM_UPDATE_SLAVE_LATENCY,
    CONN_PARAM_UPDATE_SUPRV_TIMEOUT
};

static CYBLE_CONN_HANDLE_T m_connection;
static uint8 m_is_connected = BLE_FALSE;
static uint8 m_is_connection_update = BLE_TRUE;
static uint8 m_is_stack_busy = CYBLE_STACK_STATE_FREE;

void ble_init()
{
}

bool ble_is_connected()
{
    return m_is_connected == BLE_TRUE;
}

CYBLE_CONN_HANDLE_T* ble_get_connection()
{
    return &m_connection;
}

static void indicate_services_changed()
{
    CYBLE_GATTS_HANDLE_VALUE_IND_T indication;

    indication.attrHandle = CYBLE_UUID_CHAR_SERVICE_CHANGED;
    indication.value.val = 0;
    indication.value.len = 0;
    CyBle_GattsIndication(m_connection, &indication);
}

void ble_app_event_handler(uint32 event, void* param)
{
    switch (event)
    {
        case CYBLE_EVT_STACK_ON:
        {
            CYBLE_BLESS_CLK_CFG_PARAMS_T clockConfig;

            /* load capacitors on the ECO should be tuned and the tuned value
             * must be set in the CY_SYS_XTAL_BLERD_BB_XO_CAPTRIM_REG  */
            CY_SYS_XTAL_BLERD_BB_XO_CAPTRIM_REG = CAPACITOR_TRIM_VALUE;

            /* Get the configured clock parameters for BLE sub-system */
            CyBle_GetBleClockCfgParam(&clockConfig);

            /* Update the sleep clock inaccuracy PPM based on WCO crystal used */
            /* If you see frequent link disconnection, tune your WCO or update
             * the sleep clock accuracy here */
            clockConfig.bleLlSca = CYBLE_LL_SCA_031_TO_050_PPM;
            //clockConfig.bleLlSca = CYBLE_LL_SCA_021_TO_030_PPM;
            //clockConfig.bleLlSca = CYBLE_LL_SCA_000_TO_020_PPM;

            /* set the clock configuration parameter of BLE sub-system with updated values*/
            CyBle_SetBleClockCfgParam(&clockConfig);

            /* Put the device into discoverable mode so that a central device can connect to it */
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;
        }

        case CYBLE_EVT_TIMEOUT:
            DBG_PRINT_TEXT("CYBLE_EVT_TIMEOUT\r\n");
            /* Event Handling for Timeout  */
            break;

        /**********************************************************
        *                       GAP Events
        ***********************************************************/
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            /* If the current BLE state is Disconnected, then the Advertisement
             * Start Stop event implies that advertisement has stopped */
            if (CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
            {
                CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            }
            break;

        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            DBG_PRINT_TEXT("CYBLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            if (!backbone_is_reset_pending())
            {
                CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            }
            break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            break;

        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            DBG_PRINT_TEXT("\r\n");
            DBG_PRINT_TEXT("Authentication Complete\r\n");
            break;

        /**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CYBLE_EVT_GATT_CONNECT_IND:
            /* This event is received when device is connected over GATT level */
            m_connection = *(CYBLE_CONN_HANDLE_T*)param;
            m_is_connected = BLE_TRUE;
            backbone_connected(&m_connection);
            backbone_set_accelerometer_notification(&m_connection, false);
            backbone_set_distance_notification(&m_connection, false);
            backbone_set_session_statistics_notification(&m_connection, false);
            backbone_set_slouch_notification(&m_connection, false);
            backbone_set_status_notification(&m_connection, false);
            
            indicate_services_changed();
            break;

        case CYBLE_EVT_GATT_DISCONNECT_IND:
            DBG_PRINT_TEXT("CYBLE_EVT_GATT_DISCONNECT_IND\r\n");
            /* This event is received when device is disconnected */
            m_is_connected = BLE_FALSE;
            m_is_connection_update = BLE_TRUE;
            break;

        case CYBLE_EVT_GATTS_WRITE_REQ:
        {
            CYBLE_GATTS_WRITE_REQ_PARAM_T *request;

            request = (CYBLE_GATTS_WRITE_REQ_PARAM_T*)param;
            if (CYBLE_BACKBONE_ACCELEROMETER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == request->handleValPair.attrHandle)
            {
                backbone_set_accelerometer_notification(&m_connection,
                                                        request->handleValPair.value.val[CYBLE_BACKBONE_ACCELEROMETER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == BLE_TRUE);
            }
            else if (CYBLE_BACKBONE_DISTANCE_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == request->handleValPair.attrHandle)
            {
                backbone_set_distance_notification(&m_connection,
                                                   request->handleValPair.value.val[CYBLE_BACKBONE_DISTANCE_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == BLE_TRUE);
            }
            else if (CYBLE_BACKBONE_SESSION_STATISTICS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == request->handleValPair.attrHandle)
            {
                backbone_set_session_statistics_notification(&m_connection,
                                                             request->handleValPair.value.val[CYBLE_BACKBONE_SESSION_STATISTICS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == BLE_TRUE);
            }
            else if (CYBLE_BACKBONE_SLOUCH_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == request->handleValPair.attrHandle)
            {
                backbone_set_slouch_notification(&m_connection,
                                                 request->handleValPair.value.val[CYBLE_BACKBONE_SLOUCH_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == BLE_TRUE);
            }
            else if (CYBLE_BACKBONE_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == request->handleValPair.attrHandle)
            {
                backbone_set_status_notification(&m_connection,
                                                 request->handleValPair.value.val[CYBLE_BACKBONE_ACCELEROMETER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == BLE_TRUE);
            }
            else if (CYBLE_BACKBONE_ENTER_BOOTLOADER_CHAR_HANDLE == request->handleValPair.attrHandle)
            {
                backbone_enterbootloader(request->handleValPair.value.val,
                                         request->handleValPair.value.len);
            }
            else if (CYBLE_BACKBONE_CONTROL_SESSION_CHAR_HANDLE == request->handleValPair.attrHandle)
            {
                backbone_controlsession(request->handleValPair.value.val,
                                        request->handleValPair.value.len);
            }
            else if (CYBLE_BACKBONE_CONTROL_MOTOR_CHAR_HANDLE == request->handleValPair.attrHandle)
            {
                backbone_control_motor(request->handleValPair.value.val,
                                       request->handleValPair.value.len);
            }

            /* Send the response to the write request received. */
            CyBle_GattsWriteRsp(m_connection);
            break;
        }

        case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_RSP:
            m_is_connection_update = BLE_FALSE;
            break;

        case CYBLE_EVT_STACK_BUSY_STATUS:
            /* This event is generated when the internal stack buffer is full
             * and no more data can be accepted or the stack has buffer
             * available and can accept data.  This event is used by application
             * to prevent pushing lot of data to stack. */
            m_is_stack_busy = *((uint8*)param);
            break;

        default:
            break;
    }
}

/*******************************************************************************
* Function Name: UpdateConnectionParam
********************************************************************************
* Summary:
*        Send the Connection Update Request to Client device after connection
* and modify theconnection interval for low power operation.
*
* Parameters:
*   void
*
* Return:
*  void
*
*******************************************************************************/
void ble_update_connection_parameters(void)
{
    /* If device is connected and Update connection parameter not updated yet,
    * then send the Connection Parameter Update request to Client. */
    if (m_is_connected && m_is_connection_update)
    {
        /* Reset the flag to indicate that connection Update request has been sent */
        m_is_connection_update = BLE_FALSE;

        /* Send Connection Update request with set Parameter */
        CyBle_L2capLeConnectionParamUpdateRequest(m_connection.bdHandle,
                                                  &ConnectionParam);
    }
}
