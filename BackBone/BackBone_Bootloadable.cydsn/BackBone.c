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
#include <BackBone.h>
#include <BLEApplications.h>

ACCELEROMETER Accelerometer = {0, 0, 0, 0};
GYROSCOPE Gyroscope = {0, 0, 0};
CONFIG_DATA ConfigData = {0};
PEDOMETER Pedometer = {0};

uint8 BackBoneFlags;

void BackBone_SetConfigData(uint8* SourceBuffer, uint8 Length)
{
	uint8* pointer = SourceBuffer;
	uint8 i;
	
	for (i=0; i<Length; i++)
	{
		ConfigData.RawData[i] = *pointer;
		pointer++;
	}
}

void BackBone_GetAccelerometerData(uint8* DestinationBuffer, uint8 Length)
{
	uint8* pointer = DestinationBuffer;
	uint8 i;
	
	for (i=0; i<Length; i++)
	{
		*pointer = Accelerometer.RawData[i];
		pointer++;
	}
}

void BackBone_GetGyroscopeData(uint8* DestinationBuffer, uint8 Length)
{
	uint8* pointer = DestinationBuffer;
	uint8 i;
	
	for (i=0; i<Length; i++)
	{
		*pointer = Gyroscope.RawData[i];
		pointer++;
	}
}

void BackBone_SetAccelerometerData(float XAxis, float YAxis, float ZAxis, float RMS)
{
	Accelerometer.AccelerometerData[0] = XAxis;	
	Accelerometer.AccelerometerData[1] = YAxis;	
	Accelerometer.AccelerometerData[2] = ZAxis;	
	Accelerometer.AccelerometerData[3] = RMS;
	
	BackBoneFlags |= ACCEL_DATA_NEW;
}

void BackBone_SetGyroscopeData(float XAxis, float YAxis, float ZAxis)
{
	Gyroscope.GyroscopeData[0] = XAxis;
	Gyroscope.GyroscopeData[1] = YAxis;
	Gyroscope.GyroscopeData[2] = ZAxis;
	
	BackBoneFlags |= GYRO_DATA_NEW;
}

uint32 BackBone_GetStepCount(void)
{
	return Pedometer.StepCount;
}	

void BackBone_SetStepCount(uint32 Count)
{
	Pedometer.StepCount = Count;
	BackBoneFlags |= PEDO_DATA_NEW;
}

void BackBone_Task(void)
{
	SendAllNotifications();
}
/* [] END OF FILE */
