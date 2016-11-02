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

#ifndef __BACKBONE_H
#define __BACKBONE_H

#include <project.h>

typedef union
{
	float AccelerometerData[4];
	uint8 RawData[16];
}ACCELEROMETER;
	
typedef union
{
	float DistanceData;
	uint8 RawData[4];
}DISTANCE;

typedef union
{
	uint32 StepCount;
	uint8 RawData[4];
}PEDOMETER;

extern uint8 BackBoneFlags;

typedef union
{
	struct
	{
		uint16 Flags;
		uint16 MotorStrength;
		uint16 AccelerometerTransmissionRate;
		uint16 GyroscopeTransmissionRate;
		float PostureThreshold;
		uint16 PostureReminderInterval;
		uint16 InactivityDuration;
		uint16 InactivityReminderInterval;
	}Parameter;
	uint8 RawData[18];
}CONFIG_DATA;

#define ACCEL_DATA_LEN						(16u)
#define DISTANCE_DATA_LEN					(4u)
#define PEDOMETER_DATA_LEN					(4u)
#define CONFIG_DATA_LEN						(18u)
#define VERSION_INFO_DATA_LEN				(4u)

#define ACCEL_DATA_NEW						(0x01)
#define DISTANCE_DATA_NEW					(0x02)
#define PEDO_DATA_NEW						(0x04)
#define VERSION_DATA_NEW					(0x08)

void BackBone_GetGyroscopeData(uint8* DestinationBuffer, uint8 Length);
void BackBone_GetAccelerometerData(uint8* DestinationBuffer, uint8 Length);
uint32 BackBone_GetStepCount(void);
void BackBone_SetStepCount(uint32 Count);
void BackBone_SetAccelerometerData(float XAxis, float YAxis, float ZAxis, float RMS);
void BackBone_SetDistanceData(float distance);
void BackBone_SetConfigData(uint8* SourceBuffer, uint8 Length);
void BackBone_Task(void);

extern ACCELEROMETER Accelerometer;
extern DISTANCE Distance;
extern CONFIG_DATA ConfigData;
extern PEDOMETER Pedometer;

#endif /* __BACKBONE_H */	

/* [] END OF FILE */
