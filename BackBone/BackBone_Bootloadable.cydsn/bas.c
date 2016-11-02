/*******************************************************************************
* File Name: bas.c
*
* Version: 1.0
*
* Description:
*  This file contains BAS callback handler function.
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
* 
********************************************************************************
* Copyright 2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "bas.h"
#include "common.h"
#include <OTAMandatory.h>

uint16 batterySimulationNotify = 0u;
uint16 batteryMeasureNotify = 0u;


#define VDD					(3100u)
#define ADC_MAX_COUNTS		(2048u)

/*******************************************************************************
* Function Name: BasCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive service specific events from 
*   Battery Service.
*
* Parameters:
*  event - the event code
*  *eventParam - the event parameters
*
* Return:
*  None.
*
********************************************************************************/
void BasCallBack(uint32 event, void *eventParam)
{
    uint8 locServiceIndex;
    
    locServiceIndex = ((CYBLE_BAS_CHAR_VALUE_T *)eventParam)->serviceIndex;
    
    switch(event)
    {
        case CYBLE_EVT_BASS_NOTIFICATION_ENABLED:
            if(CYBLE_BATTERY_SERVICE_MEASURE_SERVICE_INDEX == locServiceIndex)
            {
                batteryMeasureNotify = ENABLED;
            }
            break;
                
        case CYBLE_EVT_BASS_NOTIFICATION_DISABLED:
            if(CYBLE_BATTERY_SERVICE_MEASURE_SERVICE_INDEX == locServiceIndex)
            {
                batteryMeasureNotify = DISABLED;
            }
            break;
        case CYBLE_EVT_BASC_NOTIFICATION:
            break;
        case CYBLE_EVT_BASC_READ_CHAR_RESPONSE:
            break;
        case CYBLE_EVT_BASC_READ_DESCR_RESPONSE:
            break;
        case CYBLE_EVT_BASC_WRITE_DESCR_RESPONSE:
            break;
		default:
			break;
    }
}


/*******************************************************************************
* Function Name: MeasureBattery()
********************************************************************************
*
* Summary:
*   This function measures the battery voltage and send it to the client.
*
*******************************************************************************/
void MeasureBattery(void)
{
	int16 adcResult;
    int32 mvolts;
    uint8 batteryLevel;
    CYBLE_API_RESULT_T apiResult;
    
    static uint32 batteryTimer = BATTERY_TIMEOUT;
    
    if(--batteryTimer == 0u) 
    {
        batteryTimer = BATTERY_TIMEOUT;
        
    	ADC_StartConvert();
    	ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);

        adcResult = ADC_GetResult16(ADC_BATTERY_CHANNEL);

		/* Calculate input voltage by using ratio of ADC counts from reference
    	*  and ADC Full Scale counts. 
        */
    	mvolts = (adcResult * VDD * 2) / ADC_MAX_COUNTS;
        
        /* Convert battery level voltage to percentage using linear approximation
        *  divided into two sections according to typical performance of 
        *  CR2033 battery specification:
        *  3V - 100%
        *  2.8V - 29%
        *  2.0V - 0%
        */
        if(mvolts < MEASURE_BATTERY_MIN)
        {
            batteryLevel = 0;
        }
        else if(mvolts < MEASURE_BATTERY_MID)
        {
            batteryLevel = (mvolts - MEASURE_BATTERY_MIN) * MEASURE_BATTERY_MID_PERCENT / 
                           (MEASURE_BATTERY_MID - MEASURE_BATTERY_MIN); 
        }
        else if(mvolts < MEASURE_BATTERY_MAX)
        {
            batteryLevel = MEASURE_BATTERY_MID_PERCENT +
                           (mvolts - MEASURE_BATTERY_MID) * (100 - MEASURE_BATTERY_MID_PERCENT) / 
                           (MEASURE_BATTERY_MAX - MEASURE_BATTERY_MID); 
        }
        else
        {
            batteryLevel = CYBLE_BAS_MAX_BATTERY_LEVEL_VALUE;
        }
        
        if(batteryMeasureNotify == ENABLED)
        {
            /* Update Battery Level characteristic value and send Notification */
            apiResult = CyBle_BassSendNotification(cyBle_connHandle, CYBLE_BATTERY_SERVICE_MEASURE_SERVICE_INDEX, 
                CYBLE_BAS_BATTERY_LEVEL, sizeof(batteryLevel), &batteryLevel);
        }
        else
        {
            /* Update Battery Level characteristic value */
            apiResult = CyBle_BassSetCharacteristicValue(CYBLE_BATTERY_SERVICE_MEASURE_SERVICE_INDEX, 
                CYBLE_BAS_BATTERY_LEVEL, sizeof(batteryLevel), &batteryLevel);
        }
            
        if(apiResult != CYBLE_ERROR_OK)
        {
            batteryMeasureNotify = DISABLED;
        }
    }
}


/* [] END OF FILE */
