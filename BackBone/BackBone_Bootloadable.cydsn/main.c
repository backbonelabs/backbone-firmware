/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  BLE example project that measures the battery voltage using PSoC 4 BLE's 
*  internal ADC and notifies the BLE central device on any change in battery 
*  voltage.
*
* Note:
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



/*******************************************************************************
* Function Name: LowPowerImplementation()
********************************************************************************
* Summary:
* Implements low power in the project.
*
* Parameters:
* None
*
* Return:
* None
*
* Theory:
* The function tries to enter deep sleep as much as possible - whenever the 
* BLE is idle and the UART transmission/reception is not happening. At all other
* times, the function tries to enter CPU sleep.
*
*******************************************************************************/
static void LowPowerImplementation(void)
{
    CYBLE_LP_MODE_T bleMode;
    uint8 interruptStatus;
    
    /* For advertising and connected states, implement deep sleep 
     * functionality to achieve low power in the system. For more details
     * on the low power implementation, refer to the Low Power Application 
     * Note.
     */
    if((CyBle_GetState() == CYBLE_STATE_ADVERTISING) || 
       (CyBle_GetState() == CYBLE_STATE_CONNECTED))
    {
        /* Request BLE subsystem to enter into Deep-Sleep mode between connection and advertising intervals */
        bleMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
        
		/* Disable global interrupts */
        interruptStatus = CyEnterCriticalSection();
        
		/* When BLE subsystem has been put into Deep-Sleep mode */
        if((bleMode == CYBLE_BLESS_DEEPSLEEP) && (MotorFlag == 0) &&  (hal.new_gyro == 0))
        {
            /* And it is still there or ECO is on */
            if((CyBle_GetBleSsState() == CYBLE_BLESS_STATE_ECO_ON) || 
               (CyBle_GetBleSsState() == CYBLE_BLESS_STATE_DEEPSLEEP))
            {
                CySysPmDeepSleep();
            }
        }
        else /* When BLE subsystem has been put into Sleep mode or is active */
        {
            /* And hardware doesn't finish Tx/Rx opeation - put the CPU into Sleep mode */
            if(((CyBle_GetBleSsState() != CYBLE_BLESS_STATE_EVENT_CLOSE) || (MotorFlag != 0)) &&  (hal.new_gyro == 0))
            {
                CySysPmSleep();
            }
        }
        
		/* Enable global interrupt */
        CyExitCriticalSection(interruptStatus);
    }
}



/*******************************************************************************
* Function Name: main()
********************************************************************************
* Summary:
*  Main function for the project.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Theory:
*  The function starts BLE and UART components.
*  This function process all BLE events and also implements the low power 
*  functionality.
*
*******************************************************************************/
int main()
{	
    CyGlobalIntEnable;  

    #if !defined(__ARMCC_VERSION)
        InitializeBootloaderSRAM();
    #endif

    /* Checks if Self Project Image is updated and Runs for the First time */
    AfterImageUpdate();

	inv_start();
	
    CyBle_Start(CustomEventHandler);
    CyBle_BasRegisterAttrCallback(BasCallBack);
    
	ADC_Start();
	MotorPWM_Start();

    while(1) 
    {              
        if (hal.new_gyro == 1)
		{
			hal.new_gyro = 0;
            CyDelay(10);
			fifo_handler();
		}
		
		BackBone_Task();
		
		/* Process all the generated events. */
        CyBle_ProcessEvents();                

        #ifdef ENABLE_LOW_POWER_MODE
			/* To achieve low power in the device */
            if(restartAdvertisement==FALSE)
            {
	            LowPowerImplementation();
            }
		#endif

        if(restartAdvertisement)
		{
			restartAdvertisement = FALSE;			
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);            
		}
        
        if(TRUE == deviceConnected)
		{
			UpdateConnectionParam();	

           	MeasureBattery(); 
		}
		
	}   
}  


/* [] END OF FILE */
