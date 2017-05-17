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

#include <project.h>
#include <inv.h>
#include <stdbool.h>
#include "invn\inv_mems.h"
#include "debug.h"

static uint16 inv_flags = 0;
static inv_error_t inv_init_status = 0;
static inv_error_t inv_selftest_status = 0;


struct hal_s_ hal = { 0, 0, 0, 0 };

int self_test_result = 0;
int dmp_bias[9] = { 0 };

unsigned short accel_data_was_set = 0;
unsigned short gyro_data_was_set = 0;
unsigned short raw_gyro_data_was_set = 0;
unsigned short compass_data_was_set = 0;
unsigned short raw_compass_data_was_set = 0;
unsigned short quat6_data_was_set = 0;
unsigned short quat9_data_was_set = 0;
unsigned short rv_quat_was_set = 0;
unsigned short gmrv_quat_was_set = 0;
unsigned short bac_data_was_set = 0;
unsigned short flip_data_was_set = 0;
unsigned short tilt_data_was_set = 0;


float grv_float[4];
signed long cube_grv[4] = { 0, 0, 0, 0 };
float accel_float[3];
float rv_float[4];
signed long cube_rv[4] = { 0, 0, 0, 0 };
float gyro_float[3];
float gyro_raw_float[3];
float gyro_bias_float[3];
long previous_bias[3];
float compass_float[3];
float compass_raw_float[3];
float compass_bias[3];
float gmrv_float[4];
signed long cube_gmrv[4] = { 0, 0, 0, 0 };
int accel_accuracy = 0;
int gyro_accuracy = 0;
int rv_accuracy = 0;
int compass_accuracy = 0;
int gmrv_accuracy = 0;
long long ts = 0;
signed long result_quat[4] = { 0, 0, 0, 0 };
int a_average[3] = { 0, 0, 0 };
int g_average[3] = { 0, 0, 0 };
unsigned long steps = 0;
uint16_t bac_state = 0;
uint8_t tilt_state = 0;
long bac_ts = 0;
uint16_t flip_pickup = 0;
uint8_t step_detected = 0;
float current_output_rate = 5;

signed char ACCEL_GYRO_ORIENTATION[] = {0,-1,0,1,0,0,0,0,1};

/* Change the COMPASS_SLAVE_ID to the correct ID of compass used. You can find the defines in inv_mems_hw_config.h*/
const unsigned char COMPASS_SLAVE_ID = HW_AK09912;

/* Change COMPASS_CHIP_ADDR to 0x0C for ICM20698 which uses internal AKM 9916 */
/* Change COMPASS_CHIP_ADDR to 0x0E for other AKM 9912/9911/9913/8963*/
const unsigned char COMPASS_CHIP_ADDR = 0x0E;
const unsigned char PRESSURE_CHIP_ADDR = 0x00;
const unsigned char ACCEL_GYRO_CHIP_ADDR = 0x69;

signed char COMPASS_ORIENTATION[] = {0,-1,0,1,0,0,0,0,1};

long SOFT_IRON_MATRIX[] = {1073741824,0,0,0,1073741824,0,0,0,1073741824};

char tst[INV_TST_LEN] = { 0 };

inv_error_t set_output_rates(float rate);
inv_error_t inv_perform_selftest(void);

CY_ISR(INVN_INT_InterruptHandler)
{
    hal.new_gyro = 1;
    INVN_INT_ClearInterrupt();
}

void inv_enable_accelerometer()
{
    if (inv_flags != 0)
    {
        return;
    }

    I2C_Start();
    dmp_reset_fifo();
    hal.report |= PRINT_ACCEL;
    inv_enable_sensor(ANDROID_SENSOR_ACCELEROMETER, true);
    dmp_reset_odr_counters();
    set_output_rates(5);

    I2C_Stop();

    isr_INVN_INT_StartEx(INVN_INT_InterruptHandler);
}

void inv_disable_accelerometer()
{
    if (inv_flags != 0)
    {
        return;
    }

    I2C_Start();
    dmp_reset_fifo();
    hal.report &= ~PRINT_ACCEL;
    inv_enable_sensor(ANDROID_SENSOR_ACCELEROMETER, false);
    dmp_reset_odr_counters();

    I2C_Stop();

    isr_INVN_INT_Stop();
}

inv_error_t inv_start(void)
{
    /* Start I2C Master */
    I2C_Start();

    /* Initialize the 20648 device */
    // inv_set_chip_to_body_axis_quaternion(ACCEL_GYRO_ORIENTATION, 0.0);
    inv_init_status = inv_initialize_lower_driver(SERIAL_INTERFACE_I2C, 0);
    if (inv_init_status == 0)
    {
        inv_flags &= ~INV_ERROR_INIT;
    }
    else
    {
        inv_flags |= INV_ERROR_INIT;
    }


    /* Perform 60248 self test and update bias values */
    inv_selftest_status = inv_perform_selftest();
    if (inv_selftest_status == 0)
    {
        inv_flags &= ~INV_ERROR_SELF_TEST;
    }
    else
    {
        inv_flags |= INV_ERROR_SELF_TEST;
    }

    I2C_Stop();
    return inv_selftest_status;
}

inv_error_t inv_rerun_selftest(void)
{
    /* Start I2C Master */
    I2C_Start();

    /* Perform 60248 self test and update bias values */
    inv_selftest_status = inv_perform_selftest();
    if (inv_selftest_status == 0)
    {
        inv_flags &= ~INV_ERROR_SELF_TEST;
    }
    else
    {
        inv_flags |= INV_ERROR_SELF_TEST;
    }

    I2C_Stop();

    return inv_selftest_status;
}

uint16 inv_get_status(void)
{
    return inv_flags;
}

uint32_t inv_get_init_status(void)
{
    return inv_init_status;
}

uint32_t inv_get_selftest_status(void)
{
    return inv_selftest_status;
}

inv_error_t set_output_rates(float rate)
{
    unsigned short base_rate = 1125;
    inv_error_t result=0;

    dmp_reset_fifo();

    if (hal.report & PRINT_ACCEL)
    {
        result |= inv_set_odr(ANDROID_SENSOR_ACCELEROMETER, (unsigned short)(base_rate / rate));
    }

    if (hal.report & PRINT_GYRO)
    {
        result |= inv_set_odr(ANDROID_SENSOR_GYROSCOPE, (unsigned short)(base_rate / rate));
    }

    if (hal.report & PRINT_RAW_GYRO)
    {
        result |= inv_set_odr(ANDROID_SENSOR_GYROSCOPE_UNCALIBRATED, (unsigned short)(base_rate / rate));
    }

//  if (hal.report & PRINT_GRV)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_GAME_ROTATION_VECTOR, (unsigned short)(base_rate / rate));
//  }
//
//  if (hal.report & PRINT_CUBE_GRV)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_GAME_ROTATION_VECTOR, (unsigned short)(base_rate / rate));
//  }
//
    if (hal.report & PRINT_STEP_DETECTOR)
    {
        result |= inv_set_odr(ANDROID_SENSOR_STEP_DETECTOR, (unsigned short)(base_rate / rate));
    }

//  if (hal.report & PRINT_SMD)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_WAKEUP_SIGNIFICANT_MOTION, (unsigned short)(base_rate / rate));
//  }
//
    if (hal.report & PRINT_STEP_COUNTER)
    {
        result |= inv_set_odr(ANDROID_SENSOR_STEP_COUNTER, (unsigned short)(base_rate / rate));
    }

//  if (hal.report & PRINT_LINEAR_ACCEL)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_LINEAR_ACCELERATION, (unsigned short)(base_rate / rate));
//  }
//
//  if (hal.report & PRINT_GRAVITY)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_GRAVITY, (unsigned short)(base_rate / rate));
//  }
//
//  if (hal.report & PRINT_COMPASS)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_GEOMAGNETIC_FIELD, (unsigned short)(base_rate / rate));
//  }
//
//  if (hal.report & PRINT_RAW_COMPASS)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_MAGNETIC_FIELD_UNCALIBRATED, (unsigned short)(base_rate / rate));
//  }
//
//  if (hal.report & PRINT_RV)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_ROTATION_VECTOR, (unsigned short)(base_rate / rate));
//  }
//
//  if (hal.report & PRINT_GEOMAG)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_GEOMAGNETIC_ROTATION_VECTOR, (unsigned short)(base_rate / rate));
//  }
//
//  if (hal.report & PRINT_BAC)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_ACTIVITY_CLASSIFICATON, (unsigned short)(base_rate / rate));
//  }
//
    if (hal.report & PRINT_TILT)
    {
        result |= inv_set_odr(ANDROID_SENSOR_WAKEUP_TILT_DETECTOR, (unsigned short)(base_rate / rate));
    }
//
//  if (hal.report & PRINT_FLIP_PICKUP)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_FLIP_PICKUP, (unsigned short)(base_rate / rate));
//  }
//
//  if (hal.report & PRINT_CUBE_RV)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_ROTATION_VECTOR, (unsigned short)(base_rate / rate));
//  }
//
//  if (hal.report & PRINT_CUBE_GEOMAG)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_GEOMAGNETIC_ROTATION_VECTOR, (unsigned short)(base_rate / rate));
//  }
//
//  if (hal.report & PRINT_ORIENT)
//  {
//      result |= inv_set_odr(ANDROID_SENSOR_ORIENTATION, (unsigned short)(base_rate / rate));
//  }

    current_output_rate = rate;

    dmp_reset_odr_counters();

    return result;
}

inv_error_t inv_perform_selftest(void)
{
    inv_error_t status;
    dmp_reset_fifo();
    int sensors_bias[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    int scale = 2000 / 250; //self-test uses 250dps FSR, main() set the FSR to 2000dps

    self_test_result = inv_mems_run_selftest();
    dmp_get_bias_20648(sensors_bias);

    dmp_bias[0] = a_average[0] * (1 << 11);   // Change from LSB to format expected by DMP
    dmp_bias[1] = a_average[1] * (1 << 11);
    dmp_bias[2] = (a_average[2] - 16384)*(1 << 11); //remove the gravity and scale (FSR=2 in selftest)
    dmp_bias[3] = (g_average[0] / scale)*(1 << 15);
    dmp_bias[4] = (g_average[1] / scale)*(1 << 15);
    dmp_bias[5] = (g_average[2] / scale)*(1 << 15);

    //Acc-Gyro self-test is passed already
    if ((self_test_result & 0x3) == 0x3)
    {
        //update the DMP biases
        dmp_set_bias_20648(dmp_bias);
        status = 0;
    }
    else
    {
        status = -1;
    }

    return status;
}

void process_sensor_output()
{

    signed long  long_quat[3] = { 0 };

    if (hal.report & PRINT_ACCEL)
    {
        if (accel_data_was_set == 1)
        {
            if (self_test_result && (accel_accuracy == 0)) //self-test is done already
                accel_accuracy = 1; //accuracy reaches intermediate level after one-axis factory cal--yd
            //INV_SPRINTF(tst, INV_TST_LEN, "Accel Data\t %8.5f, %8.5f, %8.5f, %d, %lld\r\n", accel_float[0], accel_float[1], accel_float[2], accel_accuracy, ts); print_command_console(tst);
            accel_data_was_set = 0;
        }
    }

    if (hal.report & PRINT_GYRO)
    {
        if (gyro_data_was_set == 1)
        {
            if (self_test_result && (gyro_accuracy == 0)) //self-test is done already
                gyro_accuracy = 1; //accuracy reaches intermediate level after one-axis factory cal--yd
            //INV_SPRINTF(tst, INV_TST_LEN, "Gyro Data\t %7.5f, %7.5f, %7.5f, %d, %lld\r\n", gyro_float[0], gyro_float[1], gyro_float[2], gyro_accuracy, ts); print_command_console(tst);
            gyro_data_was_set = 0;
        }
    }

//  if (hal.report & PRINT_RAW_GYRO) {
//#if ((MEMS_CHIP == HW_ICM30630) || (MEMS_CHIP == HW_ICM20648) || (MEMS_CHIP == HW_ICM20609))
//      if (raw_gyro_data_was_set == 1) {
//          //INV_SPRINTF(tst, INV_TST_LEN, "Raw Gyro Data\t %7.5f, %7.5f, %7.5f,%d, %lld\r\n", gyro_raw_float[0], gyro_raw_float[1], gyro_raw_float[2], 0, ts); print_command_console(tst);
//            //            INV_SPRINTF(tst, INV_TST_LEN, "Gyro Bias\t %7.5f, %7.5f, %7.5f\r\n", gyro_bias_float[0], gyro_bias_float[1], gyro_bias_float[2]); print_command_console(tst);
//          raw_gyro_data_was_set = 0;
//      }
//#endif
//  }
//
    if (quat6_data_was_set == 1)
    {
        if (hal.report & PRINT_GRV)
        {
            //INV_SPRINTF(tst, INV_TST_LEN, "Game RV\t %7.5f, %7.5f, %7.5f, %7.5f, %lld\r\n", grv_float[0], grv_float[1], grv_float[2], grv_float[3], ts);
            //print_data_console(tst);
        }
        if (hal.report & PRINT_CUBE_GRV)
        {
            dmp_get_6quaternion(long_quat);
            inv_compute_scalar_part(long_quat, result_quat);
            //send_data_packet(PACKET_TYPE_QUAT, (void *)result_quat);
        }
#if (MEMS_CHIP != HW_ICM20609)
#if defined MEMS_AUGMENTED_SENSORS
        if (hal.report & PRINT_GRAVITY)
        {
            INV_SPRINTF(tst, INV_TST_LEN, "Gravity\t %7.5f, %7.5f, %7.5f, %lld\r\n", gravityFloat[0], gravityFloat[1], gravityFloat[2], ts);
            print_data_console(tst);
        }
        if (hal.report & PRINT_LINEAR_ACCEL)
        {
            INV_SPRINTF(tst, INV_TST_LEN, "LinearAcc\t %7.5f, %7.5f, %7.5f, %lld\r\n", linAccFloat[0], linAccFloat[1], linAccFloat[2], ts);
            print_data_console(tst);
        }

#endif
#endif
        quat6_data_was_set = 0;
    }
#if (MEMS_CHIP != HW_ICM20609)
//        if (hal.report & PRINT_COMPASS) {
//      if (compass_data_was_set == 1) {
//          //INV_SPRINTF(tst, INV_TST_LEN, "Compass Data\t %7.5f, %7.5f, %7.5f,\t%d, %lld\r\n", compass_float[0], compass_float[1], compass_float[2], compass_accuracy, ts); print_command_console(tst);
//          compass_data_was_set = 0;
//      }
//  }
//
//  if (hal.report & PRINT_RAW_COMPASS) {
//      if (raw_compass_data_was_set == 1) {
//          //INV_SPRINTF(tst, INV_TST_LEN, "Raw Compass Data\t %7.5f, %7.5f, %7.5f,\t%d, %lld\r\n", compass_raw_float[0], compass_raw_float[1], compass_raw_float[2], 0, ts); print_command_console(tst);
//          //INV_SPRINTF(tst, INV_TST_LEN, "Compass Bias\t %7.3f, %7.3f, %7.3f\r\n", compass_bias[0], compass_bias[1], compass_bias[2]); print_command_console(tst);
//          raw_compass_data_was_set = 0;
//      }
//  }
//

    if (quat9_data_was_set == 1)
    {
        if (hal.report & PRINT_RV)
        {
            //INV_SPRINTF(tst, INV_TST_LEN, "RV\t %7.5f, %7.5f, %7.5f, %7.5f, %d, %lld\r\n", rv_float[0], rv_float[1], rv_float[2], rv_float[3], rv_accuracy, ts);
            //print_data_console(tst);
        }
        if (hal.report &  PRINT_CUBE_RV)
        {
            long temp_long_quat[3];
            dmp_get_9quaternion(long_quat);
            inv_convert_rotation_vector_1(long_quat, temp_long_quat);
            inv_compute_scalar_part(temp_long_quat, result_quat);
            //send_data_packet(PACKET_TYPE_QUAT, (void *)result_quat);

        }
#if defined MEMS_AUGMENTED_SENSORS
        if (hal.report & PRINT_ORIENT)
        {
            INV_SPRINTF(tst, INV_TST_LEN, "Orientation\t %7.5f, %7.5f, %7.5f, %lld\r\n", orientationFloat[0], orientationFloat[1], orientationFloat[2], ts);
            print_data_console(tst);
        }
        quat9_data_was_set = 0;
#endif
    }

    if (gmrv_quat_was_set == 1)
    {
        if (hal.report & PRINT_GEOMAG)
        {
            //INV_SPRINTF(tst, INV_TST_LEN, "GeoMAG RV\t %7.5f, %7.5f, %7.5f, %7.5f, %d, %lld\r\n", gmrv_float[0], gmrv_float[1], gmrv_float[2], gmrv_float[3], gmrv_accuracy, ts);
            //print_data_console(tst);
        }
        if (hal.report & PRINT_CUBE_GEOMAG)
        {
            //send_data_packet(PACKET_TYPE_QUAT, (void *)cube_gmrv);
        }
        gmrv_quat_was_set = 0;
    }

//  if (hal.report & PRINT_BAC) {
//      if (bac_data_was_set == 1)
//      {
//          //INV_SPRINTF(tst, INV_TST_LEN, "BAC Ts:\t %ld\r\n", bac_ts);
//          //print_data_console(tst);
////            if ((bac_state >> 8) & BAC_DRIVE)
////                print_data_console("\t Enter Drive\r\n");
////            if ((bac_state >> 8) & BAC_WALK)
////                print_data_console("\t Enter Walk\r\n");
////            if ((bac_state >> 8) & BAC_RUN)
////                print_data_console("\t Enter Run\r\n");
////            if ((bac_state >> 8) & BAC_BIKE)
////                print_data_console("\t Enter Bike\r\n");
////            if ((bac_state >> 8) & BAC_TILT)
////                print_data_console("\t Enter Tilt\r\n");
////            if ((bac_state >> 8) & BAC_STILL)
////                print_data_console("\t Enter Still\r\n");
////            if (bac_state & BAC_DRIVE)
////                print_data_console("\t Exit Drive\r\n");
////            if (bac_state & BAC_WALK)
////                print_data_console("\t Exit Walk\r\n");
////            if (bac_state & BAC_RUN)
////                print_data_console("\t Exit Run\r\n");
////            if (bac_state & BAC_BIKE)
////                print_data_console("\t Exit Bike\r\n");
////            if (bac_state & BAC_TILT)
////                print_data_console("\t Exit tilt\r\n");
////            if (bac_state & BAC_STILL)
////                print_data_console("\t Exit Still\r\n");
//
//          bac_data_was_set = 0;
//          //bac_prev_ts = ts;
//      }
//  }
//
//  if (hal.report & PRINT_FLIP_PICKUP) {
//      if (flip_data_was_set == 1) {
//          if (flip_pickup == 1){}
//          //  print_data_console("\t Flip Detected\r\n");
//          else if (flip_pickup == 2){}
//          //  print_data_console("\t Pickup Detected\r\n");
//          flip_data_was_set = 0;
//      }
//  }
//
    if (hal.report & PRINT_TILT)
    {
        if (tilt_data_was_set == 1)
        {
            if (tilt_state == 2) {}
            //  print_data_console("\t Tilt Started\r\n");
            else if (tilt_state == 1) {}
            //  print_data_console("\t Tilt Ended\r\n");
            tilt_data_was_set = 0;
        }
    }
#endif
    if (hal.report & PRINT_STEP_DETECTOR)
    {
        if (step_detected == 1)
            //    print_data_console("Step Detected>>>>\r\n");
            step_detected = 0;
    }


}

float inv_get_accelerometer_x()
{
    return accel_float[0];
}

float inv_get_accelerometer_y()
{
    return accel_float[1];
}

float inv_get_accelerometer_z()
{
    return accel_float[2];
}

void fifo_handler()
{
    short              int_read_back = 0;
    unsigned short     header = 0, header2 = 0;
    int data_left_in_fifo = 0;
    short short_data[3] = { 0 };
    signed long  long_data[3] = { 0 };
    signed long  long_quat[3] = { 0 };
    static mpu_time_t lastIrqTimeUs = 0;
    static mpu_time_t currentIrqTimeUs = 0;
    unsigned short sample_cnt_array[GENERAL_SENSORS_MAX] = { 0 };

    if (inv_flags != 0)
    {
        return;
    }

    I2C_Start();
    // Process Incoming INT and Get/Pack FIFO Data
    inv_identify_interrupt(&int_read_back);
#if (MEMS_CHIP != HW_ICM20609)
    if (int_read_back & (BIT_MSG_DMP_INT | BIT_MSG_DMP_INT_0 | BIT_MSG_DMP_INT_2 | BIT_MSG_DMP_INT_5))
#else
    if (int_read_back & BIT_DMP_INT_CI)
#endif
    {
        // Read FIFO contents and parse it.
        unsigned short total_sample_cnt = 0;
        //currentIrqTimeUs = inv_get_tick_count();

        do
        {


            if (inv_mems_fifo_swmirror(&data_left_in_fifo, &total_sample_cnt, sample_cnt_array))
                break;

            if (lastIrqTimeUs != 0)
                ts = (currentIrqTimeUs - lastIrqTimeUs) / total_sample_cnt;

            lastIrqTimeUs = currentIrqTimeUs;
            while (total_sample_cnt--)
            {
                if (inv_mems_fifo_pop(&header, &header2, &data_left_in_fifo))
                    break;

                if (header & ACCEL_SET)
                {
                    float scale;

                    accel_data_was_set = 1;
                    dmp_get_accel(long_data);

                    accel_accuracy = inv_get_accel_accuracy();
                    scale = (1 << inv_get_accel_fullscale()) * 2.f / (1L << 30); // Convert from raw units to g's
                    // scale *= 9.80665f; // Convert to m/s^2
                    inv_convert_dmp3_to_body(long_data, scale, accel_float);
                }


                if (header & GYRO_SET)
                {
                    float scale;
                    signed long  raw_data[3] = { 0 };
#if ((MEMS_CHIP == HW_ICM30630) || (MEMS_CHIP == HW_ICM20648) || (MEMS_CHIP == HW_ICM20609))
                    signed long  bias_data[3] = { 0 };
#endif

                    raw_gyro_data_was_set = 1;
                    dmp_get_raw_gyro(short_data);
                    scale = (1 << inv_get_gyro_fullscale()) * 250.f / (1L << 15); // From raw to dps
                    scale *= (float)M_PI / 180.f; // Convert to radian.
                    raw_data[0] = (long)short_data[0];
                    raw_data[1] = (long)short_data[1];
                    raw_data[2] = (long)short_data[2];
                    inv_convert_dmp3_to_body(raw_data, scale, gyro_raw_float);

                    //TODO: Anything to do?
                    //BackBone_SetGyroscopeData(gyro_float[0], gyro_float[1], gyro_float[2]);


#if ((MEMS_CHIP == HW_ICM30630) || (MEMS_CHIP == HW_ICM20648))
                    // We have gyro bias data in raw units, scaled by 2^5
                    scale = (1 << inv_get_gyro_fullscale()) * 250.f / (1L << 20); // From raw to dps
                    scale *= (float)M_PI / 180.f; // Convert to radian.
                    dmp_get_gyro_bias(short_data);
                    bias_data[0] = (long)short_data[0];
                    bias_data[1] = (long)short_data[1];
                    bias_data[2] = (long)short_data[2];
                    inv_convert_dmp3_to_body(bias_data, scale, gyro_bias_float);

                    if (hal.report & PRINT_GYRO)
                    {
                        // shift to Q20 to do all operations in Q20
                        raw_data[0] = raw_data[0] << 5;
                        raw_data[1] = raw_data[1] << 5;
                        raw_data[2] = raw_data[2] << 5;
                        inv_mems_dmp_get_calibrated_gyro(long_data, raw_data, bias_data);
                        inv_convert_dmp3_to_body(long_data, scale, gyro_float);
                        gyro_data_was_set = 1;
                    }
                    gyro_accuracy = inv_get_gyro_accuracy();

#elif (MEMS_CHIP == HW_ICM20609)
                    dmp_get_gyro_bias(bias_data);
                    scale = (1 << inv_get_gyro_fullscale()) * 250.f / (1L << 20); // From raw to dps
                    scale *= (float)M_PI / 180.f; // Convert to radian.
                    inv_convert_dmp3_to_body(bias_data, scale, gyro_bias_float);

                    if (hal.report & PRINT_GYRO)
                    {
                        inv_mems_dmp_get_calibrated_gyro(long_data, short_data, bias_data);
                        // shift to Q30 to do all operations in Q30
                        scale = (1 << inv_get_gyro_fullscale()) * 250.f / (1L << 30); // From raw to dps
                        scale *= (float)M_PI / 180.f; // Convert to radian.

                        inv_convert_dmp3_to_body(long_data, scale, gyro_float);

                        gyro_data_was_set = 1;
                    }
                    if ((bias_data[0] != 0) && (bias_data[1] != 0) && (bias_data[2] != 0))
                        gyro_accuracy = 3;
#endif
                }

                if (header & GYRO_CALIBR_SET)
                {
#if ((MEMS_CHIP != HW_ICM30630) && (MEMS_CHIP != HW_ICM20648))
                    float scale;
                    gyro_data_was_set = 1;
                    dmp_get_calibrated_gyro(long_data);

                    gyro_accuracy = inv_get_gyro_accuracy();
                    // We have gyro data in raw units, scaled by 2^15
                    scale = (1 << inv_get_gyro_fullscale()) * 250.f / (1L << 30); // From raw to dps
                    scale *= (float)M_PI / 180.f; // Convert to radian.
                    inv_convert_dmp3_to_body(long_data, scale, gyro_float);
#endif
                }
#if (MEMS_CHIP != HW_ICM20609)
                if (header & CPASS_CALIBR_SET)
                {
                    float scale;
                    compass_data_was_set = 1;
                    dmp_get_calibrated_compass(long_data);
                    compass_accuracy = inv_get_mag_accuracy();
                    scale = 1.52587890625e-005f; //COMPASS_CONVERSION
                    inv_convert_dmp3_to_body(long_data, scale, compass_float);
                }

                if (header & CPASS_SET)
                {
                    // Raw compass [DMP]
                    dmp_get_raw_compass(long_data);
                    compass_raw_float[0] = long_data[0] * 1.52587890625e-005f;
                    compass_raw_float[1] = long_data[1] * 1.52587890625e-005f;
                    compass_raw_float[2] = long_data[2] * 1.52587890625e-005f;
                    compass_bias[0] = compass_raw_float[0] - compass_float[0];
                    compass_bias[1] = compass_raw_float[1] - compass_float[1];
                    compass_bias[2] = compass_raw_float[2] - compass_float[2];
                    raw_compass_data_was_set = 1;
                }
#endif

                if (header & QUAT6_SET)
                {
                    quat6_data_was_set = 1;

                    dmp_get_6quaternion(long_quat);
                    inv_convert_rotation_vector(long_quat, grv_float);
#if defined MEMS_AUGMENTED_SENSORS
                    long gravityQ16[3], temp_gravityQ16[3];
                    long linAccQ16[3];
                    long accelQ16[3];
                    /*Calculate Gravity*/
                    inv_convert_rotation_vector_1(long_quat, temp_gravityQ16);
                    inv_mems_augmented_sensors_get_gravity(gravityQ16, temp_gravityQ16);
                    gravityFloat[0] = inv_q16_to_float(gravityQ16[0]);
                    gravityFloat[1] = inv_q16_to_float(gravityQ16[1]);
                    gravityFloat[2] = inv_q16_to_float(gravityQ16[2]);
                    /*Calculate Linear Acceleration*/
                    accelQ16[0] = (int32_t)((float)(accel_float[0])*(1ULL << 16) + ((accel_float[0] >= 0) - 0.5f));
                    accelQ16[1] = (int32_t)((float)(accel_float[1])*(1ULL << 16) + ((accel_float[1] >= 0) - 0.5f));
                    accelQ16[2] = (int32_t)((float)(accel_float[2])*(1ULL << 16) + ((accel_float[2] >= 0) - 0.5f));
                    inv_mems_augmented_sensors_get_linearacceleration(linAccQ16, gravityQ16, accelQ16);
                    linAccFloat[0] = inv_q16_to_float(linAccQ16[0]);
                    linAccFloat[1] = inv_q16_to_float(linAccQ16[1]);
                    linAccFloat[2] = inv_q16_to_float(linAccQ16[2]);
#endif
                }
#if (MEMS_CHIP != HW_ICM20609)
                /* 9axis orientation quaternion sample available from DMP FIFO */
                if (header & QUAT9_SET)
                {
#if defined MEMS_AUGMENTED_SENSORS
                    long orientationQ16[3],temp_orientationQ16[3];
#endif

                    quat9_data_was_set = 1;
                    dmp_get_9quaternion(long_quat);
#if (MEMS_CHIP == HW_ICM20630 || MEMS_CHIP == HW_ICM20648)
                    rv_accuracy = (int)((float)inv_get_rv_accuracy() / (float)(1ULL << (29)));
#else
                    compass_accuracy = inv_get_mag_accuracy();
#endif
                    inv_convert_rotation_vector(long_quat, rv_float);

#if defined MEMS_AUGMENTED_SENSORS
                    if (hal.report & PRINT_ORIENT)
                    {
                        inv_convert_rotation_vector_1(long_quat, temp_orientationQ16);
                        inv_mems_augmented_sensors_get_orientation(orientationQ16, temp_orientationQ16);
                        orientationFloat[0] = inv_q16_to_float(orientationQ16[0]);
                        orientationFloat[1] = inv_q16_to_float(orientationQ16[1]);
                        orientationFloat[2] = inv_q16_to_float(orientationQ16[2]);
                    }
#endif
                }

#if (MEMS_CHIP == HW_ICM20648)
                /* 6axis AM orientation quaternion sample available from DMP FIFO */
                if (header & GEOMAG_SET)
                {
                    /* Read 6 axis quaternion out of DMP FIFO in Q30 and convert it to Android format */
                    dmp_get_gmrvquaternion(long_quat);
                    if (hal.report & PRINT_CUBE_GEOMAG)
                        inv_compute_scalar_part(long_quat, cube_gmrv);
                    if (hal.report & PRINT_GEOMAG)
                    {
                        inv_convert_rotation_vector(long_quat, gmrv_float);
                        /* Read geomagnetic rotation vector heading accuracy out of DMP FIFO in Q29*/
                        gmrv_accuracy = (int)((float)inv_get_gmrv_accuracy() / (float)(1ULL << (29)));
                    }
                    gmrv_quat_was_set = 1;
                }
#endif


#if (MEMS_CHIP == HW_ICM20645e) || (MEMS_CHIP == HW_ICM20648)
                /* Activity recognition sample available from DMP FIFO */
                if (header2 & ACT_RECOG_SET)
                {
                    /* Read activity type and associated timestamp out of DMP FIFO
                    activity type is a set of 2 bytes :
                    - high byte indicates activity start
                    - low byte indicates activity end */
                    dmp_get_bac_state(&bac_state);
                    dmp_get_bac_ts(&bac_ts);

                    if (hal.report & PRINT_TILT)
                    {
                        /* Tilt information is inside BAC, so extract it out */

                        /* Check if bit tilt is set for activity start byte */
                        if ((bac_state >> 8) & BAC_TILT)
                        {
                            /* Start of tilt detected */
                            tilt_state = 2;
                        }
                        /* Check if bit tilt is set for activity end byte */
                        else if (bac_state & BAC_TILT)
                        {
                            /* End of tilt detected */
                            tilt_state = 1;
                        }

                    }

                    if (hal.report & PRINT_BAC)
                        bac_data_was_set = 1;
                    if (hal.report & PRINT_TILT)
                        tilt_data_was_set = 1;
                }

                if (header2 & FLIP_PICKUP_SET)
                {
                    if (hal.report & PRINT_FLIP_PICKUP)
                    {
                        dmp_get_flip_pickup_state(&flip_pickup);
                        flip_data_was_set = 1;
                    }
                }
#endif
#endif

                process_sensor_output();
            }
            if (!data_left_in_fifo)
                break;

        }
        while (data_left_in_fifo);

#if (MEMS_CHIP == HW_ICM20609)
        if (int_read_back & DMP_INT_SMD)
        {
            if (hal.report & PRINT_SMD)
            {
                print_command_console(">> SMD Interrupt *********\r\n");
            }

        }
#endif
#if (MEMS_CHIP != HW_ICM20609)
        if (int_read_back & BIT_MSG_DMP_INT_3)
        {
#else
        if (int_read_back & DMP_INT_PED)
        {
#endif
            if (hal.report & PRINT_STEP_DETECTOR)
            {
//              print_command_console("Step Detected>>>>>>>\r\n");

            }
        }
#if (MEMS_CHIP != HW_ICM20609)
//                if (header & PED_STEPDET_SET){
#else
        if (int_read_back & DMP_INT_PED)
        {
#endif
        if (hal.report & PRINT_STEP_COUNTER)
        {
            unsigned long steps = 0;
            static unsigned long old_steps;
            dmp_get_pedometer_num_of_steps(&steps);

            // TODO: Figure out what to do with this
            //BackBone_SetStepCount(steps);

            if (steps != old_steps)
            {
//                  INV_SPRINTF(tst, INV_TST_LEN, "\tStep Counter %d\r\n", steps);
//                  print_command_console(tst);
                old_steps = steps;
            }
        }
//      }

#if (MEMS_CHIP == HW_ICM20648)
        if (int_read_back & BIT_MSG_DMP_INT_2)
        {
            if (hal.report & PRINT_SMD) {}
//              print_command_console(">> SMD Interrupt *********\r\n");
        }
#endif
    }
    I2C_Stop();
}
