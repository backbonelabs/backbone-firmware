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

uint16 batterySimulationNotify = 0u;
uint16 batteryMeasureNotify = 0u;


#define VDD                 (3100u)
#define ADC_MAX_COUNTS      (2048u)

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

    switch (event)
    {
        case CYBLE_EVT_BASS_NOTIFICATION_ENABLED:
            if (CYBLE_BATTERY_SERVICE_MEASURE_SERVICE_INDEX == locServiceIndex)
            {
                batteryMeasureNotify = ENABLED;
            }
            break;

        case CYBLE_EVT_BASS_NOTIFICATION_DISABLED:
            if (CYBLE_BATTERY_SERVICE_MEASURE_SERVICE_INDEX == locServiceIndex)
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

#define CURVE_SIZE (4)

static struct
{
    int32 mvolts;
    int32 capacity;
} curve[CURVE_SIZE] =
{
    {3227,   0},
    {3608,   5},
    {4017,  86},
    {4150, 100},
};

/*******************************************************************************
* Function Name: MeasureBattery()
********************************************************************************
*
* Summary:
*   This function measures the battery voltage and send it to the client.
*
*******************************************************************************/
int32 MeasureBattery(bool immediate)
{
    int16 adcResult;
    int32 mvolts = 0;
    uint8 batteryLevel;
    CYBLE_API_RESULT_T apiResult;

    static uint32 batteryTimer = BATTERY_TIMEOUT;
    if (--batteryTimer == 0u || immediate)
    {
        batteryTimer = BATTERY_TIMEOUT;

        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);

        adcResult = ADC_GetResult16(ADC_BATTERY_CHANNEL);

        /* Calculate input voltage by using ratio of ADC counts from reference
         * and ADC Full Scale counts. */
        mvolts = (adcResult * VDD * 2) / ADC_MAX_COUNTS;

        if (mvolts <= curve[0].mvolts)
        {
            batteryLevel = curve[0].capacity;
        }
        else if (mvolts >= curve[CURVE_SIZE-1].mvolts)
        {
            batteryLevel = curve[CURVE_SIZE-1].capacity;
        }
        else
        {
            int i;
            for (i = 1; i < CURVE_SIZE; i++)
            {
                if (mvolts < curve[i].mvolts)
                {
                    /* Linear interpolation for this section of the curve */
                    batteryLevel = curve[i-1].capacity +
                                   ((curve[i].capacity - curve[i-1].capacity) * (mvolts - curve[i-1].mvolts)) /
                                   (curve[i].mvolts - curve[i-1].mvolts);
                    break;
                }
            }
        }

        if (batteryMeasureNotify == ENABLED)
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

        if (apiResult != CYBLE_ERROR_OK)
        {
            batteryMeasureNotify = DISABLED;
        }
    }

    return mvolts;
}
