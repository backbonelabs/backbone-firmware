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
#include "cytypes.h"

/* Direct test mode instructions. */
#define CYBLE_DTM_TX_START                                                0x46u
#define CYBLE_DTM_RX_START                                                0x47u
#define CYBLE_DTM_STOP                                                    0x48u
#define CYBLE_DTM_CMPLT_BIT_MASK                                    0x01000000u
#define CYBLE_DTM_LEN_BIT_POS                                               10u
#define CYBLE_DTM_PAYLOAD_BIT_POS                                            7u
#define CYBLE_DTM_TX_FREQ_BIT_POS                                            0u

/*****************************************************************************
* DTM Function Prototypes
*****************************************************************************/
/* Direct Test Mode related LL hardware APIs */


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
);


/******************************************************************************
##Function Name: CyBle_DtmRxTest
*******************************************************************************

Summary:This function Enables DTM Rx Test for given parameters in BLESS

Parameters:
 rx_freq: Transmit_Freq index


Return: None


******************************************************************************/
void CyBle_DtmRxTest(/* IN */ uint8 rx_freq);

/******************************************************************************
##Function Name: CyBle_DtmStopTest
*******************************************************************************

Summary:This function Stops ongoing DTM Test in BLESS, and returns the # of
packets that are relevant for Rx.

Parameters:
 rx_freq: Transmit_Freq index


Return: Numbers of packets received.


******************************************************************************/

uint16 CyBle_DtmStopTest(void);



uint8_t Anritsu();
/* [] END OF FILE */
