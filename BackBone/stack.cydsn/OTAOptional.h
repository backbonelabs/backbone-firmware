/*******************************************************************************
* File Name: OTAOptional.h
* Version 1.10
*
* Description:
*  Contains the function prototypes and constants available to the example
*  project. They are optional for OTA functionality.
*
********************************************************************************
* Copyright 2014-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#include <project.h>

#define APP_FLAG_OFFSET                 (20u)
#define STACK_FLAG_OFFSET               (10u)
#define LENGHT_OF_UART_ROW              (20u)

#define STACK_MD_BASE_ADDR              (CYDEV_FLASH_BASE + \
                                            (CYDEV_FLASH_SIZE - ((uint32)CYDEV_FLS_ROW_SIZE)))
                                                        
#define UPDATE_FLAG_OFFSET              (STACK_MD_BASE_ADDR + STACK_FLAG_OFFSET)
                                                        
#define APP_PRJ_MD_BASE_ADDR            (CYDEV_FLASH_BASE + \
                                            (CYDEV_FLASH_SIZE - ((uint32)CYDEV_FLS_ROW_SIZE * 2u)))

#define APP_UPDATE_FLAG_OFFSET          (APP_PRJ_MD_BASE_ADDR + APP_FLAG_OFFSET)

#define WARNING_TIMEOUT                 (0xA00)
#define SWITCHING_TIMEOUT               (0xF00)

void AfterImageUpdate(void);
#if ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES))
    cystatus Clear_ROM_Array(const uint8 eepromPtr[], uint32 byteCount);
#endif /* ((CYBLE_GAP_ROLE_PERIPHERAL || CYBLE_GAP_ROLE_CENTRAL) && (CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)) */

void TimeoutImplementation(void);

/* [] END OF FILE */
