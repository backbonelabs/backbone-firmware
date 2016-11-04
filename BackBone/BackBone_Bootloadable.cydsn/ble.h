/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#if !defined(BLEAPPLICATIONS_H)
#define BLEAPPLICATIONS_H

#include <project.h>
#include <stdbool.h>
	
/* General Macros */
#define BLE_TRUE							1
#define BLE_FALSE							0
#define BLE_ZERO							0

void ble_init();
void ble_update_connection_parameters();
bool ble_is_connected();
CYBLE_CONN_HANDLE_T* ble_get_connection();
void ble_app_event_handler(uint32 event, void* param);

#endif
/* [] END OF FILE */
