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
* Function Name: AppCallBack()
********************************************************************************
*
* Summary:
*   This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  event - event code
*  eventParam - event parameters
*
*******************************************************************************/
//void AppCallBack(uint32 event, void* eventParam)
//{
//    CYBLE_API_RESULT_T apiResult;
//    CYBLE_GAP_BD_ADDR_T localAddr;
//    uint8 i;
//
//	/* Structure to store data written by Client */	
//	CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
//    
//    #if (DEBUG_UART_ENABLED == YES)
//        CYBLE_GAP_AUTH_INFO_T *authInfo;
//    #endif /* (DEBUG_UART_ENABLED == YES) */
//    
//    switch (event)
//    {
//        /**********************************************************
//        *                       General Events
//        ***********************************************************/
//        /* This event is received when the component is Started */
//        case CYBLE_EVT_STACK_ON: 
//            /* Enter into discoverable mode so that remote can search it. */
//            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
//            if(apiResult != CYBLE_ERROR_OK)
//            {
//                DBG_PRINTF("StartAdvertisement API Error: %d \r\n", apiResult);
//            }
//            DBG_PRINTF("Bluetooth On, StartAdvertisement with addr: ");
//            localAddr.type = 0u;
//            CyBle_GetDeviceAddress(&localAddr);
//            for(i = CYBLE_GAP_BD_ADDR_SIZE; i > 0u; i--)
//            {
//                DBG_PRINTF("%2.2x", localAddr.bdAddr[i-1]);
//            }
//            DBG_PRINTF("\r\n");
//            break;
//        case CYBLE_EVT_TIMEOUT: 
//            break;
//        /* This event indicates that some internal HW error has occurred. */
//        case CYBLE_EVT_HARDWARE_ERROR:
//            DBG_PRINTF("CYBLE_EVT_HARDWARE_ERROR \r\n");
//            break;
//            
//        /* This event will be triggered by host stack if BLE stack is busy or not busy.
//         *  Parameter corresponding to this event will be the state of BLE stack.
//         *  BLE stack busy = CYBLE_STACK_STATE_BUSY,
//         *  BLE stack not busy = CYBLE_STACK_STATE_FREE 
//         */
//        case CYBLE_EVT_STACK_BUSY_STATUS:
//            DBG_PRINTF("EVT_STACK_BUSY_STATUS: %x\r\n", *(uint8 *)eventParam);
//            break;
//        case CYBLE_EVT_HCI_STATUS:
//            DBG_PRINTF("EVT_HCI_STATUS: %x \r\n", *(uint8 *)eventParam);
//            break;
//            
//        /**********************************************************
//        *                       GAP Events
//        ***********************************************************/
//        case CYBLE_EVT_GAP_AUTH_REQ:
//            DBG_PRINTF("EVT_AUTH_REQ: security=%x, bonding=%x, ekeySize=%x, err=%x \r\n", 
//                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).security, 
//                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).bonding, 
//                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).ekeySize, 
//                (*(CYBLE_GAP_AUTH_INFO_T *)eventParam).authErr);
//            break;
//        case CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST:
//            DBG_PRINTF("EVT_PASSKEY_ENTRY_REQUEST press 'p' to enter passkey \r\n");
//            break;
//        case CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST:
//            DBG_PRINTF("EVT_PASSKEY_DISPLAY_REQUEST %6.6ld \r\n", *(uint32 *)eventParam);
//            break;
//        case CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT:
//            DBG_PRINTF("EVT_GAP_KEYINFO_EXCHNGE_CMPLT \r\n");
//            break;
//        case CYBLE_EVT_GAP_AUTH_COMPLETE:
//            #if (DEBUG_UART_ENABLED == YES)
//            authInfo = (CYBLE_GAP_AUTH_INFO_T *)eventParam;
//            DBG_PRINTF("AUTH_COMPLETE: security:%x, bonding:%x, ekeySize:%x, authErr %x \r\n", 
//                                    authInfo->security, authInfo->bonding, authInfo->ekeySize, authInfo->authErr);
//            #endif /* (DEBUG_UART_ENABLED == YES) */
//            break;
//        case CYBLE_EVT_GAP_AUTH_FAILED:
//            DBG_PRINTF("EVT_AUTH_FAILED: %x \r\n", *(uint8 *)eventParam);
//            break;
//        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
//            DBG_PRINTF("EVT_ADVERTISING, state: %x \r\n", CyBle_GetState());
//            if(CYBLE_STATE_DISCONNECTED == CyBle_GetState())
//            {   
//                /* Fast and slow advertising period complete, go to low power  
//                 * mode (Hibernate mode) and wait for an external
//                 * user event to wake up the device again */
//                DBG_PRINTF("Hibernate \r\n");
//            }
//            break;
//        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
//            DBG_PRINTF("EVT_GAP_DEVICE_CONNECTED \r\n");
//            break;
//        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
//            DBG_PRINTF("EVT_GAP_DEVICE_DISCONNECTED\r\n");
//            apiResult = CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
//            if(apiResult != CYBLE_ERROR_OK)
//            {
//                DBG_PRINTF("StartAdvertisement API Error: %d \r\n", apiResult);
//            }
//            break;
//        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
//            { 
//                uint16 mtu;
//                CyBle_GattGetMtuSize(&mtu);
//                DBG_PRINTF("CYBLE_EVT_GATTS_XCNHG_MTU_REQ, final mtu= %d \r\n", mtu);
//            }
//            break;
//        case CYBLE_EVT_GATTS_WRITE_REQ:
//			{
//				wrReqParam = eventParam;
//				if(CYBLE_BACKBONE_WRITE_DATA_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
//				{
//					BLEWriteData[0] = wrReqParam->handleValPair.value.val[0];
//					BLEWriteData[1] = wrReqParam->handleValPair.value.val[1];
//					BLEWriteData[2] = wrReqParam->handleValPair.value.val[2];
//					BLEWriteData[3] = wrReqParam->handleValPair.value.val[3];
//					
//					MotorPWMDutyCycle = BLEWriteData[1];
//					
//					if(BLEWriteData[0] != 0)
//					{
//						MotorFlag = 1;
//						MotorPWM_Start();
//						
//						MotorPWM_WriteCompare(MotorPWMDutyCycle);
//					}
//					else
//					{
//						MotorFlag = 0;
//						MotorPWM_Stop();
//					}
//				}
//				/* Send the response to the write request. */
//				CyBle_GattsWriteRsp(cyBle_connHandle);
//				break;
//			}
//            break;
//        case CYBLE_EVT_GAP_ENCRYPT_CHANGE:
//            DBG_PRINTF("EVT_GAP_ENCRYPT_CHANGE: %x \r\n", *(uint8 *)eventParam);
//            break;
//        case CYBLE_EVT_GAPC_CONNECTION_UPDATE_COMPLETE:
//            DBG_PRINTF("EVT_CONNECTION_UPDATE_COMPLETE: %x \r\n", *(uint8 *)eventParam);
//            break;
//            
//        /**********************************************************
//        *                       GATT Events
//        ***********************************************************/
//        case CYBLE_EVT_GATT_CONNECT_IND:
//            DBG_PRINTF("EVT_GATT_CONNECT_IND: %x, %x \r\n", cyBle_connHandle.attId, cyBle_connHandle.bdHandle);
//            break;
//        case CYBLE_EVT_GATT_DISCONNECT_IND:
//            DBG_PRINTF("EVT_GATT_DISCONNECT_IND \r\n");
//            break;
//            
//        /**********************************************************
//        *                       Other Events
//        ***********************************************************/
//        default:
//            DBG_PRINTF("OTHER event: %lx \r\n", event);
//            break;
//    }
//
//}

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
    InitializeBootloaderSRAM();
    
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
