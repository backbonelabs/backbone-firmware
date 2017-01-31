//#include <Windows.h>
//#include <conio.h>

#include <stdio.h>
#include <string.h>

#include "drivers/inv_mems_hw_config.h"
#include "invn/invn_types.h"

#include "i2c.h"
#include "i2c_comm.h"


int inv_serial_interface_write_hook(uint16_t reg, uint32_t length, uint8_t *data)
{
    int result;

    result = Sensors_I2C_WriteRegister(ACCEL_GYRO_CHIP_ADDR, (unsigned char)reg, (unsigned short)length, data);

    return result;
}

int inv_serial_interface_read_hook(uint16_t reg, uint32_t length, uint8_t *data)
{
    int status;

    status= Sensors_I2C_ReadRegister(ACCEL_GYRO_CHIP_ADDR, (unsigned char)reg, (unsigned short)length, data);

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

