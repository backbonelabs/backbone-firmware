/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
********************************************************************************
* Copyright 2012, Cypress Semiconductor Corporation. All rights reserved.
* This software is owned by Cypress Semiconductor Corporation and is protected
* by and subject to worldwide patent and copyright laws and treaties.
* Therefore, you may use this software only as provided in the license agreement
* accompanying the software package from which you obtained this software.
* CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

#include <device.h>
#include "stdio.h"

#if defined (__GNUC__)
    /* Add an explicit reference to the floating point printf library */
    /* to allow the usage of floating point conversion specifiers. */
    /* This is not linked in by default with the newlib-nano library. */
    asm (".global _printf_float");
#endif

/* The size of the buffer is equal to maximum packet size of the
*  IN and OUT bulk endpoints.
*/
#define BUFFER_LEN  64u

char8 *parity[] = { "None", "Odd", "Even", "Mark", "Space" };
char8 *stop[] = { "1", "1.5", "2" };
char8 DUT_Sel= '0';
void SetHiZAnalog_Mode()
{
    Pin_1_SetDriveMode(Pin_1_DM_ALG_HIZ);
    Pin_2_SetDriveMode(Pin_2_DM_ALG_HIZ);
    Pin_3_SetDriveMode(Pin_3_DM_ALG_HIZ);
    Pin_4_SetDriveMode(Pin_4_DM_ALG_HIZ);
    Pin_5_SetDriveMode(Pin_5_DM_ALG_HIZ);
    Pin_6_SetDriveMode(Pin_6_DM_ALG_HIZ);
    Pin_7_SetDriveMode(Pin_7_DM_ALG_HIZ);
    Pin_8_SetDriveMode(Pin_8_DM_ALG_HIZ);
    Pin_9_SetDriveMode(Pin_9_DM_ALG_HIZ);
    Pin_10_SetDriveMode(Pin_10_DM_ALG_HIZ);
    Pin_11_SetDriveMode(Pin_11_DM_ALG_HIZ);
    Pin_12_SetDriveMode(Pin_12_DM_ALG_HIZ);
    Pin_13_SetDriveMode(Pin_13_DM_ALG_HIZ);
    Pin_14_SetDriveMode(Pin_14_DM_ALG_HIZ);
    Pin_15_SetDriveMode(Pin_15_DM_ALG_HIZ);
    Pin_16_SetDriveMode(Pin_16_DM_ALG_HIZ);
    Pin_17_SetDriveMode(Pin_17_DM_ALG_HIZ);
    Pin_18_SetDriveMode(Pin_18_DM_ALG_HIZ);
    Pin_19_SetDriveMode(Pin_19_DM_ALG_HIZ);
    Pin_20_SetDriveMode(Pin_20_DM_ALG_HIZ);
    Pin_21_SetDriveMode(Pin_21_DM_ALG_HIZ);
    Pin_22_SetDriveMode(Pin_22_DM_ALG_HIZ);
    Pin_23_SetDriveMode(Pin_23_DM_ALG_HIZ);
    Pin_24_SetDriveMode(Pin_24_DM_ALG_HIZ);
    Pin_25_SetDriveMode(Pin_25_DM_ALG_HIZ);
    Pin_26_SetDriveMode(Pin_26_DM_ALG_HIZ);
    Pin_27_SetDriveMode(Pin_27_DM_ALG_HIZ);
    Pin_28_SetDriveMode(Pin_28_DM_ALG_HIZ);
    Pin_29_SetDriveMode(Pin_29_DM_ALG_HIZ);
    Pin_30_SetDriveMode(Pin_30_DM_ALG_HIZ);
    Pin_31_SetDriveMode(Pin_31_DM_ALG_HIZ);
    Pin_32_SetDriveMode(Pin_32_DM_ALG_HIZ);
    //Anritsu side no need to worry
//    Pin_33_SetDriveMode(Pin_33_DM_ALG_HIZ);
//    Pin_34_SetDriveMode(Pin_34_DM_ALG_HIZ);

}

int main()
{
    uint16 count;
    uint8 buffer[BUFFER_LEN];
    char8 lineStr[20];
    uint8 state;

    /* Enable Global Interrupts */
    CyGlobalIntEnable;

    /* Start USBFS Operation with 3V operation */
    USBUART_1_Start(0u, USBUART_1_5V_OPERATION);

    SetHiZAnalog_Mode();

    /* Main Loop: */
    for (;;)
    {
        if (USBUART_1_IsConfigurationChanged() != 0u) /* Host could send double SET_INTERFACE request */
        {
            if (USBUART_1_GetConfiguration() != 0u)  /* Init IN endpoints when device configured */
            {
                /* Enumeration is done, enable OUT endpoint for receive data from Host */
                USBUART_1_CDC_Init();
            }
        }
        if (USBUART_1_GetConfiguration() != 0u)   /* Service USB CDC when device configured */
        {
            if (USBUART_1_DataIsReady() != 0u)              /* Check for input data from PC */
            {
                count = USBUART_1_GetAll(buffer);           /* Read received data and re-enable OUT endpoint */


                if (count != 0u)
                {
                    DUT_Sel = buffer[0];
                    SetHiZAnalog_Mode(); // Put all the GPIOs in teh HIGH Z state

                    switch (DUT_Sel)
                    {
                        case '0':
                            Pin_1_SetDriveMode(Pin_1_DM_STRONG);
                            Pin_2_SetDriveMode(Pin_2_DM_DIG_HIZ);
                            Control_Reg_1_Write(0);

                            break;
                        case '1':
                            Pin_3_SetDriveMode(Pin_3_DM_STRONG);
                            Pin_4_SetDriveMode(Pin_4_DM_DIG_HIZ);
                            Control_Reg_1_Write(1);

                            break;
                        case '2':
                            Pin_5_SetDriveMode(Pin_5_DM_STRONG);
                            Pin_6_SetDriveMode(Pin_6_DM_DIG_HIZ);
                            Control_Reg_1_Write(2);

                            break;
                        case '3':
                            Pin_7_SetDriveMode(Pin_7_DM_STRONG);
                            Pin_8_SetDriveMode(Pin_8_DM_DIG_HIZ);
                            Control_Reg_1_Write(3);

                            break;
                        case '4':
                            Pin_9_SetDriveMode(Pin_9_DM_STRONG);
                            Pin_10_SetDriveMode(Pin_10_DM_DIG_HIZ);
                            Control_Reg_1_Write(4);

                            break;
                        case '5':
                            Pin_11_SetDriveMode(Pin_11_DM_STRONG);
                            Pin_12_SetDriveMode(Pin_12_DM_DIG_HIZ);
                            Control_Reg_1_Write(5);

                            break;
                        case '6':
                            Pin_13_SetDriveMode(Pin_13_DM_STRONG);
                            Pin_14_SetDriveMode(Pin_14_DM_DIG_HIZ);
                            Control_Reg_1_Write(6);

                            break;
                        case '7':
                            Pin_15_SetDriveMode(Pin_15_DM_STRONG);
                            Pin_16_SetDriveMode(Pin_16_DM_DIG_HIZ);
                            Control_Reg_1_Write(7);

                            break;
                        case '8':
                            Pin_17_SetDriveMode(Pin_17_DM_STRONG);
                            Pin_18_SetDriveMode(Pin_18_DM_DIG_HIZ);
                            Control_Reg_1_Write(8);

                            break;
                        case '9':
                            Pin_19_SetDriveMode(Pin_19_DM_STRONG);
                            Pin_20_SetDriveMode(Pin_20_DM_DIG_HIZ);
                            Control_Reg_1_Write(9);
                            break;
                        case 'a':
                        case 'A':
                            Pin_21_SetDriveMode(Pin_21_DM_STRONG);
                            Pin_22_SetDriveMode(Pin_22_DM_DIG_HIZ);
                            Control_Reg_1_Write(10);
                            break;
                        case 'b':
                        case 'B':
                            Pin_23_SetDriveMode(Pin_23_DM_STRONG);
                            Pin_24_SetDriveMode(Pin_24_DM_DIG_HIZ);
                            Control_Reg_1_Write(11);
                            break;
                        case 'c':
                        case 'C':
                            Pin_25_SetDriveMode(Pin_25_DM_STRONG);
                            Pin_26_SetDriveMode(Pin_26_DM_DIG_HIZ);
                            Control_Reg_1_Write(12);

                            break;
                        case 'd':
                        case 'D':
                            Pin_27_SetDriveMode(Pin_27_DM_STRONG);
                            Pin_28_SetDriveMode(Pin_28_DM_DIG_HIZ);
                            Control_Reg_1_Write(13);
                            break;
                        case 'e':
                        case 'E':
                            Pin_29_SetDriveMode(Pin_29_DM_STRONG);
                            Pin_30_SetDriveMode(Pin_30_DM_DIG_HIZ);
                            Control_Reg_1_Write(14);
                            break;
                        case 'f':
                        case 'F':
                            Pin_31_SetDriveMode(Pin_31_DM_STRONG);
                            Pin_32_SetDriveMode(Pin_32_DM_DIG_HIZ);
                            Control_Reg_1_Write(15);
                            break;

                        default:
                            SetHiZAnalog_Mode();
                            buffer[0]= 'g';

                            break;

                    }


                }


                CyDelayUs(500);

                if (count != 0u)
                {
                    while (USBUART_1_CDCIsReady() == 0u);   /* Wait till component is ready to send more data to the PC */
                    USBUART_1_PutData(buffer, count);       /* Send data back to PC */
                    /* If the last sent packet is exactly maximum packet size,
                    *  it shall be followed by a zero-length packet to assure the
                    *  end of segment is properly identified by the terminal.
                    */
                    if (count == BUFFER_LEN)
                    {
                        while (USBUART_1_CDCIsReady() == 0u); /* Wait till component is ready to send more data to the PC */
                        USBUART_1_PutData(NULL, 0u);         /* Send zero-length packet to PC */
                    }
                }
            }

            state = USBUART_1_IsLineChanged();              /* Check for Line settings changed */
            if (state != 0u)
            {
                if (state & USBUART_1_LINE_CODING_CHANGED)  /* Show new settings */
                {
                    sprintf(lineStr,"BR:%4ld,DB:%d",USBUART_1_GetDTERate(),(uint16)USBUART_1_GetDataBits());
                    sprintf(lineStr,"SB:%s,Parity:%s", stop[(uint16)USBUART_1_GetCharFormat()], \
                            parity[(uint16)USBUART_1_GetParityType()]);

                }
                if (state & USBUART_1_LINE_CONTROL_CHANGED) /* Show new settings */
                {
                    state = USBUART_1_GetLineControl();
                    sprintf(lineStr,"DTR:%s,RTS:%s",  (state & USBUART_1_LINE_CONTROL_DTR) ? "ON" : "OFF", \
                            (state & USBUART_1_LINE_CONTROL_RTS) ? "ON" : "OFF");

                }
            }
        }
    }
}


/* [] END OF FILE */
