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

#include "common.h"
#include "bas.h"
#include "inv.h"
#include <timeout.h>
#include <main.h>
#include <BleApplications.h>
#include <OTAMandatory.h>
#include "watchdog.h"
#include "posture.h"
#include "backbone.h"

extern void InitializeBootloaderSRAM();

__inline void ManageSystemPower()
{
    CYBLE_BLESS_STATE_T blePower;
    uint8 interruptStatus ;
    
    interruptStatus = CyEnterCriticalSection();
    
    blePower = CyBle_GetBleSsState();
    
    /* System can enter DeepSleep only when BLESS and rest of the 
     * application are in DeepSleep power modes */
    if(blePower == CYBLE_BLESS_STATE_DEEPSLEEP || 
       blePower == CYBLE_BLESS_STATE_ECO_ON)
    {
        CySysPmDeepSleep(); 
    }
    else if(blePower != CYBLE_BLESS_STATE_EVENT_CLOSE)
    {
        /* change HF clock source from IMO to ECO, as IMO is not required 
         * and can be stopped to save power */
        CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_ECO); 
        /* stop IMO for reducing power consumption */
        CySysClkImoStop();              
        /* put the CPU to sleep */
        CySysPmSleep();            
        /* starts execution after waking up, start IMO */
        CySysClkImoStart();
        /* change HF clock source back to IMO */
        CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_IMO);
    }
    
    CyExitCriticalSection(interruptStatus );
}

__inline void ManageApplicationPower()
{
    // put any application components to sleep.
}

float y[] = {1.0, 1.0, 1.0, 0.9, 0.9, 0.9, 0.8, 0.8, 0.8, 0.9};
float z[] = {0.0, 0.0, 0.0, 0.1, 0.1, 0.1, 0.2, 0.2, 0.2, 0.1};
int aidx;

__inline void RunApplication()
{
    // If there is data to read from the accelerometer, read it
    // then go to deep sleep.
    if (hal.new_gyro == 1)
	{
		hal.new_gyro = 0;
        //CyDelay(10);
        // TODO: Add this back when accelerometer is working
		//fifo_handler();

        // Remove this when accelerometer is working
        LED_Green_Write(0); // on
        CyDelay(50);
        LED_Green_Write(1); // off
        posture_update(y[aidx], z[aidx]);
        aidx += 1;
        if (aidx >= 10) aidx = 0;
        
        if (ble_is_connected())
        {
            backbone_accelerometer_t accelerometer_data;
            backbone_distance_t distance_data;
            
            accelerometer_data.axis[0] = 0;
            accelerometer_data.axis[1] = y[aidx];
            accelerometer_data.axis[2] = z[aidx];
            accelerometer_data.axis[3] = 0;            
            backbone_set_accelerometer_data(ble_get_connection(), &accelerometer_data);

            distance_data.distance = posture_get_distance();
            backbone_set_distance_data(ble_get_connection(), &distance_data);
            
            backbone_notify_accelerometer(ble_get_connection());
            backbone_notify_distance(ble_get_connection());
            backbone_notify_session_statistics(ble_get_connection());
            
            
            
    		ble_update_connection_parameters();
           	MeasureBattery(); 
        }
	}
}

__inline void ManageBlePower()
{
    CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
}

void RunBle()
{ 
    CyBle_ProcessEvents(); 

#if 0
   /* Wait until BLESS is in ECO_STABLE state to push the notification data to the BLESS */
   if(CyBle_GetBleSsState() == CYBLE_BLESS_STATE_ECO_STABLE)
   {
       // Send notifications.
   }
#endif
}

/**
 * Main function of BackBone firmware.
 *
 * Implements the application main loop, procsses BLE events, handles high level
 * application logic.  Puts the system into low power modes when there is no
 * work to do.
 *
 * Architecture of this is based on App Note: AN92584 "Designing for Low Power
 * and Estimating Battery Life for BLE Applications" 
 * http://www.cypress.com/documentation/application-notes/an92584-designing-low-power-and-estimating-battery-life-ble
 */
int main()
{
    aidx = 0;
    watchdog_init();
    
    CyGlobalIntEnable;  
    #if !defined(__ARMCC_VERSION)
        InitializeBootloaderSRAM();
    #endif
    
    AfterImageUpdate();

    /* Internal low power oscillator is stopped as it is not used in this 
     * project */
    CySysClkIloStop();
    
    /* Set the divider for ECO, ECO will be used as source when IMO is switched 
     * off to save power */
    CySysClkWriteEcoDiv(CY_SYS_CLK_ECO_DIV8);

    backbone_init();
    ble_init();
    CyBle_Start(ble_app_event_handler);
    CyBle_BasRegisterAttrCallback(BasCallBack);       
    while (CyBle_GetState() == CYBLE_STATE_INITIALIZING)
    {
        CyBle_ProcessEvents(); 
    }
   
	//inv_start();    
	//ADC_Start();
	//MotorPWM_Start();

    while(1)
    {
        RunBle();
        ManageBlePower();
        
        RunApplication();
        ManageApplicationPower();
        
        ManageSystemPower();
    }
}

/* [] END OF FILE */
