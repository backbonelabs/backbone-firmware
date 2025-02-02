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
#define BATTERY_TIMEOUT             (6000u)

#define ADC_VREF_MASK               (0x000000F0Lu)

/***************************************
*       Function Prototypes
***************************************/
void BasCallBack(uint32 event, void *eventParam);
int32 MeasureBattery(bool immediate);
void SimulateBattery(void);


/***************************************
* External data references
***************************************/
extern uint16 batterySimulationNotify;
extern uint16 batteryMeasureNotify;

#endif /* BAS_H_ */
