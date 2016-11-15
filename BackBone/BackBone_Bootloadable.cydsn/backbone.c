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

#include <project.h>

#include "backbone.h"
#include "ble.h"
#include "motor.h"
#include "OTAMandatory.h"
#include "posture.h"
#include "version.h"

uint8 accelerometer_cccd[2];
uint8 distance_cccd[2];
uint8 session_statistics_cccd[2];

void backbone_init()
{
}

void backbone_connected(CYBLE_CONN_HANDLE_T* connection)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;
    uint8_t data[] = {HW_MAJOR_VERSION, HW_MINOR_VERSION, FW_MAJOR_VERSION, FW_MINOR_VERSION};

    characteristic.attrHandle = CYBLE_BACKBONE_VERSION_CHAR_HANDLE;
    characteristic.value.val = data;
    characteristic.value.len = sizeof(data);

    CyBle_GattsWriteAttributeValue(&characteristic,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
}

void backbone_set_accelerometer_data(CYBLE_CONN_HANDLE_T* connection,
                                     backbone_accelerometer_t* data)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;

    characteristic.attrHandle = CYBLE_BACKBONE_ACCELEROMETER_CHAR_HANDLE;
    characteristic.value.val = data->raw_data;
    characteristic.value.len = BACKBONE_ACCELEROMETER_DATA_LEN;

    CyBle_GattsWriteAttributeValue(&characteristic,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
}

void backbone_set_accelerometer_notification(CYBLE_CONN_HANDLE_T* connection,
                                             bool enable)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T attribute;

    accelerometer_cccd[0] = enable ? BLE_TRUE : BLE_FALSE;
    accelerometer_cccd[1] = 0x00;

    attribute.attrHandle = CYBLE_BACKBONE_ACCELEROMETER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    attribute.value.val = accelerometer_cccd;
    attribute.value.len = sizeof(accelerometer_cccd);

    CyBle_GattsWriteAttributeValue(&attribute,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_PEER_INITIATED);
}

void backbone_notify_accelerometer(CYBLE_CONN_HANDLE_T* connection)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;
    backbone_accelerometer_t data;

    if (accelerometer_cccd[0] == BLE_TRUE)
    {
        characteristic.attrHandle = CYBLE_BACKBONE_ACCELEROMETER_CHAR_HANDLE;
        characteristic.value.val = data.raw_data;
        characteristic.value.len = BACKBONE_ACCELEROMETER_DATA_LEN;
        CyBle_GattsReadAttributeValue(&characteristic, connection, 0);

        notification.attrHandle = CYBLE_BACKBONE_ACCELEROMETER_CHAR_HANDLE;
        notification.value.val = data.raw_data;
        notification.value.len = BACKBONE_ACCELEROMETER_DATA_LEN;
        CyBle_GattsNotification(*connection, &notification);
    }
}



void backbone_set_distance_data(CYBLE_CONN_HANDLE_T* connection,
                                backbone_distance_t* data)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;

    characteristic.attrHandle = CYBLE_BACKBONE_DISTANCE_CHAR_HANDLE;
    characteristic.value.val = data->raw_data;
    characteristic.value.len = BACKBONE_DISTANCE_DATA_LEN;

    CyBle_GattsWriteAttributeValue(&characteristic,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
}

void backbone_set_distance_notification(CYBLE_CONN_HANDLE_T* connection,
                                        bool enable)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T attribute;

    distance_cccd[0] = enable ? BLE_TRUE : BLE_FALSE;
    distance_cccd[1] = 0x00;

    attribute.attrHandle = CYBLE_BACKBONE_DISTANCE_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    attribute.value.val = distance_cccd;
    attribute.value.len = sizeof(distance_cccd);

    CyBle_GattsWriteAttributeValue(&attribute,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_PEER_INITIATED);
}

void backbone_notify_distance(CYBLE_CONN_HANDLE_T* connection)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;
    backbone_distance_t data;

    if (distance_cccd[0] == BLE_TRUE)
    {
        characteristic.attrHandle = CYBLE_BACKBONE_DISTANCE_CHAR_HANDLE;
        characteristic.value.val = data.raw_data;
        characteristic.value.len = BACKBONE_DISTANCE_DATA_LEN;
        CyBle_GattsReadAttributeValue(&characteristic, connection, 0);

        notification.attrHandle = CYBLE_BACKBONE_DISTANCE_CHAR_HANDLE;
        notification.value.val = data.raw_data;
        notification.value.len = BACKBONE_DISTANCE_DATA_LEN;
        CyBle_GattsNotification(*connection, &notification);
    }
}

void backbone_enterbootloader(uint8_t* data, uint16_t len)
{
    static const uint8 ENTER_BOOTLOADER_KEY[8] =
    {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88
    };
    uint8 i;
    uint8 key_match = 1;

    if (len == sizeof(ENTER_BOOTLOADER_KEY))
    {
        for (i=0; i<sizeof(ENTER_BOOTLOADER_KEY); i++)
        {
            if (data[i] != ENTER_BOOTLOADER_KEY[i])
            {
                key_match = 0;
                break;
            }
        }
    }
    else
    {
        key_match = 0;
    }

    if (key_match)
    {
        //TODO: Add callback to notify main app to reset into the bootloader

        MotorPWM_Stop();
        //Switch to the Stack project, which enables OTA service
        Bootloadable_SetActiveApplication(0);
        Bootloadable_Load();
        CySoftwareReset();
    }
}

void backbone_set_session_statistics_notification(CYBLE_CONN_HANDLE_T* connection,
                                                  bool enable)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T attribute;

    session_statistics_cccd[0] = enable ? BLE_TRUE : BLE_FALSE;
    session_statistics_cccd[1] = 0x00;

    attribute.attrHandle = CYBLE_BACKBONE_SESSION_STATISTICS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    attribute.value.val = session_statistics_cccd;
    attribute.value.len = sizeof(session_statistics_cccd);

    CyBle_GattsWriteAttributeValue(&attribute,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_PEER_INITIATED);
}

void backbone_notify_session_statistics(CYBLE_CONN_HANDLE_T* connection)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;
    backbone_session_statistics_t data;

    if (session_statistics_cccd[0] == BLE_TRUE)
    {
        characteristic.attrHandle = CYBLE_BACKBONE_DISTANCE_CHAR_HANDLE;
        characteristic.value.val = data.raw_data;
        characteristic.value.len = BACKBONE_SESSION_STATISTICS_DATA_LEN;
        CyBle_GattsReadAttributeValue(&characteristic, connection, 0);

        notification.attrHandle = CYBLE_BACKBONE_SESSION_STATISTICS_CHAR_HANDLE;
        notification.value.val = data.raw_data;
        notification.value.len = BACKBONE_DISTANCE_DATA_LEN;
        CyBle_GattsNotification(*connection, &notification);
    }
}

void backbone_controlsession(uint8_t* data, uint16_t len)
{
    if (len < 1)
    {
        return;
    }

    switch (data[0])
    {
        case BACKBONE_START_SESSION:
            posture_start();
            break;

        case BACKBONE_STOP_SESSION:
            posture_stop();
            break;

        case BACKBONE_PAUSE_SESSION:
            //posture_pause();
            break;

        case BACKBONE_RESUME_SESSION:
            //posture_resume();
            break;
    }
}

void backbone_control_motor(uint8_t* data, uint16_t len)
{
    if (len > 0 && data[0] == 1)
    {
        uint8_t duty_cycle = len > 1 ? data[1] : 0x50;
        uint16_t milliseconds = len > 3 ? data[2] << 8 | data[3] : 500;
        motor_start(duty_cycle, milliseconds);
    }
    else
    {
        motor_stop();
    }
}