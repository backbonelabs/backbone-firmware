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

#include <project.h>

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
void MeasureBattery(void);
void SimulateBattery(void);


/***************************************
* External data references
***************************************/
extern uint16 batterySimulationNotify;
extern uint16 batteryMeasureNotify;

/* [] END OF FILE */
