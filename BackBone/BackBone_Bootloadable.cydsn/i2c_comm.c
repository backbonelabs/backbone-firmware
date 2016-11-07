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
#include <i2c_comm.h>
#include <timeout.h>
#include <.\20648_driver\common\inv_mems_drv_hook.h>

uint8 CommErrorFlags = 0;
uint8 I2CTimeout = 0;
uint8 BrixCommErrorCount = 0;
uint8 KeypadCommErrorCount = 0;
uint8 CyFiCommErrorCount = 0;

void I2CComm_Init(void)
{
    I2C_Start();
}

/******************************************************************************
* Function Name: I2CComm_WriteBuffer
*******************************************************************************
*
* Summary:
*  This function writes to an I2C Slave.  After initializing an I2C write,
*  the function waits till the write completes successfully, or if a timeout or
*  error occurs.  The function returns the status of the transfer
*
*
* Parameters:
*  SlaveAddr - 7 bit Slave address
*  SourceBuffer - Pointer to the buffer which contains the data to be written to slave
*  DataCount - Number of bytes to be written
*  Timeout - Timeout value in ms
*
* Return:
*  Status of the write
*    COMM_ERROR - If there is an error in the read
*    COMM_SUCCESS - If the read was successful
*
*******************************************************************************/
int16 I2CComm_WriteBuffer(uint8 SlaveAddr, uint8* SourceBuffer, uint8 DataCount, uint8 Timeout, uint8 Mode)
{
    uint8 I2CStatus = 0;
    int16 Status = COMM_ERROR;

    /* Enable the I2C timeout */
    Timeout_Start();
    I2CCommTimeout_Enable(Timeout);
    I2CCommTimeout_ClearFlag();

    /* Initialize I2C Write */
    I2C_I2CMasterWriteBuf(SlaveAddr, SourceBuffer, DataCount, Mode);

    /* In a loop read the I2C Master status till one of these conditions occur
       1. There is an error in the transfer - like Slave NAK, lost arbitration etc
       2. Transfer completed successfully
       3. Transfer timed out    */
    do
    {
        I2CStatus = I2C_I2CMasterStatus();
    }
    while ((!(I2CStatus & I2C_I2C_MSTAT_WR_CMPLT)) && (!(TimeoutErrorFlags & I2C_TIMEOUT)) && (!(I2CStatus & I2C_I2C_MSTAT_ERR_XFER)));

    /* Disable I2C Timeout counter and clear the master status bits */
    Timeout_Stop();
    I2CCommTimeout_Disable();
    I2C_I2CMasterClearStatus();

    /* Check what caused us to exit the while loop before */
    /* If the write completed successfully, set status to success */
    if (I2CStatus & I2C_I2C_MSTAT_WR_CMPLT)
    {
        Status = COMM_SUCCESS;
    }
    /* If there was an error in transfer, set status to error */
    else if (I2CStatus & I2C_I2C_MSTAT_ERR_XFER)
    {
        Status = COMM_ERROR;
        // Do we stop and start I2C here?
    }
    /* If there was a timeout, set status to error */
    else if (TimeoutErrorFlags & I2C_TIMEOUT)
    {
        Status = COMM_ERROR;
        I2CCommTimeout_ClearFlag();
    }

    return Status;
}

/******************************************************************************
* Function Name: I2CComm_ReadBuffer
*******************************************************************************
*
* Summary:
*  This function reads from an I2C Slave.  After initializing an I2C read,
*  the function waits till the read completes successfully, or if a timeout or
*  error occurs.  The function returns the status of the transfer
*
*
* Parameters:
*  SlaveAddr - 7 bit Slave address
*  DestinationBuffer - Pointer to the buffer where the read bytes have to be saved
*  DataCount - Number of bytes to be read
*  Timeout - Timeout value in ms
*
* Return:
*  Status of the read
*    COMM_ERROR - If there is an error in the read
*    COMM_SUCCESS - If the read was successful
*
*******************************************************************************/
int16 I2CComm_ReadBuffer(uint8 SlaveAddr, uint8* DestinationBuffer, uint8 DataCount, uint8 Timeout, uint8 Mode)
{
    uint8 I2CStatus = 0;
    int16 Status = COMM_ERROR;

    /* Enable I2C timeout */
    Timeout_Start();
    I2CCommTimeout_Enable(Timeout);
    I2CCommTimeout_ClearFlag();

    /* Initialize I2C Read */
    I2C_I2CMasterReadBuf(SlaveAddr, DestinationBuffer, DataCount, Mode);

    /* In a loop read the I2C Master status till one of these conditions occur
       1. There is an error in the transfer - like Slave NAK, lost arbitration etc
       2. Transfer completed successfully
       3. Transfer timed out    */
    do
    {
        I2CStatus = I2C_I2CMasterStatus();
    }
    while ((!(I2CStatus & I2C_I2C_MSTAT_RD_CMPLT)) && (!(TimeoutErrorFlags & I2C_TIMEOUT)) && (!(I2CStatus & I2C_I2C_MSTAT_ERR_XFER)));

    /* Stop the timeout counter and clear the I2C status flags */
    Timeout_Stop();
    I2CCommTimeout_Disable();
    I2C_I2CMasterClearStatus();

    /* Check what caused us to exit the while loop before */
    /* If read completed successfully, set status to success */
    if (I2CStatus & I2C_I2C_MSTAT_RD_CMPLT)
    {
        Status = COMM_SUCCESS;
    }
    /* If there was an error in transfer, set status to error */
    else if (I2CStatus & I2C_I2C_MSTAT_ERR_XFER)
    {
        Status = COMM_ERROR;
        // Do we stop and start I2C here?
    }
    /* If there was a timeout, set status to error */
    else if (TimeoutErrorFlags & I2C_TIMEOUT)
    {
        Status = COMM_ERROR;
        I2CCommTimeout_ClearFlag();
    }

    /* Return the status */
    return Status;
}


int Sensors_I2C_WriteRegister(unsigned char slave_addr, unsigned char reg_addr, unsigned short len, unsigned char *data_ptr)
{
    int Status = COMM_ERROR;
    uint8 I2CBuffer[INV_MAX_SERIAL_WRITE + 1];
    uint8 i;
    uint8* ptr = data_ptr;

    /* Copy the address byte and the data bytes to the I2C buffer */
    I2CBuffer[0] = reg_addr;
    for (i = 0; i < len; i++)
    {
        I2CBuffer[i+1] = *ptr;
        ptr++;
    }

    /* Write the register address and data to the ICM20648 device */
    Status = I2CComm_WriteBuffer(slave_addr, I2CBuffer, (len+1), 25, I2C_I2C_MODE_COMPLETE_XFER);

//  /* Write the register address and data to the ICM20648 device */
//  Status = I2CComm_WriteBuffer(slave_addr, &reg_addr, 1, 25, I2C_I2C_MODE_COMPLETE_XFER);
//
//  /* Check if register address write is success. If yes write the data */
//  if(Status == COMM_SUCCESS)
//  {
//      Status = I2CComm_WriteBuffer(slave_addr, data_ptr, len, 20, I2C_I2C_MODE_COMPLETE_XFER);
//  }
//
    return Status;
}

int Sensors_I2C_ReadRegister(unsigned char slave_addr, unsigned char reg_addr, unsigned short len, unsigned char *data_ptr)
{
    int Status = COMM_ERROR;

    /* First write the register address to the ICM20648 device */
    Status = I2CComm_WriteBuffer(slave_addr, &reg_addr, 1, 5, I2C_I2C_MODE_NO_STOP);

    /* Check if register address write is success. If yes read the data */
    if (Status == COMM_SUCCESS)
    {
        Status = I2CComm_ReadBuffer(slave_addr, data_ptr, len, 20, I2C_I2C_MODE_REPEAT_START);
    }
    else
    {
        I2C_I2CMasterSendStop();
    }

    return Status;
}


/* [] END OF FILE */
