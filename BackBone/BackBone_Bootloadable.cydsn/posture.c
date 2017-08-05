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

#include <stdbool.h>
#include <math.h>

#include "inv.h"
#include "debug.h"

static const float ALPHA = 0.4;

typedef enum
{
    STATE_CALIBRATE,
    STATE_MONITOR,
    STATE_SLOUCH,
    STATE_SLOUCH_EXCEED,
} state_t;

typedef struct
{
    float filtered_y;
    float filtered_z;
    float calibrated_y;
    float calibrated_z;
    float distance;
    uint32_t start_time;
    uint32_t duration;
    float distance_threshold;
    uint16_t time_threshold;
    uint32_t slouch_start_time;
    bool notify_slouch;
    uint32_t elapsed_time;
    uint32_t slouch_elapsed_time;
    uint32_t total_slouch_elapsed_time;
    uint32_t last_notify_time;
    uint8_t pattern;
    uint8_t duty_cycle;
    uint16_t motor_on_time;
    state_t state;
    bool active;
} posture_t;

static posture_t self;

void posture_start(uint32_t start_time,
                   uint32_t duration,
                   float distance_threshold,
                   uint16_t time_threshold,
                   uint8_t pattern,
                   uint8_t duty_cycle,
                   uint16_t motor_on_time)
{
    self.filtered_y = 0.0;
    self.filtered_z = 0.0;
    self.calibrated_y = 0.0;
    self.calibrated_z = 0.0;
    self.distance = 0.0;
    self.start_time = start_time;
    self.duration = duration;
    self.distance_threshold = distance_threshold;
    self.time_threshold = time_threshold;
    self.slouch_start_time = 0;
    self.notify_slouch = false;
    self.elapsed_time = 0;
    self.slouch_elapsed_time = 0;
    self.total_slouch_elapsed_time = 0;
    self.last_notify_time = 0;
    self.pattern = pattern;
    self.duty_cycle = duty_cycle;
    self.motor_on_time = motor_on_time;
    self.state = STATE_CALIBRATE;
    self.active = true;
    
    if (!inv_is_accelerometer_enabled())
    {
        DBG_PRINT_TEXT("Enabling Accelerometer\r\n");
        inv_enable_accelerometer();
    }
}

void posture_pause(void)
{
    self.active = false;
}

void posture_resume(uint32_t resume_time,
                    float distance_threshold,
                    uint16_t time_threshold,
                    uint8_t pattern,
                    uint8_t duty_cycle,
                    uint16_t motor_on_time)
{
    if (self.start_time != 0)
    {
        if (self.state == STATE_SLOUCH_EXCEED || self.state == STATE_SLOUCH)
        {
            self.slouch_start_time = resume_time - self.slouch_elapsed_time;
        }

        self.start_time = resume_time - self.elapsed_time;
        self.distance_threshold = distance_threshold;
        self.time_threshold = time_threshold;
        self.pattern = pattern;
        self.duty_cycle = duty_cycle;
        self.motor_on_time = motor_on_time;

        self.active = true;
    }
}

void posture_stop(void)
{
    self.active = false;
    self.start_time = 0;
    if (self.slouch_elapsed_time != 0)
    {
        self.total_slouch_elapsed_time += self.slouch_elapsed_time;
        self.slouch_elapsed_time = 0;
    }
}

bool posture_is_monitoring(void)
{
    return self.active;    
}

static float low_pass_filter(float current, float input)
{
    return current + (ALPHA * (input - current));
}

static void update_distance(float x, float y, float z)
{
    (void)x;

    self.filtered_y = low_pass_filter(self.filtered_y, y);
    self.filtered_z = low_pass_filter(self.filtered_z, z);
    float y_diff = self.calibrated_y - self.filtered_y;
    float z_diff = self.calibrated_z - self.filtered_z;
    self.distance = sqrt((y_diff * y_diff) + (z_diff * z_diff));
}

void posture_update(float x, float y, float z, uint32_t time)
{
    if (x == 0.0 && y == 0.0 && z == 0.0)
    {
        // The first sample after power on reset always seems to be {0, 0, 0}
        // Just ignore it for now.
        return;
    }

    self.elapsed_time = time - self.start_time;

    switch (self.state)
    {
        case STATE_CALIBRATE:
            self.calibrated_y = y;
            self.calibrated_z = z;
            self.filtered_y = y;
            self.filtered_z = z;
            self.state = STATE_MONITOR;
            break;

        case STATE_MONITOR:
            self.notify_slouch = false;
            update_distance(x, y, z);
            if (self.distance >= self.distance_threshold)
            {
                self.slouch_start_time = time;
                self.state = STATE_SLOUCH;
            }
            break;

        case STATE_SLOUCH:
            update_distance(x, y, z);
            if (self.distance < self.distance_threshold)
            {
                self.slouch_start_time = 0;
                self.state = STATE_MONITOR;
                break;
            }

            uint32_t slouch_elapsed = time - self.slouch_start_time;
            if (slouch_elapsed >= self.time_threshold)
            {
                self.notify_slouch = true;
                self.state = STATE_SLOUCH_EXCEED;
                self.last_notify_time = time;
            }
            break;

        case STATE_SLOUCH_EXCEED:
            self.notify_slouch = false;
            self.slouch_elapsed_time = time - self.slouch_start_time;

            update_distance(x, y, z);
            if (self.distance < self.distance_threshold)
            {
                self.slouch_start_time = 0;
                self.state = STATE_MONITOR;
                self.total_slouch_elapsed_time += self.slouch_elapsed_time;
                self.slouch_elapsed_time = 0;
                break;
            }

            uint32_t last_notify_elapsed_time = time - self.last_notify_time;
            if (last_notify_elapsed_time >= self.time_threshold)
            {
                self.notify_slouch = true;
                self.last_notify_time = time;
            }
            break;
    }
}

uint32_t posture_get_session_duration(void)
{
    return self.duration;
}

uint32_t posture_get_elapsed_time(void)
{
    return self.elapsed_time;
}

uint32_t posture_get_slouch_time(void)
{
    return self.total_slouch_elapsed_time | self.slouch_elapsed_time;
}

float posture_get_distance(void)
{
    return self.distance;
}

bool posture_is_notify_slouch(void)
{
    return self.notify_slouch;
}

bool posture_is_slouch(void)
{
    return self.state == STATE_SLOUCH || self.state == STATE_SLOUCH_EXCEED;
}

uint8_t posture_get_vibration_pattern(void)
{
    return self.pattern;
}

uint8_t posture_get_duty_cycle(void)
{
    return self.duty_cycle;
}

uint16_t posture_get_motor_on_time(void)
{
    return self.motor_on_time;
}
