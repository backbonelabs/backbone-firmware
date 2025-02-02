/*******************************************************************************
* File Name: debug.h
*
* Version: 1.20
*
* Description:
*  Provides debug API.
*
********************************************************************************
* Copyright 2014-2016, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>
#include "cytypes.h"
#include "project.h"
#include "options.h"

#if (DEBUG_UART_ENABLED == YES)
    #define DBG_PRINT_TEXT(a)       do { UART_PutString(a); } while (0)
    #define DBG_NEW_PAGE()          UART_PutChar(0x0C)

    #if (DEBUG_UART_USE_PRINTF_FORMAT == YES)
        #define DBG_PRINT_DEC(a)    do { printf("%u", (unsigned int)(a)); } while (0)
        #define DBG_PRINT_HEX(a)    do { printf("%X", (unsigned int)(a)); } while (0)
        #define DBG_PRINT_HEX_BYTE(a)   do { printf("%2.2x", (unsigned int)(a)); } while (0)
        #define DBG_PRINTF(...)     (printf(__VA_ARGS__))
    #else
        void DBG_PRINT_DEC(uint32 a);
        void DBG_PRINT_HEX(uint32 a);
        #define DBG_PRINT_HEX_BYTE(a) ( UART_PutHexByte(a) )
    #endif /* (DEBUG_UART_USE_PRINTF_FORMAT == YES) */

    #define DBG_PRINT_ARRAY(a, b) UART_PutArray(a, b)
    void DBG_PRINT_HEX_TEXT(uint32 hex, char *text);
    void DBG_PRINT_HEX8_TEXT(uint32 hex, char *text);
    void DBG_PRINT_DEC_TEXT(uint32 dec, char *text);

#else
    #define DBG_PRINT_TEXT(a)       do { (void)0; } while (0)
    #define DBG_PRINT_DEC(a)        do { (void)0; } while (0)
    #define DBG_PRINT_HEX(a)        do { (void)0; } while (0)
    #define DBG_PRINT_HEX_BYTE(a)   do { (void)0; } while (0)
    #define DBG_PRINT_ARRAY(a,b)    do { (void)0; } while (0)
    #define DBG_PRINTF(...)         do { (void)0; } while (0)
    #define DBG_NEW_PAGE()          do { (void)0; } while (0)
#endif /* (DEBUG_UART_ENABLED == YES) */

#endif /* DEBUG_H_ */
