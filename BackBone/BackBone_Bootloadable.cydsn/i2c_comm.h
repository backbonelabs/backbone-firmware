/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

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
