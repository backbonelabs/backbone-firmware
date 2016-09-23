/*******************************************************************************
* File Name: I2CComm.h
*
* Version: 1.0
*
* Description:
* 1. This is header file of I2CComm.c;
*
* Owner:
* GRAA
*
********************************************************************************
* Copyright (2011) , Cypress Semiconductor Corporation.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
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
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/

#ifndef __I2CCOMM_H
#define __I2CCOMM_H

#include <project.h>	
	
extern uint8 CommErrorFlags;
extern uint8 I2CTimeout;
extern uint8 BrixCommErrorCount;
extern uint8 KeypadCommErrorCount;
extern uint8 CyFiCommErrorCount;

/* Flags in the CommErrorFlags variable */
#define COMM_ERR_CYFI_FATAL		0x01	// CYFI Fatal Error flag
#define COMM_ERR_KEYPAD_FATAL	0x02	// Keypad Fatal Error flag
#define COMM_ERR_BRIX_FATAL		0x04	// Brix Fatal Error flag
#define COMM_ERR_CYFI			0x10	// CYFI error flag
#define COMM_ERR_KEYPAD			0x20	// Keypad error flag
#define COMM_ERR_BRIX			0x40	// Brix error flag
	
#define COMM_SUCCESS		(0x0000)
#define COMM_ERROR			(0xFFFF)

/* Slave Addresses of I2C peripherals */
#define BRIX_SLAVE_ADDR 	0x01	// Brix card
#define CYFI_SLAVE_ADDR		0x00	// CYFI card
#define KEYPAD_SLAVE_ADDR 	0x04	// Front panel board

/* Timeout for read and write operations for the peripherals */
#define BRIX_READ_TIMEOUT		20
#define BRIX_WRITE_TIMEOUT		20
#define CYFI_READ_TIMEOUT		20
#define CYFI_WRITE_TIMEOUT		20
#define KEYPAD_READ_TIMEOUT		10
#define KEYPAD_WRITE_TIMEOUT	10

/* Number of tries before the comm error flag for a peripheral is set */
#define COMM_ERROR_RETRY_COUNT	5

/* This constant defines the length of the address byte used in write
   I2C write operation to set the sub-address of the write operation
   on the EzI2C interface's register map.  The CYFI and Brix data structures
   use the first byte as the sub-address for EzI2C write operations */
#define EZI2C_OFFSET_ADDR_LEN	1	


	

/* Function prototypes */
void I2CComm_SetI2CTimeout(uint16 Timeout);
void I2CComm_Timeout_Start(uint16 Timeout);
void I2CComm_Timeout_Stop(void);
int Sensors_I2C_WriteRegister(unsigned char slave_addr, unsigned char reg_addr, unsigned short len, unsigned char *data_ptr);
int Sensors_I2C_ReadRegister(unsigned char slave_addr, unsigned char reg_addr, unsigned short len, unsigned char *data_ptr);

#endif   /* __I2CCOMM_H */
//[] END OF FILE
