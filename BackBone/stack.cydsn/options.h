/*******************************************************************************
* File Name: Options.h
*
* Version: 1.10
*
* Description:
*  Provides project configuration options.
*
* Hardware Dependency:
*  CY8CKIT-042 BLE
*
********************************************************************************
* Copyright 2014-2015, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#if !defined(options_H)
#define options_H

#define NO                      (0u)
#define YES                     (1u)


/*******************************************************************************
* Following section contains bootloadable project compile-time options.
*******************************************************************************/
#define DEBUG_UART_ENABLED      (NO)
/* YES - use printf, NO - use UART_PutString
 * e.g. DBG_PRINTF("message %d\r\n", i) will transform to UART_PutString("message %d\r\n") */
#define DEBUG_UART_USE_PRINTF_FORMAT  (NO)
#define PRINT_BOUNDING_DATA     (NO)
#define ENABLE_ENCRYPTION

#endif /* options_H */


/* [] END OF FILE */
