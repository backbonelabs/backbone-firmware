/******************************************************************************
* Project Name		: PSoC_4_BLE_CapSense_Slider_LED
* File Name			: BLEApplications.c
* Version 			: 1.0
* Device Used		: CY8C4247LQI-BL483
* Software Used		: PSoC Creator 3.3 SP1
* Compiler    		: ARM GCC 4.9.3, ARM MDK Generic
* Related Hardware	: CY8CKIT-042-BLE Bluetooth Low Energy Pioneer Kit 
* Owner             : ROIT
*
********************************************************************************
* Copyright (2015-16), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 
*******************************************************************************/
#include <main.h>
#include <BLEApplications.h>
#include <OTAMandatory.h>

/**************************Variable Declarations*****************************/
static const uint8 EnterBootloaderKey[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

/* 'connectionHandle' stores connection parameters */
CYBLE_CONN_HANDLE_T  connectionHandle;

/*This flag is set when the Central device writes to CCCD (Client Characteristic 
* Configuration Descriptor) of the CapSense slider Characteristic to enable 
* notifications */
uint8 SendAccelNotifications = FALSE;
uint8 SendGyroNotifications = FALSE;
uint8 SendPedometerNotifications = FALSE;
uint8 SendVersionInfoNotifications = FALSE;

/* This flag is used by application to know whether a Central 
* device has been connected. This is updated in BLE event callback 
* function*/
uint8 deviceConnected = FALSE;

/* 'restartAdvertisement' flag provided the present state of power mode in firmware */
uint8 restartAdvertisement = FALSE;

/* This flag is used to let application send a L2CAP connection update request
* to Central device */
static uint8 isConnectionUpdateRequested = TRUE;

/* Connection Parameter update values. This values are used by the BLE component
* to update the connector parameter, including connection interval, to desired 
* value */
static CYBLE_GAP_CONN_UPDATE_PARAM_T ConnectionParam =
{
    CONN_PARAM_UPDATE_MIN_CONN_INTERVAL,  		      
    CONN_PARAM_UPDATE_MAX_CONN_INTERVAL,		       
    CONN_PARAM_UPDATE_SLAVE_LATENCY,			    
    CONN_PARAM_UPDATE_SUPRV_TIMEOUT 			         	
};

/* This flag is used to switch between CPU Deep Sleep and Sleep, depending on
* the status of RGB LED color values received. This method allows to conserve 
* more power while LED usage */
uint8 shut_down_led = TRUE;

/* Status flag for the Stack Busy state. This flag is used to notify the application 
* whether there is stack buffer free to push more data or not */
uint8 busyStatus = 0;

/* Local variable to store the current CCCD value */
uint8 CapSenseCCCDvalue[2];
uint8 RGBCCCDvalue[2];
uint8 AccelCCCDValue[2];
uint8 GyroCCCDValue[2];
uint8 PedometerCCCDValue[2];
uint8 VersionInfoCCCDValue[2];

/* Handle value to update the CCCD */
CYBLE_GATT_HANDLE_VALUE_PAIR_T AccelNotificationHandle;
CYBLE_GATT_HANDLE_VALUE_PAIR_T GyroNotificationHandle;
CYBLE_GATT_HANDLE_VALUE_PAIR_T PedometerNotificationHandle;
CYBLE_GATT_HANDLE_VALUE_PAIR_T VersionInfoNotificationHandle;

/****************************************************************************/

/*******************************************************************************
* Function Name: CustomEventHandler
********************************************************************************
* Summary:
*        Call back event function to handle various events from BLE stack
*
* Parameters:
*  event:		event returned
*  eventParam:	link to value of the events returned
*
* Return:
*  void
*
*******************************************************************************/
void CustomEventHandler(uint32 event, void * eventParam)
{
	uint8 i;
	uint8 KeyMatch = 0;
	
	/* Local variable to store the data received as part of the Write request 
	* events */
	CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;

    switch(event)
    {
        case CYBLE_EVT_STACK_ON:
			/* This event is received when component is Started */
			
			/* Set restartAdvertisement flag to allow calling Advertisement 
			* API from main function */
			restartAdvertisement = TRUE;
			
			break;
			
		case CYBLE_EVT_TIMEOUT:
			/* Event Handling for Timeout  */
	
			break;
        
		/**********************************************************
        *                       GAP Events
        ***********************************************************/
		case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
			
			/* If the current BLE state is Disconnected, then the Advertisement
			* Start Stop event implies that advertisement has stopped */
			if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
			{
				/* Set restartAdvertisement flag to allow calling Advertisement 
				* API from main function */
				restartAdvertisement = TRUE;
			}
			break;
			
		case CYBLE_EVT_GAP_DEVICE_CONNECTED: 					
			/* This event is received when device is connected over GAP layer */
			break;

			
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
			/* This event is received when device is disconnected */
			
			/* Set restartAdvertisement flag to allow calling Advertisement 
			* API from main function */
			restartAdvertisement = TRUE;
			
			/* Set flag to allow system to go to Deep Sleep */
			shut_down_led = TRUE;
			break;
        
		/**********************************************************
        *                       GATT Events
        ***********************************************************/
        case CYBLE_EVT_GATT_CONNECT_IND:
			/* This event is received when device is connected over GATT level */
			
			/* Update attribute handle on GATT Connection*/
            connectionHandle = *(CYBLE_CONN_HANDLE_T  *)eventParam;
			
			/* This flag is used in application to check connection status */
			deviceConnected = TRUE;
			
			break;
        
        case CYBLE_EVT_GATT_DISCONNECT_IND:
			/* This event is received when device is disconnected */
			
			/* Update deviceConnected flag*/
			deviceConnected = FALSE;
			
			/* Reset CapSense notification flag to prevent further notifications
			 * being sent to Central device after next connection. */
			SendAccelNotifications = FALSE;
			SendGyroNotifications = FALSE;
			SendPedometerNotifications = FALSE;
			
			/* Reset the isConnectionUpdateRequested flag to allow sending
			* connection parameter update request in next connection */
			isConnectionUpdateRequested = TRUE;
			
			/* Set the flag to allow system to go to Deep Sleep */
			shut_down_led = TRUE;
			
			break;
            
        case CYBLE_EVT_GATTS_WRITE_REQ:
		{
			/* This event is received when Central device sends a Write command on an Attribute */
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;

			if(CYBLE_BACKBONE_ACCELEROMETER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == wrReqParam->handleValPair.attrHandle)
			{
				if(wrReqParam->handleValPair.value.val[CYBLE_BACKBONE_ACCELEROMETER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == TRUE)
				{
					SendAccelNotifications = TRUE;	
				}
				else if(wrReqParam->handleValPair.value.val[CYBLE_BACKBONE_ACCELEROMETER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == FALSE)
				{
					SendAccelNotifications = FALSE;	
				}
				else
				{
				}
				
				AccelCCCDValue[0] = SendAccelNotifications;
				AccelCCCDValue[1] = 0x00;
				
				AccelNotificationHandle.attrHandle = CYBLE_BACKBONE_ACCELEROMETER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
				AccelNotificationHandle.value.val = AccelCCCDValue;
				AccelNotificationHandle.value.len = ACCEL_DATA_LEN;
				
				CyBle_GattsWriteAttributeValue(&AccelNotificationHandle, ZERO, &connectionHandle, CYBLE_GATT_DB_PEER_INITIATED);
			}
			
			if(CYBLE_BACKBONE_GYROSCOPE_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == wrReqParam->handleValPair.attrHandle)
			{
				if(wrReqParam->handleValPair.value.val[CYBLE_BACKBONE_GYROSCOPE_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == TRUE)
				{
					SendGyroNotifications = TRUE;	
				}
				else if (wrReqParam->handleValPair.value.val[CYBLE_BACKBONE_GYROSCOPE_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == FALSE)
				{
					SendGyroNotifications = FALSE;	
				}
				else
				{
				}
				
				GyroCCCDValue[0] = SendGyroNotifications;
				GyroCCCDValue[1] = 0x00;
				
				GyroNotificationHandle.attrHandle = CYBLE_BACKBONE_GYROSCOPE_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
				GyroNotificationHandle.value.val = GyroCCCDValue;
				GyroNotificationHandle.value.len = GYRO_DATA_LEN;
				
				CyBle_GattsWriteAttributeValue(&GyroNotificationHandle, ZERO, &connectionHandle, CYBLE_GATT_DB_PEER_INITIATED);
			}
			
			if(CYBLE_BACKBONE_PEDOMETER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == wrReqParam->handleValPair.attrHandle)
			{
				if(wrReqParam->handleValPair.value.val[CYBLE_BACKBONE_PEDOMETER_CHARACTERISTIC_USER_DESCRIPTION_DESC_INDEX] == TRUE)
				{
					SendPedometerNotifications = TRUE;	
				}
				else if (wrReqParam->handleValPair.value.val[CYBLE_BACKBONE_PEDOMETER_CHARACTERISTIC_USER_DESCRIPTION_DESC_INDEX] == FALSE)
				{
					SendPedometerNotifications = FALSE;
				}
				else
				{
				}
				
				PedometerCCCDValue[0] = SendPedometerNotifications;
				PedometerCCCDValue[1] = 0x00;
				
				PedometerNotificationHandle.attrHandle = CYBLE_BACKBONE_PEDOMETER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
				PedometerNotificationHandle.value.val = PedometerCCCDValue;
				PedometerNotificationHandle.value.len = PEDOMETER_DATA_LEN;
				
				CyBle_GattsWriteAttributeValue(&PedometerNotificationHandle, ZERO, &connectionHandle, CYBLE_GATT_DB_PEER_INITIATED);
			}
			
			if(CYBLE_BACKBONE_VERSION_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE == wrReqParam->handleValPair.attrHandle)
			{
				if(wrReqParam->handleValPair.value.val[CYBLE_BACKBONE_ACCELEROMETER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == TRUE)
				{
					SendVersionInfoNotifications = TRUE;	
				}
				else if(wrReqParam->handleValPair.value.val[CYBLE_BACKBONE_ACCELEROMETER_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_INDEX] == FALSE)
				{
					SendVersionInfoNotifications = FALSE;	
				}
				else
				{
				}
				
				VersionInfoCCCDValue[0] = SendVersionInfoNotifications;
				VersionInfoCCCDValue[1] = 0x00;
				
				VersionInfoNotificationHandle.attrHandle = CYBLE_BACKBONE_VERSION_CLIENT_CHARACTERISTIC_CONFIGURATION_DESC_HANDLE;
				VersionInfoNotificationHandle.value.val = VersionInfoCCCDValue;
				AccelNotificationHandle.value.len = VERSION_INFO_DATA_LEN;
				
				CyBle_GattsWriteAttributeValue(&VersionInfoNotificationHandle, ZERO, &connectionHandle, CYBLE_GATT_DB_PEER_INITIATED);
				
				BackBoneFlags |= VERSION_DATA_NEW;
			}

			/* */
			if(CYBLE_BACKBONE_CONFIGDATA_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
			{
				  BackBone_SetConfigData(wrReqParam->handleValPair.value.val, CONFIG_DATA_LEN);
			}

			if(CYBLE_BACKBONE_WRITE_DATA_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
			{
				BLEWriteData[0] = wrReqParam->handleValPair.value.val[0];
				BLEWriteData[1] = wrReqParam->handleValPair.value.val[1];
				BLEWriteData[2] = wrReqParam->handleValPair.value.val[2];
				BLEWriteData[3] = wrReqParam->handleValPair.value.val[3];
				
				MotorPWMDutyCycle = BLEWriteData[1];
				
				if(BLEWriteData[0] != 0)
				{
					MotorFlag = 1;
					MotorPWM_Start();
					
					MotorPWM_WriteCompare(MotorPWMDutyCycle);
				}
				else
				{
					MotorFlag = 0;
					MotorPWM_Stop();
				}
			}
			
			if(CYBLE_BACKBONE_ENTERBOOTLOADER_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
			{
				KeyMatch = 1;
				
				for(i=0; i<8; i++)
				{
					if (wrReqParam->handleValPair.value.val[i] != EnterBootloaderKey[i])
					{
						KeyMatch = 0;
						break;
					}
				}
				
				if(KeyMatch)
				{
		            MotorPWM_Stop();
                    //Switch to the Stack project, which enables OTA service
                    Bootloadable_SetActiveApplication(0); 
                    Bootloadable_Load(); 
                    CySoftwareReset();
				}
			}

			/* Send the response to the write request received. */
			CyBle_GattsWriteRsp(connectionHandle);
			
			break;
		}
			
		case CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_RSP:
				/* If L2CAP connection parameter update response received, reset application flag */
            	isConnectionUpdateRequested = FALSE;
            break;
			
		case CYBLE_EVT_STACK_BUSY_STATUS:
			/* This event is generated when the internal stack buffer is full and no more
			* data can be accepted or the stack has buffer available and can accept data.
			* This event is used by application to prevent pushing lot of data to stack. */
			
			/* Extract the present stack status */
            busyStatus = * (uint8*)eventParam;
            break;
			
        default:

       	 	break;
    }   	/* switch(event) */
}

void SendAllNotifications(void)
{
	CYBLE_GATTS_HANDLE_VALUE_NTF_T NotificationHandle;

	if(BackBoneFlags & ACCEL_DATA_NEW)
	{
		if(busyStatus == CYBLE_STACK_STATE_FREE)
		{
			NotificationHandle.attrHandle = CYBLE_BACKBONE_ACCELEROMETER_CHAR_HANDLE;
			NotificationHandle.value.val = Accelerometer.RawData;
			NotificationHandle.value.len = ACCEL_DATA_LEN;
			
			/* Send the updated handle as part of attribute for notifications */
			CyBle_GattsNotification(connectionHandle,&NotificationHandle);
			
			BackBoneFlags &= ~ACCEL_DATA_NEW;
		}
	}
	
	if(BackBoneFlags & GYRO_DATA_NEW)
	{
		if(busyStatus == CYBLE_STACK_STATE_FREE)
		{
			NotificationHandle.attrHandle = CYBLE_BACKBONE_GYROSCOPE_CHAR_HANDLE;
			NotificationHandle.value.val = Gyroscope.RawData;
			NotificationHandle.value.len = GYRO_DATA_LEN;
			
			/* Send the updated handle as part of attribute for notifications */
			CyBle_GattsNotification(connectionHandle,&NotificationHandle);
			
			BackBoneFlags &= ~GYRO_DATA_NEW;
		}
	}
	
	if(BackBoneFlags & PEDO_DATA_NEW)
	{
		if(busyStatus == CYBLE_STACK_STATE_FREE)
		{
			NotificationHandle.attrHandle = CYBLE_BACKBONE_PEDOMETER_CHAR_HANDLE;
			NotificationHandle.value.val = Pedometer.RawData;
			NotificationHandle.value.len = PEDOMETER_DATA_LEN;
			
			/* Send the updated handle as part of attribute for notifications */
			CyBle_GattsNotification(connectionHandle,&NotificationHandle);
			
			BackBoneFlags &= ~PEDO_DATA_NEW;
		}
	}
	
	if(BackBoneFlags & VERSION_DATA_NEW)
	{
		if(busyStatus == CYBLE_STACK_STATE_FREE)
		{
			NotificationHandle.attrHandle = CYBLE_BACKBONE_VERSION_CHAR_HANDLE;
			NotificationHandle.value.val = VersionString;
			NotificationHandle.value.len = sizeof(VersionString);
			
			/* Send the updated handle as part of attribute for notifications */
			CyBle_GattsNotification(connectionHandle,&NotificationHandle);
			
			BackBoneFlags &= ~VERSION_DATA_NEW;
		}
	}
}

/*******************************************************************************
* Function Name: SendDataOverVersionInfoNotification
********************************************************************************
* Summary:
*        Send version information BLE notification
*
* Parameters:
*  void	
*
* Return:
*  void
*
*******************************************************************************/
void SendDataOverVersionInfoNotification(void)
{
	/* 'CapSensenotificationHandle' stores CapSense notification data parameters */
	CYBLE_GATTS_HANDLE_VALUE_NTF_T		VersionInfoHandle;	
	
	/* If stack is not busy, then send the notification */
	if(busyStatus == CYBLE_STACK_STATE_FREE)
	{
		/* Update notification handle with CapSense slider data*/
		VersionInfoHandle.attrHandle = CYBLE_BACKBONE_VERSION_CHAR_HANDLE;				
		VersionInfoHandle.value.val = VersionString;
		VersionInfoHandle.value.len = sizeof(VersionString);
		
		/* Send the updated handle as part of attribute for notifications */
		CyBle_GattsNotification(connectionHandle,&VersionInfoHandle);
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
*	void
*
* Return:
*  void
*
*******************************************************************************/
void UpdateConnectionParam(void)
{
	/* If device is connected and Update connection parameter not updated yet,
	* then send the Connection Parameter Update request to Client. */
    if(deviceConnected && isConnectionUpdateRequested)
	{
		/* Reset the flag to indicate that connection Update request has been sent */
		isConnectionUpdateRequested = FALSE;
		
		/* Send Connection Update request with set Parameter */
		CyBle_L2capLeConnectionParamUpdateRequest(connectionHandle.bdHandle, &ConnectionParam);
	}
}



/*******************************************************************************
* Function Name: WDT_INT_Handler
********************************************************************************
* Summary:
*        Watchdog interrupt routine for controlling LED status in connected state
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void WDT_INT_Handler(void)
{
//	/* If the Interrupt source is Counter 0 match, then process */
//	if(CySysWdtGetInterruptSource() & CY_SYS_WDT_COUNTER0_INT)
//	{
//		/* Clear Watchdog Interrupt from Counter 0 */
//		CySysWdtClearInterrupt(CY_SYS_WDT_COUNTER0_INT);
//		
//		/* If switch_off_status_led is TRUE, then the Connection status LED 
//		* is ON and it is required to shut it down after 3 seconds */
//		if(switch_off_status_led)
//		{
//			/* If timer ticks for Connection ON LED period has expired, then switch
//			* off LED */
//			if(timer_tick == FALSE)
//			{
//				/* Switch of Status LED */
//				PrISM_1_WritePulse0(RGB_LED_OFF);
//				
//				/* Set the drive mode of LED to Analog HiZ to prevent leakage current */
//				RED_SetDriveMode(RED_DM_ALG_HIZ);
//				
//				/* Reset the Flag */
//				switch_off_status_led = FALSE;
//				
//			}
//			else
//			{
//				/* Decrement timer_tick as counting method */
//				timer_tick--;
//			}
//		}
//		/* If the LED activity for Connection LED ON is completed, then firmware is 
//		* allowed to do timing for RGB LED Control operation after connection. This 
//		* operation keeps the RGB LED ON for determined time (default 3 seconds) from
//		* the last time RGB LED data was sent. This ensures that continuous usage of 
//		* the kit in RGB mode does not consume too much power */
//		else
//		{
//			/* After displaying color for predetermined time, switch off 
//			* the LEDs to save power */
//			if(FALSE == led_timer)
//			{
//				/* Set the flag to indicate to the Low power mode function that system 
//				* can be put to Deep Sleep as no LED operation is required */
//				shut_down_led= TRUE;
//				
//				/* Shut down LEDs*/
//				RED_SetDriveMode(RED_DM_ALG_HIZ);
//				GREEN_SetDriveMode(GREEN_DM_ALG_HIZ);
//				BLUE_SetDriveMode(BLUE_DM_ALG_HIZ);
//			}
//			else
//			{	
//				/* Decrement the led_timer as counting method for RGB LED Control. During
//				* this period, the PrISM is active and drives the signal to display the 
//				* appropriate color */
//				led_timer--;
//			}
//		}
//		
//		/* Unlock the WDT registers for modification */
//		CySysWdtUnlock();
//		
//		/* Disable Counter 0 to allow modifications */
//		CySysWdtDisable(CY_SYS_WDT_COUNTER0_MASK);
//		
//		/* Reset Counter 0 and give ~3 LFCLK cycles to take effect */
//		CySysWdtResetCounters(CY_SYS_WDT_COUNTER0_RESET);
//		CyDelayUs(WATCHDOG_REG_UPDATE_WAIT_TIME);
//		
//		/* Write the Counter 0 match value for 1 second and give ~3 LFCLK
//		* cycles to take effect */
//		CySysWdtWriteMatch(CY_SYS_WDT_COUNTER0, WATCHDOG_ONE_SEC_COUNT_VAL);
//		CyDelayUs(WATCHDOG_REG_UPDATE_WAIT_TIME);
//		
//		/* Enable Watchdog Counter 0 */
//		CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
//		
//		/* Lock Watchdog to prevent any further change */
//	    CySysWdtLock();
//	}
}

/*******************************************************************************
* Function Name: InitializeWatchdog
********************************************************************************
* Summary:
*        Reset watchdog timer
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void InitializeWatchdog(void)
{
//	/* Unlock the WDT registers for modification */
//	CySysWdtUnlock(); 
//	
//	/* Write Mode for Counter 0 as Interrupt on Match */
//    CySysWdtWriteMode(CY_SYS_WDT_COUNTER0, CY_SYS_WDT_MODE_INT);
//	
//	/* Set Clear on Match for Counter 0*/
//	CySysWdtWriteClearOnMatch(CY_SYS_WDT_COUNTER0, TRUE);
//    
//	/* Set Watchdog interrupt to lower priority */
//	CyIntSetPriority(WATCHDOG_INT_VEC_NUM, WATCHDOG_INT_VEC_PRIORITY);
//	
//	/* Enable Watchdog Interrupt using Interrupt number */
//    CyIntEnable(WATCHDOG_INT_VEC_NUM);
//	
//	/* Write the match value equal to 1 second in Counter 0 */
//	CySysWdtWriteMatch(CY_SYS_WDT_COUNTER0, WATCHDOG_ONE_SEC_COUNT_VAL);
//    
//	/* Enable Counter 0 */
//    CySysWdtEnable(CY_SYS_WDT_COUNTER0_MASK);
//	
//	/* Lock Watchdog to prevent further changes */
//    CySysWdtLock();
}
/* [] END OF FILE */
