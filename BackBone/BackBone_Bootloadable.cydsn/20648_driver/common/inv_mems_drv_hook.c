//#include <Windows.h>
//#include <conio.h>

#include <project.h>
#include <stdio.h>
#include <string.h>
#include <i2c_comm.h>

#include ".\20648_driver\drivers\inv_mems_hw_config.h"
#include ".\20648_driver\invn\invn_types.h"

#include "i2c.h"
//#include "board-st_discovery.h"


int inv_serial_interface_write_hook(uint16_t reg, uint32_t length, uint8_t *data)
{
    int result;

	/* Replace with I2C write for PSoC4BLE */
    result = Sensors_I2C_WriteRegister(IVORY_I2C_ADDRESS, (unsigned char)reg, (unsigned short)length, data);

    return result;
}

int inv_serial_interface_read_hook(uint16_t reg, uint32_t length, uint8_t *data)
{
	int status;
    
	/* Replace with I2C read for PSoC4BLE */
	status= Sensors_I2C_ReadRegister(IVORY_I2C_ADDRESS, (unsigned char)reg, (unsigned short)length, data);

	return status;
}

/**
 *  @brief  Sleep function.
**/
void inv_sleep(unsigned long mSecs)
{
	CyDelay(mSecs);
}

void inv_sleep_100us(unsigned long nHowMany100MicroSecondsToSleep)
{
    (void)nHowMany100MicroSecondsToSleep;
    CyDelay(1);
}

/**
 *  @brief  get system's internal tick count.
 *          Used for time reference.
 *  @return current tick count.
**/
//long inv_get_tick_count(void)
//{
//    unsigned long count;
//
//    get_tick_count(&count);
//
//    return (long)count;
//}

