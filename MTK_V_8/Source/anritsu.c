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
#include <stdbool.h>
#include "anritsu.h"
#include "CyBLE_MTK.h"

/*****************************************************************************
* MACRO Definition
*****************************************************************************/
#define EVT_STATUS_SUCCESS 0u
#define EVT_STATUS_FAIL    1u
#define STATUS_EVT_CODE    0u
#define REPORT_EVT_MASK    0x80u

#define CMD_CODE_MASK      (uint8)0xC0u
#define CHNL_FREQ_MASK     (uint8)0x3Fu

#define CMD_CODE_SHIFT     6u
#define CHNL_FREQ_LIMIT    0x28u

/*Packet payload parser macros*/
#define PL_TYPE_MASK       (uint8)0x03u
#define PL_LEN_MASK        (uint8)0xFCu
#define PL_LEN_SHIFT       2u
#define PL_LEN_LIMIT       0x26u

/*****************************************************************************
* Data Type Definition
*****************************************************************************/


/*****************************************************************************
* Enumerated Data Definition
*****************************************************************************/
/*DTM Command code*/
typedef enum
{
    DTM_RESET = 0,
    DTM_RX_TEST,
    DTM_TX_TEST,
    DTM_END,
    DTM_INVALID,
    MTK_MOD_SEL = 0xAB,
} DTM_Cmd_Code;

/*DTM Payload type*/
typedef enum
{
    DTM_PKT_TYPE_PRBS9 = 0,
    DTM_PKT_TYPE_11110000,
    DTM_PKT_TYPE_10101010,
    DTM_PKT_TYPE_VENDOR,
} DTM_PKT_PL_TYPE;

/*****************************************************************************
* Data Struct Definition
*****************************************************************************/

/*DTM Command fields*/
typedef struct _DTM_Cmd_Packet
{
    DTM_Cmd_Code Cmd_Code;
    uint8 Channel;
    uint8 Payload_Len;
    DTM_PKT_PL_TYPE Payload_Type;
} DTM_Cmd_Packet;

typedef unsigned char          UCHAR;
typedef unsigned int          UINT16;

/*****************************************************************************
* Global Variable Declaration
*****************************************************************************/
volatile bool DTMCmdDetected = false, DTMInvalidCmd = false;
DTM_Cmd_Packet CurrentCmd = {DTM_INVALID, 0, 0, 0};


/*Dummy event callback, all the DTM events handled by the stack*/
static void DtmEventHandler(uint32 event, void * eventparam)
{
    (void)event;
    (void)eventparam;
}


bool checkMSByte = false;





/******************************************************************************
##Function Name: CyBle_DtmTxTest
*******************************************************************************

Summary:This function Enables DTM Tx Test for given parameters in BLESS

Parameters:
 length: Length of DTM packet
 payload_type: Pattern of the DTM value
 tx_freq: Transmit_Freq index


Return: None


******************************************************************************/

void CyBle_DtmTxTest
(
    /* IN */ uint8 length,
    /* IN */ uint8 payload_type,
    /* IN */ uint8 tx_freq
)
{
    uint16 temp_var;

    temp_var =
        ((uint16)length << CYBLE_DTM_LEN_BIT_POS) |
        ((uint16)payload_type << CYBLE_DTM_PAYLOAD_BIT_POS) |
        ((uint16)tx_freq << CYBLE_DTM_TX_FREQ_BIT_POS);

    CY_SET_REG32(CYREG_BLE_BLELL_LE_RF_TEST_MODE, temp_var);
    CY_SET_REG32(CYREG_BLE_BLELL_COMMAND_REGISTER, CYBLE_DTM_TX_START);
}

/******************************************************************************
##Function Name: CyBle_DtmRxTest
*******************************************************************************

Summary:This function Enables DTM Rx Test for given parameters in BLESS

Parameters:
 rx_freq: Transmit_Freq index


Return: None


******************************************************************************/
void CyBle_DtmRxTest(/* IN */ uint8 rx_freq)
{

    CY_SET_REG32(CYREG_BLE_BLELL_LE_RF_TEST_MODE, rx_freq);
    CY_SET_REG32(CYREG_BLE_BLELL_COMMAND_REGISTER, CYBLE_DTM_RX_START);

}

/******************************************************************************
##Function Name: CyBle_DtmStopTest
*******************************************************************************

Summary:This function Stops ongoing DTM Test in BLESS, and returns the # of
packets that are relevant for Rx.

Parameters:
 rx_freq: Transmit_Freq index


Return: Numbers of packets received.


******************************************************************************/
uint16 CyBle_DtmStopTest(void)
{
    uint16 temp_var;

    CY_SET_REG32(CYREG_BLE_BLELL_COMMAND_REGISTER, CYBLE_DTM_STOP);

    /* Read the number of packets. */
    temp_var = (uint16)CY_GET_REG32(CYREG_BLE_BLELL_LE_RF_TEST_MODE);
    while ( 0x00 != (temp_var & CYBLE_DTM_CMPLT_BIT_MASK))
    {
        temp_var = (uint16)CY_GET_REG32(CYREG_BLE_BLELL_LE_RF_TEST_MODE);
    }

    return (uint16)CY_GET_REG32(CYREG_BLE_BLELL_DTM_RX_PKT_COUNT);
}
/* [] END OF FILE */



/*******************************************************************************
* Define Interrupt service routine and allocate an vector to the Interrupt
* Fix for the Multiplexer itermittent interrupt bug
********************************************************************************/
CY_ISR(Isr_200ms)
{
    /* Check interrupt source and clear Inerrupt */
    checkMSByte = false;
    Timer_1_ClearInterrupt(Timer_1_INTR_MASK_TC);
    Timer_1_Stop();

}


uint8 MTK_MOD_byte1 = 0x00;
uint8 MTK_MOD_byte2 = 0x00;

CY_ISR(ISR_2_Wire_Cmd_Decoder)
{


    if (UART_CHECK_INTR_RX_MASKED(UART_INTR_RX_NOT_EMPTY))
    {
        uint8 byte = (uint8)UART_SpiUartReadRxData();


        if (checkMSByte)
        {
            MTK_MOD_byte2 = byte;
            CurrentCmd.Payload_Type = byte & PL_TYPE_MASK;
            CurrentCmd.Payload_Len = (byte & PL_LEN_MASK) >> PL_LEN_SHIFT;
            if (PL_LEN_LIMIT > CurrentCmd.Payload_Len)
            {
                /*Both the bytes decoded, reset the decoder state*/
                DTMCmdDetected = true;
            }
            else
            {
                DTMInvalidCmd = true;
            }
            checkMSByte = false;

            Timer_1_Stop();



        }
        else
        {
            MTK_MOD_byte1 = byte;
            CurrentCmd.Channel = byte & CHNL_FREQ_MASK;
            CurrentCmd.Cmd_Code = (byte & CMD_CODE_MASK) >> CMD_CODE_SHIFT;
            if (CHNL_FREQ_LIMIT > CurrentCmd.Channel)
            {
                /*LSByte detected, Process the MSByte*/
                checkMSByte = true;
            }
            else
            {
                DTMInvalidCmd = true;
            }

            TC_CC_ISR_StartEx(Isr_200ms);
            Timer_1_Start();
        }

        UART_ClearRxInterruptSource(UART_INTR_RX_NOT_EMPTY);
    }
}



uint8_t Anritsu()
{
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_Start();
    UART_SetCustomInterruptHandler(ISR_2_Wire_Cmd_Decoder);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_BB_XO_CAPTRIM), CAP_TRIM);
    /* Enable the Interrupt component connected to interrupt */


    /* Start the components */

    CyBle_Start(DtmEventHandler);

    CyGlobalIntEnable;



    for (;;)
    {
        uint8 DTM_Event[2] = {0,0};

        CyBle_ProcessEvents();

        if (DTMCmdDetected)
        {
            /* Process the DTM Command */
            switch (CurrentCmd.Cmd_Code)
            {
                case DTM_RESET:
                    CyBle_Stop();
                    CyBle_Start(DtmEventHandler);
                    /*send the DTM packet status event*/
                    DTM_Event[1] = EVT_STATUS_SUCCESS; //SUCCESS
                    DTM_Event[0] = STATUS_EVT_CODE; //event code

                    UART_SpiUartPutArray(DTM_Event, sizeof(DTM_Event));
                    break;

                case DTM_RX_TEST:
                    CyBle_DtmRxTest(CurrentCmd.Channel);
                    /*send the DTM packet status event*/
                    DTM_Event[1] = EVT_STATUS_SUCCESS; //SUCCESS
                    DTM_Event[0] = STATUS_EVT_CODE; //event code

                    UART_SpiUartPutArray(DTM_Event, sizeof(DTM_Event));
                    break;

                case DTM_TX_TEST:
                    CyBle_DtmTxTest(CurrentCmd.Payload_Len,
                                    CurrentCmd.Payload_Type, CurrentCmd.Channel);
                    /*send the DTM packet status event*/
                    DTM_Event[1] = EVT_STATUS_SUCCESS; //SUCCESS
                    DTM_Event[0] = STATUS_EVT_CODE; //event code

                    UART_SpiUartPutArray(DTM_Event, sizeof(DTM_Event));
                    break;

                case DTM_END:
                {
                    uint16 value =  CyBle_DtmStopTest();
                    DTM_Event[1u] = (uint8) value;
                    DTM_Event[0u] = (uint8) (value >> 8u);

                    DTM_Event[0] |= REPORT_EVT_MASK; //Set the report event code

                    /*send the DTM packet Report event*/
                    UART_SpiUartPutArray(DTM_Event, sizeof(DTM_Event));
                }
                break;
                default:
                    /*Invalid test*/
                    /*send the DTM packet status failure event*/
                    DTM_Event[1] = EVT_STATUS_FAIL; //FAIL
                    DTM_Event[0] = STATUS_EVT_CODE; //event code

                    UART_SpiUartPutArray(DTM_Event, sizeof(DTM_Event));
                    break;
            }
            DTMCmdDetected = false;
            CurrentCmd.Cmd_Code = DTM_INVALID;
            CurrentCmd.Channel = 0;
            CurrentCmd.Payload_Len = 0;
            CurrentCmd.Payload_Type = 0;
        }
        /*send the DTM packet status failure event, if wrong command is detected*/
        if ((MTK_MOD_byte1 == 'A') && (MTK_MOD_byte2 == 'B'))
        {
            //Send back the same command to confirm
            UART_SpiUartPutArray(&MTK_MOD_byte1, sizeof(MTK_MOD_byte1));
            CyDelayUs(200);
            UART_SpiUartPutArray(&MTK_MOD_byte2,sizeof(MTK_MOD_byte2));
            CyDelayUs(200);
            UART_Stop();
            CyBle_Stop();
            MTK_MOD_byte1 = 0;
            MTK_MOD_byte2 =  0;
            return MTK_MOD_SEL;

        }
        else if (DTMInvalidCmd)
        {
            DTMInvalidCmd = false;
            DTM_Event[1] = EVT_STATUS_FAIL; //FAIL
            DTM_Event[0] = STATUS_EVT_CODE; //event code

            UART_SpiUartPutArray(DTM_Event, sizeof(DTM_Event));
        }



    }
}

/* [] END OF FILE */
