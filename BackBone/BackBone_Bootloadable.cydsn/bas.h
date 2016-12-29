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

#ifndef BAS_H_
#define BAS_H_

#include <project.h>
#include <stdbool.h>

/***************************************
*          Constants
***************************************/

/* Counts depend on connection parameters */
#define BATTERY_TIMEOUT             (3000u)

/* Various thresholds for battery voltage measurement */
#define MEASURE_BATTERY_MAX         (4200)
#define MEASURE_BATTERY_MID         (3500)
#define MEASURE_BATTERY_MID_PERCENT (40)
#define MEASURE_BATTERY_MIN         (3000)
#define LOW_BATTERY_LIMIT           (10)

#define ADC_VREF_MASK               (0x000000F0Lu)

/***************************************
*       Function Prototypes
***************************************/
void BasCallBack(uint32 event, void *eventParam);
void MeasureBattery(bool immediate);
void SimulateBattery(void);


/***************************************
* External data references
***************************************/
extern uint16 batterySimulationNotify;
extern uint16 batteryMeasureNotify;

#endif /* BAS_H_ */
