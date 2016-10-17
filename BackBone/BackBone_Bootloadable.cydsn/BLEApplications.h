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

/********************************************************************************
*	Contains  macros and function declaration used in the BLEApplication.c file 
********************************************************************************/
#if !defined(BLEAPPLICATIONS_H)
#define BLEAPPLICATIONS_H

#include <project.h>
#include <BackBone.h>
	
/*************************Pre-processor directives****************************/
/* 'ENABLE_LOW_POWER_MODE' pre-processor directive enables the low power mode 
* handling in the firmware, ensuring low power consumption during project usage.
* To disable, comment the following #define. 
* If disabled, prevent usage of the project with coin cell */
#define ENABLE_LOW_POWER_MODE
	
/* 'CAPSENSE_ENABLED' pre-processor directive enables CapSense functionality 
* in the firmware. To disable Capsense, comment the following #define as well 
* as disable the CapSense component from the TopDesign */
#define CAPSENSE_ENABLED
/****************************************************************************/

/**************************Function Declarations*****************************/
void CustomEventHandler(uint32 event, void * eventParam);
void SendDataOverRGBledNotification(uint8 *rgbLedData, uint8 len);
void UpdateConnectionParam(void);
void HandleStatusLED(void);
void WDT_INT_Handler(void);
void InitializeWatchdog(void);
void SendAllNotifications(void);
void SendDataOverVersionInfoNotification(void);
/****************************************************************************/
	
/***************************Macro Declarations*******************************/
/* Client Characteristic Configuration descriptor data length. This is defined
* as per BLE spec. */
#define CCCD_DATA_LEN						2

/* Bit mask for notification bit in CCCD (Client Characteristic 
* Configuration Descriptor) written by Client device. */
#define CCCD_NTF_BIT_MASK					0x01

/* Minimum connection interval = CONN_PARAM_UPDATE_MIN_CONN_INTERVAL * 1.25 ms*/
#define CONN_PARAM_UPDATE_MIN_CONN_INTERVAL	50        	
/* Maximum connection interval = CONN_PARAM_UPDATE_MAX_CONN_INTERVAL * 1.25 ms */
#define CONN_PARAM_UPDATE_MAX_CONN_INTERVAL	52        	
/* Slave latency = Number of connection events */
#define CONN_PARAM_UPDATE_SLAVE_LATENCY		4          
/* Supervision timeout = CONN_PARAM_UPDATE_SUPRV_TIMEOUT * 10*/
#define CONN_PARAM_UPDATE_SUPRV_TIMEOUT		200     
	
/* General Macros */
#define TRUE							1
#define FALSE							0
#define ZERO							0

#endif
/* [] END OF FILE */
