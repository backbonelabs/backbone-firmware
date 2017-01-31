/*
* ________________________________________________________________________________________________________
* Copyright � 2014-2015 InvenSense Inc. Portions Copyright � 2014-2015 Movea. All rights reserved.
* This software, related documentation and any modifications thereto (collectively �Software�) is subject
* to InvenSense and its licensors' intellectual property rights under U.S. and international copyright and
* other intellectual property rights laws.
* InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
* and any use, reproduction, disclosure or distribution of the Software without an express license
* agreement from InvenSense is strictly prohibited.
* ________________________________________________________________________________________________________
*/

#ifndef _INV_IVORY_EXPORT_HEADERS_H__
#define _INV_IVORY_EXPORT_HEADERS_H__

// common headers
#include "invn/mlmath.h"
// driver headers
#include "drivers/inv_mems_hw_config.h"
#include "drivers/inv_mems_data_converter.h"
#include "drivers/inv_mems_mpu_fifo_control.h"
#include "drivers/inv_mems_defines.h"
#include "drivers/inv_mems_transport.h"
#include "drivers/inv_mems_slave_compass.h"
#include "drivers/inv_mems_secondary_transport.h"
#ifndef MEMS_20609
    #include "dmp3a/dmp3Default_20648.h"
    #include "drivers/inv_mems_base_control.h"
    #include "drivers/inv_mems_base_driver.h"
#else
    #include "dmp3/dmp3Default_20608D.h"
    #include "drivers/inv_mems_base_control_20609.h"
    #include "drivers/inv_mems_base_driver_20609.h"
#endif
#if defined MEMS_AUGMENTED_SENSORS
    #include "drivers/inv_mems_augmented_sensors.h"
#endif
#if (MEMS_CHIP == HW_ICM20648 || MEMS_CHIP == HW_ICM20609)
    #include "drivers/inv_mems_mpu_selftest.h"
#endif
// dmp3 headers
#include "dmp3/inv_mems_interface_mapping.h"

#endif // _INV_IVORY_EXPORT_HEADERS_H__
