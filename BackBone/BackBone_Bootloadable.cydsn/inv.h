/* ===========================================
 *
 * Copyright BACKBONE LABS INC, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION,
 * WHICH IS THE PROPERTY OF BACKBONE LABS INC.
 *
 * ===========================================
 */

#ifndef INV_H_
#define INV_H_

#include <stdbool.h>
#include ".\20648_driver\invn\inv_mems.h"

struct hal_s_
{
    long report;          // What to report?
    unsigned char  debug_reg_on;     // with '\' as a command this turns ON
    int expected_data;
    volatile unsigned char new_gyro;
};

/*******************************************************************************/
#define PRINT_ACCEL         (0x01)
#define PRINT_GYRO          (0x02)
#define PRINT_RAW_GYRO      (0x04)
#define PRINT_COMPASS       (0x08)
#define PRINT_RAW_COMPASS   (0x10)
#define PRINT_RV            (0x20)
#define PRINT_GRV           (0x40)
#define PRINT_ORIENT        (0x80)
#define PRINT_LINEAR_ACCEL  (0x100)
#define PRINT_GRAVITY       (0x200)
#define PRINT_STEP_COUNTER  (0x400)
#define PRINT_STEP_DETECTOR (0x800)
#define PRINT_SMD           (0x1000)
#define PRINT_GEOMAG        (0x2000)
#define PRINT_PRESSURE      (0x8000)
#define PRINT_CUBE_GRV      (0x10000)
#define PRINT_CUBE_RV       (0x20000)
#define PRINT_CUBE_GEOMAG   (0x40000)
#define PRINT_LPQ           (0x80000)
#define PRINT_BAC           (0x100000)
#define PRINT_FLIP_PICKUP   (0x200000)
#define PRINT_TILT          (0x400000)
#define PRINT_PROXIMITY     (0x800000)
#define PRINT_HRM           (0x1000000)
#define PRINT_SHAKE         (0x2000000)
#define PRINT_B2S           (0x4000000)

#define PRINT_GES_GROUP (PRINT_STEP_COUNTER | \
                            PRINT_STEP_DETECTOR | \
                            PRINT_SMD | \
                            PRINT_BAC | \
                            PRINT_FLIP_PICKUP | \
                            PRINT_TILT)

#ifdef MEMS_20609
    #define DMP_INT_SMD     0x0400
    #define DMP_INT_PED     0x0800
#endif

#ifdef _WIN32
    #define INV_SPRINTF(str, len, ...) sprintf_s(str, len, __VA_ARGS__)
#else
    #define INV_SPRINTF(str, len, ...) sprintf(str, __VA_ARGS__)
#endif

/** @brief Set of flags for BAC state */
#define BAC_DRIVE   0x01
#define BAC_WALK    0x02
#define BAC_RUN     0x04
#define BAC_BIKE    0x08
#define BAC_TILT    0x10
#define BAC_STILL   0x20

#define INV_TST_LEN 256

#define MAX_BUF_LENGTH  (18)

enum packet_type_e
{
    PACKET_TYPE_ACCEL,
    PACKET_TYPE_GYRO,
    PACKET_TYPE_QUAT,
    PACKET_TYPE_COMPASS = 7
};

/* Status bits in InvFlags */
#define INV_ERROR_INIT          (0x0001)
#define INV_ERROR_SELF_TEST     (0x0002)


extern struct hal_s_ hal;

inv_error_t inv_start(void);
inv_error_t inv_rerun_selftest(void);
uint32_t inv_get_init_status(void);
uint32_t inv_get_selftest_status(void);
void inv_enable_accelerometer(void);
void inv_disable_accelerometer(void);
bool inv_is_accelerometer_enabled(void);
float inv_get_accelerometer_x(void);
float inv_get_accelerometer_y(void);
float inv_get_accelerometer_z(void);
uint32_t inv_get_step_count(void);
void inv_reset_step_count(void);
void inv_set_step_count(uint32_t steps);
void fifo_handler(void);

#endif /* INV_H_ */
