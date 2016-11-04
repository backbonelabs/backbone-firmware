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

#ifndef BLE_H_
#define BLE_H_

#include <project.h>
#include <stdbool.h>

/* General Macros */
#define BLE_TRUE                            1
#define BLE_FALSE                           0
#define BLE_ZERO                            0

void ble_init();
void ble_update_connection_parameters();
bool ble_is_connected();
CYBLE_CONN_HANDLE_T* ble_get_connection();
void ble_app_event_handler(uint32 event, void* param);

#endif /* BLE_H_ */
