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

extern void InitializeBootloaderSRAM();

uint8 BLEWriteData[4];
uint8 BLEReadData[4];
uint8 MotorFlag = 0;
uint16 MotorPWMDutyCycle = 0;

uint8 VersionString[4] = {HW_MAJOR_VERSION, HW_MINOR_VERSION, FW_MAJOR_VERSION, FW_MINOR_VERSION};

extern uint8 deviceConnected;
extern uint8 restartAdvertisement;

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

__inline void RunApplication()
{
    // If there is data to read from the accelerometer, read it
    // then go to deep sleep.
    if (hal.new_gyro == 1)
	{
		hal.new_gyro = 0;
        CyDelay(10);
		fifo_handler();
        BackBone_Task();
	}

#if 0
    if(TRUE == deviceConnected)
	{
		UpdateConnectionParam();
       	MeasureBattery(); 
	}
#endif
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

    CyBle_Start(CustomEventHandler);
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
