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
#include "debug.h"
#include "watchdog.h"
#include "inv.h"
#include "bas.h"

static uint8 accelerometer_cccd[2];
static uint8 distance_cccd[2];
static uint8 slouch_cccd[2];
static uint8 session_statistics_cccd[2];
static uint8 status_cccd[2];
static uint8 step_count_cccd[2];
static bool m_reset_pending;
static bool m_reset_requested;
static bool m_self_test_requested;
static bool m_step_count_dirty;

void backbone_init()
{
    m_reset_pending = false;
    m_reset_requested = false;
    m_self_test_requested = false;
    m_step_count_dirty = false;
}

bool backbone_is_reset_pending()
{
    return m_reset_pending;
}

bool backbone_is_reset_requested()
{
    return m_reset_requested;
}

void backbone_clear_reset_requested()
{
    m_reset_requested = false;
}

bool backbone_is_selftest_requested()
{
    return m_self_test_requested;
}

void backbone_clear_selftest_requested()
{
    m_self_test_requested = false;
}

void backbone_connected(CYBLE_CONN_HANDLE_T* connection)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;
    uint8_t data[] = {HW_MAJOR_VERSION, HW_MINOR_VERSION, FW_MAJOR_VERSION, FW_MINOR_VERSION};
    backbone_status_t status;
    backbone_step_count_t step_count;

    characteristic.attrHandle = CYBLE_BACKBONE_VERSION_CHAR_HANDLE;
    characteristic.value.val = data;
    characteristic.value.len = sizeof(data);

    CyBle_GattsWriteAttributeValue(&characteristic,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);

    int32 mvolts = MeasureBattery(true);
    status.fields.inv_init = inv_get_init_status();
    status.fields.inv_selftest = inv_get_selftest_status();
    status.fields.reserved1 = mvolts;
    status.fields.reserved2 = 0;
    backbone_set_status_data(connection, &status);

    step_count.fields.step_count = inv_get_step_count();
    backbone_set_step_count_data(connection, &step_count);
    // Update session statistics in the GATT database incase a session finished
    // while disconnected
    if (posture_get_elapsed_time() >= posture_get_session_duration() &&
        posture_get_session_duration() != 0)
    {
        backbone_session_statistics_t session_statistics_data;

        session_statistics_data.fields.flags = 0;
        session_statistics_data.fields.total_time = posture_get_elapsed_time();
        session_statistics_data.fields.slouch_time = posture_get_slouch_time();
        backbone_set_session_statistics_data(ble_get_connection(), &session_statistics_data);
        backbone_notify_session_statistics(ble_get_connection());
    }
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

void backbone_set_slouch_data(CYBLE_CONN_HANDLE_T* connection,
                              backbone_slouch_t* data)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;

    characteristic.attrHandle = CYBLE_BACKBONE_SLOUCH_CHAR_HANDLE;
    characteristic.value.val = data->raw_data;
    characteristic.value.len = BACKBONE_SLOUCH_DATA_LEN;

    CyBle_GattsWriteAttributeValue(&characteristic,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
}

void backbone_set_slouch_notification(CYBLE_CONN_HANDLE_T* connection,
                                      bool enable)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T attribute;

    slouch_cccd[0] = enable ? BLE_TRUE : BLE_FALSE;
    slouch_cccd[1] = 0x00;

    attribute.attrHandle = CYBLE_BACKBONE_SLOUCH_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    attribute.value.val = slouch_cccd;
    attribute.value.len = sizeof(slouch_cccd);

    CyBle_GattsWriteAttributeValue(&attribute,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_PEER_INITIATED);
}

void backbone_notify_slouch(CYBLE_CONN_HANDLE_T* connection)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;
    backbone_slouch_t data;

    if (slouch_cccd[0] == BLE_TRUE)
    {
        characteristic.attrHandle = CYBLE_BACKBONE_SLOUCH_CHAR_HANDLE;
        characteristic.value.val = data.raw_data;
        characteristic.value.len = BACKBONE_SLOUCH_DATA_LEN;
        CyBle_GattsReadAttributeValue(&characteristic, connection, 0);

        notification.attrHandle = CYBLE_BACKBONE_SLOUCH_CHAR_HANDLE;
        notification.value.val = data.raw_data;
        notification.value.len = BACKBONE_SLOUCH_DATA_LEN;
        CyBle_GattsNotification(*connection, &notification);
    }
}

void backbone_set_status_data(CYBLE_CONN_HANDLE_T* connection,
                              backbone_status_t* data)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;

    characteristic.attrHandle = CYBLE_BACKBONE_STATUS_CHAR_HANDLE;
    characteristic.value.val = data->raw_data;
    characteristic.value.len = BACKBONE_STATUS_DATA_LEN;

    CyBle_GattsWriteAttributeValue(&characteristic,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
}

void backbone_set_status_notification(CYBLE_CONN_HANDLE_T* connection,
                                      bool enable)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T attribute;

    status_cccd[0] = enable ? BLE_TRUE : BLE_FALSE;
    status_cccd[1] = 0x00;

    attribute.attrHandle = CYBLE_BACKBONE_STATUS_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    attribute.value.val = status_cccd;
    attribute.value.len = sizeof(status_cccd);

    CyBle_GattsWriteAttributeValue(&attribute,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_PEER_INITIATED);
}

void backbone_notify_status(CYBLE_CONN_HANDLE_T* connection)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;
    backbone_status_t data;

    if (status_cccd[0] == BLE_TRUE)
    {
        characteristic.attrHandle = CYBLE_BACKBONE_STATUS_CHAR_HANDLE;
        characteristic.value.val = data.raw_data;
        characteristic.value.len = BACKBONE_STATUS_DATA_LEN;
        CyBle_GattsReadAttributeValue(&characteristic, connection, 0);

        notification.attrHandle = CYBLE_BACKBONE_STATUS_CHAR_HANDLE;
        notification.value.val = data.raw_data;
        notification.value.len = BACKBONE_STATUS_DATA_LEN;
        CyBle_GattsNotification(*connection, &notification);
    }
}

void backbone_set_step_count_data(CYBLE_CONN_HANDLE_T* connection,
                                  backbone_step_count_t* data)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;
    backbone_step_count_t old_value;

    characteristic.attrHandle = CYBLE_BACKBONE_STEP_COUNT_CHAR_HANDLE;
    characteristic.value.val = old_value.raw_data;
    characteristic.value.len = BACKBONE_STEP_COUNT_DATA_LEN;
    CyBle_GattsReadAttributeValue(&characteristic, connection, CYBLE_GATT_DB_LOCALLY_INITIATED);

    if (old_value.fields.step_count != data->fields.step_count)
    {
        m_step_count_dirty = true;
    }

    characteristic.value.val = data->raw_data;
    CyBle_GattsWriteAttributeValue(&characteristic,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
}

void backbone_set_step_count_notification(CYBLE_CONN_HANDLE_T* connection,
                                          bool enable)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T attribute;

    step_count_cccd[0] = enable ? BLE_TRUE : BLE_FALSE;
    step_count_cccd[1] = 0x00;

    attribute.attrHandle = CYBLE_BACKBONE_STEP_COUNT_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
    attribute.value.val = status_cccd;
    attribute.value.len = sizeof(status_cccd);

    CyBle_GattsWriteAttributeValue(&attribute,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_PEER_INITIATED);
}

void backbone_notify_step_count(CYBLE_CONN_HANDLE_T* connection)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;
    backbone_step_count_t data;

    if (step_count_cccd[0] == BLE_TRUE && m_step_count_dirty)
    {
        m_step_count_dirty = false;

        characteristic.attrHandle = CYBLE_BACKBONE_STEP_COUNT_CHAR_HANDLE;
        characteristic.value.val = data.raw_data;
        characteristic.value.len = BACKBONE_STEP_COUNT_DATA_LEN;
        CyBle_GattsReadAttributeValue(&characteristic, connection, 0);

        notification.attrHandle = CYBLE_BACKBONE_STEP_COUNT_CHAR_HANDLE;
        notification.value.val = data.raw_data;
        notification.value.len = BACKBONE_STEP_COUNT_DATA_LEN;
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
        m_reset_requested = true;
        m_reset_pending = true;
    }
}

void backbone_set_session_statistics_data(CYBLE_CONN_HANDLE_T* connection,
                                          backbone_session_statistics_t* data)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T characteristic;

    characteristic.attrHandle = CYBLE_BACKBONE_SESSION_STATISTICS_CHAR_HANDLE;
    characteristic.value.val = data->raw_data;
    characteristic.value.len = BACKBONE_SESSION_STATISTICS_DATA_LEN;

    CyBle_GattsWriteAttributeValue(&characteristic,
                                   0,
                                   connection,
                                   CYBLE_GATT_DB_LOCALLY_INITIATED);
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
        characteristic.attrHandle = CYBLE_BACKBONE_SESSION_STATISTICS_CHAR_HANDLE;
        characteristic.value.val = data.raw_data;
        characteristic.value.len = BACKBONE_SESSION_STATISTICS_DATA_LEN;
        CyBle_GattsReadAttributeValue(&characteristic, connection, 0);

        notification.attrHandle = CYBLE_BACKBONE_SESSION_STATISTICS_CHAR_HANDLE;
        notification.value.val = data.raw_data;
        notification.value.len = BACKBONE_SESSION_STATISTICS_DATA_LEN;
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
        // cmd  duration(s)  distance_threshold  time_threshold  pattern  duty_cycle  motor_on_time
        //  00  00 00 00 3C               03 E8           00 05       01          50             32
        case BACKBONE_START_SESSION:
            if (len == 12)
            {
                uint32_t duration = data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4];
                float distance_threshold = (float)((data[5] << 8 | data[6])) / 10000.0;
                uint16_t time_threshold = data[7] << 8 | data[8];
                uint8_t pattern = data[9];
                uint8_t duty_cycle = data[10];
                uint16_t motor_on_time = data[11] * 10;
                posture_start(watchdog_get_time(),
                              duration,
                              distance_threshold,
                              time_threshold,
                              pattern,
                              duty_cycle,
                              motor_on_time);
            }
            break;

        case BACKBONE_STOP_SESSION:
            posture_stop();

            backbone_session_statistics_t session_statistics_data;

            session_statistics_data.fields.flags = 0;
            session_statistics_data.fields.total_time = posture_get_elapsed_time();
            session_statistics_data.fields.slouch_time = posture_get_slouch_time();
            backbone_set_session_statistics_data(ble_get_connection(), &session_statistics_data);
            backbone_notify_session_statistics(ble_get_connection());
            break;

        case BACKBONE_PAUSE_SESSION:
            posture_pause();
            break;

        // cmd  distance_threshold  time_threshold  pattern  duty_cycle  motor_on_time
        //  01               03 E8           00 05       01          50             32
        case BACKBONE_RESUME_SESSION:
            if (len == 8)
            {
                float distance_threshold = (float)((data[1] << 8 | data[2])) / 10000.0;
                uint16_t time_threshold = data[3] << 8 | data[4];
                uint8_t pattern = data[5];
                uint8_t duty_cycle = data[6];
                uint16_t motor_on_time = data[7] * 10;
                posture_resume(watchdog_get_time(),
                               distance_threshold,
                               time_threshold,
                               pattern,
                               duty_cycle,
                               motor_on_time);
            }
            break;

        case BACKBONE_RUN_ACCEL_SELFTEST:
            m_self_test_requested = true;
            break;

        case BACKBONE_START_ACCELEROMETER:
            inv_enable_accelerometer();
            break;

        case BACKBONE_STOP_ACCELEROMETER:
            inv_disable_accelerometer();
            break;

        case BACKBONE_SET_TIME:
            if (len == 5)
            {
                uint32_t day_time = data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4];
                watchdog_set_day_time(day_time);
            }
            break;
    }
}

void backbone_control_motor(uint8_t* data, uint16_t len)
{
    //01 50 10 00
    if (len > 0 && data[0] == 1)
    {
        uint8_t duty_cycle = len > 1 ? data[1] : 0x50;
        uint16_t milliseconds = len > 3 ? data[2] << 8 | data[3] : 500;
        motor_start(duty_cycle, milliseconds, 1);
    }
    else
    {
        motor_stop();
    }
}
