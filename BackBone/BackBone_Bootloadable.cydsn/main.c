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

#include "common.h"
#include "bas.h"
#include "inv.h"
#include "timeout.h"
#include "version.h"
#include "ble.h"
#include "OTAMandatory.h"
#include "watchdog.h"
#include "posture.h"
#include "backbone.h"
#include "motor.h"
#include "debug.h"

#ifdef ENABLE_MTK
    #include "CyBLE_MTK.h"
#endif

extern void InitializeBootloaderSRAM();

static bool m_stop_and_reset;
CY_ISR(reset_timeout)
{
    MotorTimer_ClearInterrupt(MotorTimer_INTR_MASK_TC);
    m_stop_and_reset = true;
}

__inline void ManageSystemPower()
{
    CYBLE_BLESS_STATE_T blePower;
    uint8 interruptStatus ;

    interruptStatus = CyEnterCriticalSection();

    blePower = CyBle_GetBleSsState();

    if (!motor_is_running() && !backbone_is_reset_pending())
    {
        /* System can enter DeepSleep only when BLESS and rest of the
         * application are in DeepSleep power modes */
        if (blePower == CYBLE_BLESS_STATE_DEEPSLEEP ||
            blePower == CYBLE_BLESS_STATE_ECO_ON)
        {
            VIN_Sleep();
            ADC_Sleep();
            MOTOR_PIN_Sleep();
            MotorPWM_Sleep();
            MotorTimer_Sleep();
            I2C_Sleep();
            I2C_sda_Sleep();
            I2C_scl_Sleep();
            INVN_INT_Sleep();
            Timer1ms_Sleep();

            CySysPmDeepSleep();

            Timer1ms_Wakeup();
            INVN_INT_Wakeup();
            I2C_scl_Wakeup();
            I2C_sda_Wakeup();
            I2C_Wakeup();
            MotorTimer_Wakeup();
            MotorPWM_Wakeup();
            MOTOR_PIN_Wakeup();
            ADC_Wakeup();
            VIN_Wakeup();
        }
        else if (blePower != CYBLE_BLESS_STATE_EVENT_CLOSE)
        {
            /* change HF clock source from IMO to ECO, as IMO is not required
             * and can be stopped to save power */
            CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_ECO);
            /* stop IMO for reducing power consumption */
            CySysClkImoStop();
            /* put the CPU to sleep */
            CySysPmSleep();
            /* starts execution after waking up, start IMO */
            CySysClkImoStart();
            /* change HF clock source back to IMO */
            CySysClkWriteHfclkDirect(CY_SYS_CLK_HFCLK_IMO);
        }
    }

    CyExitCriticalSection(interruptStatus );
}

__inline void ManageApplicationPower()
{
    // put any application components to sleep.
}

__inline void RunApplication()
{
    // If there is data to read from the accelerometer, read it
    // then go to deep sleep.
    if (hal.new_gyro == 1)
    {
        hal.new_gyro = 0;
        CyDelay(10);
        fifo_handler();

        posture_update(inv_get_accelerometer_x(),
                       inv_get_accelerometer_y(),
                       inv_get_accelerometer_z(),
                       watchdog_get_time());

        if (posture_is_notify_slouch())
        {
            motor_start(posture_get_duty_cycle(),
                        posture_get_motor_on_time(),
                        posture_get_vibration_pattern());
        }

        if (ble_is_connected())
        {
            backbone_accelerometer_t accelerometer_data;
            backbone_distance_t distance_data;
            backbone_slouch_t slouch_data;
            backbone_session_statistics_t session_statistics_data;

            accelerometer_data.axis[0] = inv_get_accelerometer_x();
            accelerometer_data.axis[1] = inv_get_accelerometer_y();
            accelerometer_data.axis[2] = inv_get_accelerometer_z();
            accelerometer_data.axis[3] = 0;
            backbone_set_accelerometer_data(ble_get_connection(), &accelerometer_data);

            distance_data.fields.distance = posture_get_distance();
            distance_data.fields.elapsed_time = posture_get_elapsed_time();
            backbone_set_distance_data(ble_get_connection(), &distance_data);

            session_statistics_data.fields.flags = 1;
            session_statistics_data.fields.total_time = posture_get_elapsed_time();
            session_statistics_data.fields.slouch_time = posture_get_slouch_time();
            backbone_set_session_statistics_data(ble_get_connection(), &session_statistics_data);

            slouch_data.slouch[0] = posture_is_slouch() ? 1 : 0;
            backbone_set_slouch_data(ble_get_connection(), &slouch_data);

            backbone_notify_accelerometer(ble_get_connection());
            backbone_notify_distance(ble_get_connection());

            if (posture_is_notify_slouch())
            {
                backbone_notify_slouch(ble_get_connection());
            }
        }

        if (posture_get_elapsed_time() >= posture_get_session_duration() &&
            posture_get_session_duration() != 0)
        {
            posture_stop();

            if (ble_is_connected())
            {
                backbone_session_statistics_t session_statistics_data;

                session_statistics_data.fields.flags = 0;
                session_statistics_data.fields.total_time = posture_get_elapsed_time();
                session_statistics_data.fields.slouch_time = posture_get_slouch_time();
                backbone_set_session_statistics_data(ble_get_connection(), &session_statistics_data);
                backbone_notify_session_statistics(ble_get_connection());
            }
        }
    }

    if (ble_is_connected())
    {
        ble_update_connection_parameters();
        MeasureBattery(false);

        if (backbone_is_selftest_requested())
        {
            backbone_clear_selftest_requested();
            int32 mvolts = MeasureBattery(true);
            backbone_status_t status;
            inv_rerun_selftest();
            status.fields.inv_init = inv_get_init_status();
            status.fields.inv_selftest = inv_get_selftest_status();
            status.fields.reserved1 = mvolts;
            status.fields.reserved2 = 0;
            backbone_set_status_data(ble_get_connection(), &status);
            backbone_notify_status(ble_get_connection());
        }
    }

    if (backbone_is_reset_requested())
    {
        backbone_clear_reset_requested();

        posture_stop();
        motor_stop();
        CyBle_GapDisconnectWithReason(ble_get_connection()->bdHandle, 0x15);

        // Now that the motor is stopped, we can reuse the motor timer to give
        // a short delay before resetting into the bootloader.
        MotorTimerInterrupt_StartEx(reset_timeout);
        MotorTimer_WriteCounter(0);
        MotorTimer_Start();
        MotorTimer_SetOneShot(1);
        MotorTimer_WritePeriod(1500);
    }
}

__inline void ManageBlePower()
{
    CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
}

void RunBle()
{
    CyBle_ProcessEvents();

#if(CYBLE_BONDING_REQUIREMENT == CYBLE_BONDING_YES)
    if (cyBle_pendingFlashWrite != 0u)
    {
        CyBle_StoreBondingData(0u);
    }
#endif

#if 0
    /* Wait until BLESS is in ECO_STABLE state to push the notification data to the BLESS */
    if (CyBle_GetBleSsState() == CYBLE_BLESS_STATE_ECO_STABLE)
    {
        // Send notifications.
    }
#endif
}

/**
 * Main function of BackBone firmware.
 *
 * Implements the application main loop, procsses BLE events, handles high level
 * application logic.  Puts the system into low power modes when there is no
 * work to do.
 *
 * Architecture of this is based on App Note: AN92584 "Designing for Low Power
 * and Estimating Battery Life for BLE Applications"
 * http://www.cypress.com/documentation/application-notes/an92584-designing-low-power-and-estimating-battery-life-ble
 */
int main()
{
    m_stop_and_reset = false;
    watchdog_init();
    CyGlobalIntEnable;
#if !defined(__ARMCC_VERSION)
    InitializeBootloaderSRAM();
#endif

    AfterImageUpdate();

    DBG_PRINT_TEXT("> Backbone Firmware\r\n");
    DBG_PRINT_TEXT("> Compile Date and Time: " __DATE__ " " __TIME__ "\r\n\r\n");

    /* Internal low power oscillator is stopped as it is not used in this
     * project */
    CySysClkIloStop();

    /* Set the divider for ECO, ECO will be used as source when IMO is switched
     * off to save power */
    CySysClkWriteEcoDiv(CY_SYS_CLK_ECO_DIV8);

#ifdef ENABLE_MTK
    while (1)
    {
        MTK_mode();
    }
#endif

    /* Initialize the hardware first.  The InvenSense chip takes especially long
     * (about 10 seconds) because the embedded motion processor firmware is
     * loaded over I2C.  */
    inv_start();
    ADC_Start();
    motor_init();

    /* Initialize BLE after the hardware so that firmware
     * is able to respond to BLE requests. */
    backbone_init();
    ble_init();
    CyBle_Start(ble_app_event_handler);
    CyBle_BasRegisterAttrCallback(BasCallBack);

    while (CyBle_GetState() == CYBLE_STATE_INITIALIZING)
    {
        CyBle_ProcessEvents();
    }

    while (1)
    {
        RunBle();
        ManageBlePower();

        RunApplication();
        ManageApplicationPower();

        ManageSystemPower();

        if (watchdog_is_clear_requested())
        {
            watchdog_clear();
        }

        if (m_stop_and_reset)
        {
            CyBle_Stop();

            // Finally, switch to the bootloader / stack project
            Bootloadable_SetActiveApplication(0);
            Bootloadable_Load();
        }
    }
}
