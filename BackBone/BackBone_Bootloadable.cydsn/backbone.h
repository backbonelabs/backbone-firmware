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

#ifndef BACKBONE_H_
#define BACKBONE_H_

#include <project.h>
#include <stdbool.h>

typedef enum
{
    BACKBONE_START_SESSION,
    BACKBONE_RESUME_SESSION,
    BACKBONE_PAUSE_SESSION,
    BACKBONE_STOP_SESSION,
    BACKBONE_RUN_ACCEL_SELFTEST
} backbone_session_control_t;

typedef union
{
    float axis[4];
    uint8 raw_data[16];
} backbone_accelerometer_t;
#define BACKBONE_ACCELEROMETER_DATA_LEN (16u)

typedef union
{
    struct
    {
        float distance;
        uint32_t elapsed_time;
    } fields;
    uint8 raw_data[8];
} backbone_distance_t;
#define BACKBONE_DISTANCE_DATA_LEN (8u)

typedef union
{
    struct
    {
        uint32_t flags;
        uint32_t total_time;
        uint32_t slouch_time;
    } fields;
    uint8_t raw_data[12];
} backbone_session_statistics_t;
#define BACKBONE_SESSION_STATISTICS_DATA_LEN (12u)

typedef union
{
    uint8_t slouch[1];
    uint8_t raw_data[1];
} backbone_slouch_t;
#define BACKBONE_SLOUCH_DATA_LEN (1u)

typedef union
{
    struct
    {
        uint32_t inv_init;
        uint32_t inv_selftest;
        uint32_t reserved1;
        uint32_t reserved2;
    } fields;
    uint8_t raw_data[16];
} backbone_status_t;
#define BACKBONE_STATUS_DATA_LEN (16u)

void backbone_init(void);
bool backbone_is_reset_pending(void);
bool backbone_is_reset_requested(void);
void backbone_clear_reset_requested(void);
bool backbone_is_selftest_requested();
void backbone_clear_selftest_requested();

void backbone_connected(CYBLE_CONN_HANDLE_T* connection);



void backbone_set_accelerometer_data(CYBLE_CONN_HANDLE_T* connection,
                                     backbone_accelerometer_t* data);
void backbone_set_accelerometer_notification(CYBLE_CONN_HANDLE_T* connection,
                                             bool enable);
void backbone_notify_accelerometer(CYBLE_CONN_HANDLE_T* connection);



void backbone_set_distance_data(CYBLE_CONN_HANDLE_T* connection,
                                backbone_distance_t* data);
void backbone_set_distance_notification(CYBLE_CONN_HANDLE_T* connection,
                                        bool enable);
void backbone_notify_distance(CYBLE_CONN_HANDLE_T* connection);



void backbone_set_slouch_data(CYBLE_CONN_HANDLE_T* connection,
                              backbone_slouch_t* data);
void backbone_set_slouch_notification(CYBLE_CONN_HANDLE_T* connection,
                                      bool enable);
void backbone_notify_slouch(CYBLE_CONN_HANDLE_T* connection);



void backbone_set_status_data(CYBLE_CONN_HANDLE_T* connection,
                              backbone_status_t* data);
void backbone_set_status_notification(CYBLE_CONN_HANDLE_T* connection,
                                             bool enable);
void backbone_notify_status(CYBLE_CONN_HANDLE_T* connection);



void backbone_enterbootloader(uint8_t* data, uint16_t len);



void backbone_controlsession(uint8_t* data, uint16_t len);



void backbone_set_session_statistics_data(CYBLE_CONN_HANDLE_T* connection,
                                          backbone_session_statistics_t* data);
void backbone_set_session_statistics_notification(CYBLE_CONN_HANDLE_T* connection,
                                                  bool enable);
void backbone_notify_session_statistics(CYBLE_CONN_HANDLE_T* connection);



void backbone_control_motor(uint8_t* data, uint16_t len);

#endif /* BACKBONE_H_ */
