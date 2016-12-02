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
static const uint32_t FILTER_INIT_TIME = 3;

typedef struct
{
    bool calibrated;
    float filtered_y;
    float filtered_z;
    float calibrated_y;
    float calibrated_z;
    float distance;
    uint32_t start_time;
    uint32_t duration;
    float distance_threshold;
    uint16_t time_threshold;
    uint32_t slouch_detect_time;
    bool notify_slouch;
} posture_t;

static posture_t self;

void posture_start(uint32_t start_time, 
                   uint32_t duration, 
                   float distance_threshold, 
                   uint16_t time_threshold)
{
    self.calibrated = false;
    self.filtered_y = 0.0;
    self.filtered_z = 0.0;
    self.calibrated_y = 0.0;
    self.calibrated_z = 0.0;
    self.distance = 0.0;
    self.start_time = start_time;
    self.duration = duration;
    self.distance_threshold = distance_threshold;
    self.time_threshold = time_threshold;
    
    inv_enable_accelerometer();
}

void posture_stop(void)
{
    inv_disable_accelerometer();
}

float low_pass_filter(float current, float input)
{
    return current + (ALPHA * (input - current));
}

void posture_update(float y, float z, uint32_t time)
{
    self.filtered_y = low_pass_filter(self.filtered_y, y);
    self.filtered_z = low_pass_filter(self.filtered_z, z);

    if (!self.calibrated)
    {
        self.calibrated_y = y;
        self.calibrated_z = z;
        self.calibrated = true;
        self.distance = 0;
    }
    
    if (time - self.start_time > FILTER_INIT_TIME)
    {
        float y_diff = self.calibrated_y - self.filtered_y;
        float z_diff = self.calibrated_z - self.filtered_z;
        self.distance = sqrt((y_diff * y_diff) + (z_diff * z_diff));
        
        if (self.distance > self.distance_threshold)
        {
            if (self.slouch_detect_time == 0)
            {
                self.slouch_detect_time = time;
            }
            
            uint32_t slouch_elapsed = time - self.slouch_detect_time;
            if (slouch_elapsed > self.time_threshold)
            {
                self.slouch_detect_time = 0;
                self.notify_slouch = true;
            }
            else
            {
                self.notify_slouch = false;
            }
        }
        else
        {
            self.slouch_detect_time = 0;
            self.notify_slouch = false;
        }
    }
    
    if (time - self.start_time > self.duration)
    {
        posture_stop();
    }
}

float posture_get_distance(void)
{
    return self.distance;
}

bool posture_is_notify_slouch()
{
    return self.notify_slouch;
}