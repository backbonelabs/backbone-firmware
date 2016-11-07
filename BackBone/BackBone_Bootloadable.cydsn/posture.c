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

#include <stdbool.h>
#include <math.h>

typedef struct
{
    bool calibrated;
    float calibrated_y;
    float calibrated_z;
    float distance;
} posture_t;

static posture_t self;

void posture_start()
{
    self.calibrated = false;
    self.calibrated_y = 0.0;
    self.calibrated_z = 0.0;
}

void posture_update(float y, float z)
{
    if (!self.calibrated)
    {
        self.calibrated_y = y;
        self.calibrated_z = z;
        self.calibrated = true;
        self.distance = 0;
    }
    else
    {
        float y_diff = self.calibrated_y - y;
        float z_diff = self.calibrated_z - z;
        self.distance = sqrt((y_diff * y_diff) + (z_diff * z_diff));
    }
}

float posture_get_distance()
{
    return self.distance;
}

#if 0
- (void)calculatePostureMetrics:(NSDictionary *)data
{

    // Store y axis data
    double y = [[data objectForKey:@"y"] doubleValue];

    // Store z axis data
    double z = [[data objectForKey:@"z"] doubleValue];

    // Check if we've already set calibration values
    if (!self.calibrated)
    {

        // Set control metrics/calibrated posture values
        self.controlY = y;
        self.controlZ = z;

        // Set calibrated flag to true, so that we don't set values again
        self.calibrated = true;

    }
    else
    {

        // Calculate difference between calibrated posture values and current y & z axes values
        // Use Pythagorean Theorem to calculate distance between current and calibrated posture values
        self.currentDistance = sqrt(pow((self.controlZ - z), 2) + pow((self.controlY - y), 2));

        // Once current distance is calculated, call handle distance to decide what to do with it
        [self handleDistance];
    }
}

- (void)handleDistance
{

    // Check whether posture distance exceeds the distance threshold
    if (self.currentDistance >= self.distanceThreshold)
    {

        // Store timestamp of when user's slouching initially started
        if (!self.time)
        {
            self.time = [[NSDate date] timeIntervalSince1970];
        }
        else
        {

            // Calculate how much time has elapsed since slouching started
            self.slouchTime = [[NSDate date] timeIntervalSince1970] - self.time;
        }

        // Check if user has been slouching for longer than slouch time threshold
        // Slouch time threshold is how long the user can slouch for before being notified
        if (self.slouchTime > self.slouchTimeThreshold)
        {

            // Emit posture data event
            [self emitPostureData];

            // Reset slouch time variables, in order to calculate time again
            self.time = 0;
            self.slouchTime = 0;
        }
    }
    else
    {
        // User stopped slouching, reset slouch time variables
        self.time = 0;
        self.slouchTime = 0;
    }

    // Emit posture data event
    [self emitPostureData];
}
/* [] END OF FILE */
#endif