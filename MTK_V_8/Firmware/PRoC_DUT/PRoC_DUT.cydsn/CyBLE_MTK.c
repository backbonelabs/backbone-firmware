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
#include "options.h"

#ifdef ENABLE_MTK

#include "uart_cmd_int.h"
#include "stdio.h"
#include "CyBLE_MTK.h"
#include "stdbool.h"
#include "custom_MTK_commands.h"

#if (!defined(CYBLE_INTERFACE_ENABLED) || (CYBLE_INTERFACE_ENABLED == 0)) && (!defined(UART_INTERFACE_ENABLED) || (UART_INTERFACE_ENABLED == 0))
#error "No input method selected! Please select at least one input method for MTK."
#define NO_INPUT_ENABLED            1
#endif

#define BDA_LENGTH          6
#define POSTFILT_VALUE      8

static uint32_t MTK_packet_length, MTK_packet_type;
static enum UART_cmds command = NO_CMD, prev_command;
static int32_t cmd_args[MAX_CMD_ARG_LEN], prev_cmd_args[MAX_CMD_ARG_LEN];
static bool DTM_test_inprogress = false;
static bool rx_count_read = false, WBAResult = false, RBAResult = false;
static bool STRResult = false, LTRResult = false;
static uint8 BDAddress[BDA_LENGTH];
static int32 RSSIValue = 0;

#if (CYBLE_INTERFACE_ENABLED == 1) || (CYBLE_TESTS_ENABLED == 1)
static uint8 num_L2Cap_retries = 0;
static bool is_L2CAP_enabled = false;
static uint16 MTK_local_CID;
#endif // CYBLE_INTERFACE_ENABLED || CYBLE_TESTS_ENABLED

#if (CYBLE_TESTS_ENABLED == 1)
static int32_t packet_count;
#endif

#if (CYBLE_INTERFACE_ENABLED == 1)
static int32_t DTM_RX_end_delay;
#endif // CYBLE_INTERFACE_ENABLED

#if (CYBLE_MTK_HOST == 1) || ((CYBLE_INTERFACE_ENABLED == 0) && (UART_INTERFACE_ENABLED == 1))
static uint32_t msCount, time_elapsed;
#endif // CYBLE_MTK_HOST

#if (CYBLE_MTK_HOST == 1) && ((CYBLE_INTERFACE_ENABLED == 1) || (CYBLE_TESTS_ENABLED == 1))
static bool is_L2CAP_connecting = false, data_received = false;
static bool redirect_to_DUT = false/*, data_write_complete = false*/;
static CYBLE_GAP_BD_ADDR_T temp;
static uint32_t RRS_result;
#endif // CYBLE_MTK_HOST && CYBLE_INTERFACE_ENABLED

#if ((CYBLE_MTK_HOST == 1) && ((CYBLE_INTERFACE_ENABLED == 1) || (CYBLE_TESTS_ENABLED == 1)) || ((CYBLE_MTK_DUT == 1) || (CYBLE_TESTS_ENABLED == 1)))
static bool data_write_complete = false;
#endif // CYBLE_MTK_HOST && CYBLE_INTERFACE_ENABLED

#if (CYBLE_MTK_DUT == 1) && ((CYBLE_INTERFACE_ENABLED == 1) || (CYBLE_TESTS_ENABLED == 1))
static uint8_t BLE_arg_array[14];
#endif // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED

#if (CYBLE_MTK_DUT == 1) && ((CYBLE_TESTS_ENABLED == 1))
static uint8_t TX_buffer[21];
#endif // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED

#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
static uint16_t tx_count = 0;
#endif // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED

static int32_t DTM_TX_end_delay;
static uint16 rx_count = 0;
static volatile bool new_cmd_flag;
static uint32_t msCount_backup;
static uint32_t BLE_BLELL_RECEIVE_TRIG_CTRL_backup;
static uint32_t BLE_BLELL_COMMAND_REGISTER_backup;
static uint32_t BLE_BLELL_LE_RF_TEST_MODE_backup;
static uint32_t BLE_BLERD_CFG2_backup;
static uint32_t BLE_BLERD_CFGCTRL_backup;
static uint32_t BLE_BLERD_SY_backup;
static uint32_t BLE_BLERD_DBUS_backup;
static uint32_t BLE_BLERD_CFG1_backup;

static void set_power(CYBLE_BLESS_PWR_LVL_T power);
static CYBLE_BLESS_PWR_LVL_T translate_power(int32_t power);

static int RX_Count;
//static void UART_print_int(int32_t input);

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void clear_timer_interrupt()
{
    Timer_ClearInterrupt(Timer_INTR_MASK_TC);
    Timer_ISR_ClearPending();
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
void timer_ISR()
{
    if (prev_cmd_args[2] == -1)
    {
        clear_timer_interrupt();
        return;
    }
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_COMMAND_REGISTER),0x48); //setting it to idle
    clear_timer_interrupt();
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
void BLE_call_back(uint32 event, void* eventParam)
{
#if (CYBLE_INTERFACE_ENABLED == 1) || (CYBLE_TESTS_ENABLED == 1)
#if (CYBLE_MTK_HOST == 1)
    static bool is_scanning_stopped = false;
#endif // CYBLE_MTK_HOST

    if(event == CYBLE_EVT_L2CAP_CBFC_CONN_IND)
    {
        if(!is_L2CAP_enabled)
        {
            CYBLE_L2CAP_CBFC_CONNECT_PARAM_T connParam;
            connParam.mtu = MTK_L2CAP_MTU_SIZE;
            connParam.mps = MTK_L2CAP_MPS_SIZE;
            connParam.credit = MTK_L2CAP_CREDIT;
            MTK_local_CID = ((CYBLE_L2CAP_CBFC_CONN_IND_PARAM_T *)eventParam)->lCid;
            is_L2CAP_enabled = true;
            RX_Count = 0;
            CyBle_L2capCbfcConnectRsp(MTK_local_CID, CYBLE_L2CAP_CONNECTION_SUCCESSFUL, &connParam);
            num_L2Cap_retries = 0;
        }
    }
#if (CYBLE_MTK_HOST == 1)
    else if(event == CYBLE_EVT_L2CAP_CBFC_CONN_CNF)
    {
        is_L2CAP_connecting = false;
        if(!(((CYBLE_L2CAP_CBFC_CONN_CNF_PARAM_T *)eventParam)->response))
        {
            num_L2Cap_retries = 0;
            MTK_local_CID = ((CYBLE_L2CAP_CBFC_CONN_CNF_PARAM_T *)eventParam)->lCid;
            is_L2CAP_enabled = true;
            RX_Count = 0;
            Green_LED_Write(0);
            Red_LED_Write(1);
        }
    }
#endif // CYBLE_MTK_HOST
    else if(event == CYBLE_EVT_L2CAP_CBFC_DATA_READ)
    {
#if (CYBLE_MTK_HOST == 1) || (CYBLE_MTK_DUT == 1)
        uint16_t i;
#endif // CYBLE_MTK_HOST || CYBLE_MTK_DUT

#if (CYBLE_MTK_HOST == 1)
        uint8_t *temp;
#endif // CYBLE_MTK_HOST

#if (CYBLE_MTK_DUT == 1) || (CYBLE_MTK_HOST == 1)
        CYBLE_L2CAP_CBFC_RX_PARAM_T *rxDataParam = (CYBLE_L2CAP_CBFC_RX_PARAM_T*)eventParam;
#endif // CYBLE_MTK_DUT || CYBLE_MTK_HOST

#if (CYBLE_MTK_HOST == 1)
        data_received = true;
        if (rxDataParam->rxDataLength <= 4)
        {
            temp = (uint8_t *)&RRS_result;
            for (i = 0; i < rxDataParam->rxDataLength; i++)
            {
                temp[i] = rxDataParam->rxData[i];
            }
        }
#endif // CYBLE_MTK_HOST

#if (CYBLE_MTK_DUT == 1)
        if (rxDataParam->rxDataLength == 14)
        {
            new_cmd_flag = true;
            for (i = 0; i < 14; i++)
            {
                BLE_arg_array[i] = rxDataParam->rxData[i];
            }
        }
#endif // CYBLE_MTK_DUT
    }
    else if (event == CYBLE_EVT_L2CAP_CBFC_DISCONN_IND)
    {
        if(MTK_local_CID == *(uint16 *)eventParam)
        {
            is_L2CAP_enabled = false;

#if (CYBLE_MTK_HOST == 1)
            Green_LED_Write(1);
            redirect_to_DUT = false;
            Blue_LED_Write(1);
#endif // CYBLE_MTK_HOST
        }
    }
#if (CYBLE_MTK_HOST == 1) || ((CYBLE_TESTS_ENABLED == 1) && (CYBLE_MTK_DUT == 1))
    else if (event == CYBLE_EVT_L2CAP_CBFC_DATA_WRITE_IND)
    {
        data_write_complete = true;
#if ((CYBLE_TESTS_ENABLED == 1) && (CYBLE_MTK_DUT == 1))
        if (((packet_count < prev_cmd_args[0]) && (prev_command == STC)) ||
            ((prev_cmd_args[0] == -1) && (prev_command == STC)))
        {
            packet_count++;
        }
#endif // CYBLE_TESTS_ENABLED && (CYBLE_MTK_DUT
    }
#endif // CYBLE_TESTS_ENABLED
#if (CYBLE_MTK_HOST == 1)
    else if (event == CYBLE_EVT_L2CAP_CBFC_DISCONN_CNF)
    {
//        if(((CYBLE_L2CAP_CBFC_DISCONN_CNF_PARAM_T *)eventParam)->lCid == MTK_local_CID && !((CYBLE_L2CAP_CBFC_DISCONN_CNF_PARAM_T *)eventParam)->result)
        {
            is_L2CAP_enabled = false;
            Green_LED_Write(1);
            Blue_LED_Write(1);
        }
    }
    else if (event == CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT)
    {
        uint8* addr = ((CYBLE_GAPC_ADV_REPORT_T *)eventParam)->peerBdAddr;
        if ((addr[0] == 0x11) && (addr[1] == 0x22) && (addr[2] == 0x33)  && (addr[3] == 0x44)&& (addr[5] == 0x66) && (addr[4] == 0x55))
        {
            CyBle_GapcStopScan();
            memcpy(temp.bdAddr, addr, 6);
            temp.type = ((CYBLE_GAPC_ADV_REPORT_T *)eventParam)->peerAddrType;
            is_scanning_stopped = true;
        }
    }
    else if (event == CYBLE_EVT_GAPC_SCAN_START_STOP)
    {
        if ((is_scanning_stopped == true) && (*(uint8_t *)eventParam == 0x00))
            CyBle_GapcConnectDevice(&temp);
    }
    else if (event == CYBLE_EVT_L2CAP_CBFC_RX_CREDIT_IND)
    {
        CyBle_L2capCbfcSendFlowControlCredit(MTK_local_CID, MTK_L2CAP_CREDIT);
    }
#endif // CYBLE_MTK_HOST
#endif // CYBLE_INTERFACE_ENABLED
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
#if (CYBLE_MTK_DUT == 1) && ((CYBLE_INTERFACE_ENABLED == 1) || (CYBLE_TESTS_ENABLED == 1))
uint8_t BLE_get_command(int32_t *argument_array, int32_t *num_args)
{
    uint8_t *tmp, i;
    if (new_cmd_flag == true)
    {
        new_cmd_flag = false;
        *num_args = BLE_arg_array[1];
        for (i = 0; i < *num_args; i++)
        {
            tmp = (uint8_t *)&argument_array[i];
            tmp[0] = BLE_arg_array[(i * 4) + 2];
            tmp[1] = BLE_arg_array[(i * 4) + 3];
            tmp[2] = BLE_arg_array[(i * 4) + 4];
            tmp[3] = BLE_arg_array[(i * 4) + 5];
        }
        return BLE_arg_array[0];
    }

    return NO_CMD;
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void Ble_Set_Address(uint8 *address)
{
    CYBLE_GAP_BD_ADDR_T bdAddr;

    bdAddr.bdAddr[0] = *address++;
    bdAddr.bdAddr[1] = *address++;
    bdAddr.bdAddr[2] = *address++;
    bdAddr.bdAddr[3] = *address++;
    bdAddr.bdAddr[4] = *address++;
    bdAddr.bdAddr[5] = *address++;
    bdAddr.type = 0;

    CyBle_SetDeviceAddress(&bdAddr);
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void Ble_StopAdvertisement()
{
    /* Stop directed advertisement and start again */
    if (CyBle_GetState() == BLE_STATE_ADVERTISING)
    {
        CyBle_GappStopAdvertisement();
        while (CyBle_GetState() == BLE_STATE_ADVERTISING)
        {
            CyBle_ProcessEvents();
        }
    }
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static bool Ble_StartAdvertisement()
{
    CyBle_ProcessEvents();

    if ((is_L2CAP_enabled == false) && (CyBle_GetState() == BLE_STATE_DISCONNECTED))
    {
        cyBle_discoveryModeInfo.advParam->advType = CYBLE_GAPP_CONNECTABLE_UNDIRECTED_ADV;

        CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        CyBle_ProcessEvents();
    }
    return (CyBle_GetState() == BLE_STATE_ADVERTISING);
}
#endif  // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED


/************************************************************************************************
 *
 *
 ************************************************************************************************/
#if (CYBLE_MTK_HOST == 1) && (CYBLE_INTERFACE_ENABLED == 1)
static void Ble_StopScan(void)
{
    if (CyBle_GetState() == CYBLE_STATE_SCANNING)
    {
        do
        {
            CyBle_ProcessEvents();
            CyBle_GapcStopScan();
        }
        while (CyBle_GetState() == CYBLE_STATE_SCANNING);
    }
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void Ble_Disconnect_L2CAP_Channel()
{
    if (is_L2CAP_enabled == true)
    {
        CyBle_L2capDisconnectReq(MTK_local_CID);
        do
        {
            CyBle_ProcessEvents();
            if (CyBle_GetState() == CYBLE_STATE_DISCONNECTED)
            {
                is_L2CAP_enabled = false;
                return;
            }
        } while(is_L2CAP_enabled);

        CyBle_GapDisconnect(cyBle_connHandle.bdHandle);
        do
        {
            CyBle_ProcessEvents();
        }while((CyBle_GetState() != CYBLE_STATE_DISCONNECTED));
    }
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void Ble_StartScan(void)
{
    if ((is_L2CAP_enabled == false) && (is_L2CAP_connecting == false) && (CyBle_GetState() == CYBLE_STATE_DISCONNECTED))
    {
        CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
        CyBle_ProcessEvents();
    }
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void Ble_Connect_L2CAP_Channel(void)
{
    CYBLE_L2CAP_CBFC_CONNECT_PARAM_T param = {MTK_L2CAP_MTU_SIZE, MTK_L2CAP_MPS_SIZE, MTK_L2CAP_CREDIT};

    if((CyBle_GetState() == CYBLE_STATE_CONNECTED) && (!is_L2CAP_enabled) && (!is_L2CAP_connecting))
    {
        CyBle_L2capCbfcConnectReq(cyBle_connHandle.bdHandle, MTK_L2CAP_REMOTE_PSM_ID, MTK_L2CAP_PSM_ID, &param);
        CyBle_ProcessEvents();
        is_L2CAP_connecting = true;
    }
}
#endif  // CYBLE_MTK_HOST

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void initialize_BLE_RF(bool reinitialize)
{
    if (reinitialize == true)
        CyBle_Stop();

    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_BB_XO_CAPTRIM), CAP_TRIM); //
    CyBle_Start(BLE_call_back);
    do
    {
        CyBle_ProcessEvents();
    }while(!(CyBle_GetState() != CYBLE_STATE_INITIALIZING));
    set_power(translate_power(3));
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void initialize_BLE(bool reinitialize)
{
#if (CYBLE_MTK_DUT == 1) && ((CYBLE_INTERFACE_ENABLED == 1) || (CYBLE_TESTS_ENABLED == 1))
    uint8_t ad[] = MTK_DUT_ADDRESS;
#endif  // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED

    initialize_BLE_RF(reinitialize);

#if (CYBLE_MTK_HOST == 1)
    CyBle_L2capCbfcRegisterPsm(MTK_L2CAP_PSM_ID, MTK_L2CAP_PSM_CREDIT_WATER_MARK);
#if (CYBLE_INTERFACE_ENABLED == 1)
    Ble_StartScan();
#endif  // CYBLE_INTERFACE_ENABLED
#endif  // CYBLE_MTK_HOST

#if (CYBLE_MTK_DUT == 1) && ((CYBLE_INTERFACE_ENABLED == 1) || (CYBLE_TESTS_ENABLED == 1))
    CyBle_L2capCbfcRegisterPsm(MTK_L2CAP_PSM_ID, MTK_L2CAP_PSM_CREDIT_WATER_MARK);
    Ble_Set_Address(ad);
#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
    Ble_StartAdvertisement();
#endif  // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED
#endif  // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED
}


/************************************************************************************************
 *
 *
 ************************************************************************************************/
#if (CYBLE_MTK_HOST == 1) || ((CYBLE_INTERFACE_ENABLED == 0) && (UART_INTERFACE_ENABLED == 1))
void SysTickISRCallback(void)
{
    /* Counts the number of milliseconds in one second */
    if(msCount > 0u)
    {
        --msCount;
    }
    else if ((msCount == 0) && (prev_command == TXC) && (prev_cmd_args[2] == -1))
    {
        time_elapsed++;
    }
    else
    {
        CySysTickStop();

        if (prev_command == TXP)
        {
            Timer_Stop();
            stop_DTM_tests();
        }
        else if (prev_command == RXP)
        {
            stop_DTM_tests();
            rx_count = (uint16_t)CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_DTM_RX_PKT_COUNT));
            rx_count_read = true;
        }
        else if (prev_command == DCW)
        {
            Timer_Stop();
            DTM_test_inprogress = false;
        }
        initialize_BLE(true);
        if (prev_command == TXC)
        {
            recover_from_TXC();
        }
#if (CYBLE_MTK_HOST == 1)
        Red_LED_Write(1);
#endif // CYBLE_MTK_HOST
    }
}
#endif // CYBLE_MTK_HOST


/************************************************************************************************
 *
 *
 ************************************************************************************************/
#if (CYBLE_MTK_HOST == 1) || ((CYBLE_INTERFACE_ENABLED == 0) && (UART_INTERFACE_ENABLED == 1))
void ms_timer_start(uint32_t ms_timeout)
{
    msCount = ms_timeout;
    CySysTickStart();
}
#endif // CYBLE_MTK_HOST

/************************************************************************************************
 *
 *
 ************************************************************************************************/
void MTK_init()
{
#if (CYBLE_MTK_HOST == 1) || ((CYBLE_INTERFACE_ENABLED == 0) && (UART_INTERFACE_ENABLED == 1))
    uint32_t i;
#endif // CYBLE_MTK_HOST

    MTK_packet_length = DEFAULT_MTK_PACKET_LENGTH;
    MTK_packet_type = DEFAULT_MTK_PACKET_TYPE;
    new_cmd_flag = false;

#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
    DTM_RX_end_delay = DEFAULT_DTM_RX_TEST_DELAY;
#endif  // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED
    DTM_TX_end_delay = DEFAULT_DTM_TX_END_DELAY;

#if 0
#if (UART_INTERFACE_ENABLED == 1)
    CI_start();
#endif  // UART_INTERFACE_ENABLED
#endif

    Timer_ISR_StartEx(timer_ISR);
    initialize_BLE(false);

#if (CYBLE_MTK_HOST == 1) || ((CYBLE_INTERFACE_ENABLED == 0) && (UART_INTERFACE_ENABLED == 1))
    CySysTickStart();
    for (i = 0u; i < CY_SYS_SYST_NUM_OF_CALLBACKS; ++i)
    {
        if (CySysTickGetCallback(i) == (void *) 0)
        {
            /* Set callback */
            CySysTickSetCallback(i, SysTickISRCallback);
            break;
        }
    }
    CySysTickStop();
#endif // CYBLE_MTK_HOST

#if (CYBLE_MTK_DUT == 1) && ((CYBLE_TESTS_ENABLED == 1))
    for (i = 0; i < 20; i++)
    {
        TX_buffer[i] = 0;
    }
    data_write_complete = true;
#endif // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static CYBLE_BLESS_PWR_LVL_T translate_power(int32_t power)
{
    if (power > 0)
        return CYBLE_LL_PWR_LVL_3_DBM;
    else if (power == 0)
        return CYBLE_LL_PWR_LVL_0_DBM;
    else if (power == -1)
        return CYBLE_LL_PWR_LVL_NEG_1_DBM;
    else if (power == -2)
        return CYBLE_LL_PWR_LVL_NEG_2_DBM;
    else if ((power <= -3) && (power > -6))
        return CYBLE_LL_PWR_LVL_NEG_3_DBM;
    else if ((power <= -6) && (power > -12))
        return CYBLE_LL_PWR_LVL_NEG_6_DBM;
    else if ((power <= -12) && (power > -18))
        return CYBLE_LL_PWR_LVL_NEG_12_DBM;
    else if (power <= -18)
        return CYBLE_LL_PWR_LVL_NEG_18_DBM ;
    else
        return CYBLE_LL_PWR_LVL_0_DBM;
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
#if (UART_INTERFACE_ENABLED == 1)
#ifdef UART_CI_DEBUG
void UART_print_int(int32_t input)
#else
//static void UART_print_int(int32_t input)
#endif  // UART_CI_DEBUG
//{
//    char temp[100];
//    int32_t i = 0;
//
//    temp[i] = '\0';
//    sprintf(temp, "%d", (int)input);
//    while (temp[i] != '\0')
//    {
//        UART_UartPutChar(temp[i++]);
//    }
//}
#endif  // UART_INTERFACE_ENABLED

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void backup_cmd_args()
{
    prev_command = command;
    prev_cmd_args[0] = cmd_args[0];
    prev_cmd_args[1] = cmd_args[1];
    prev_cmd_args[2] = cmd_args[2];
    prev_cmd_args[3] = cmd_args[3];
    prev_cmd_args[4] = cmd_args[4];
    prev_cmd_args[5] = cmd_args[5];
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
void stop_DTM_tests()
{
    if (DTM_test_inprogress == true)
    {
        CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_DBUS), 0xC992);
        CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_COMMAND_REGISTER),0x48); //setting it to idle
        DTM_test_inprogress = false;
    }
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
void recover_from_TXC()
{
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_SY), BLE_BLERD_SY_backup);
    CyDelayUs(120);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_CFGCTRL), BLE_BLERD_CFGCTRL_backup);
    CyDelayUs(120);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_CFG2), BLE_BLERD_CFG2_backup);
    CyDelayUs(120);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_CFG1), BLE_BLERD_CFG1_backup);
    CyDelayUs(120);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_DBUS), BLE_BLERD_DBUS_backup);
    CyDelayUs(120);

    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_RECEIVE_TRIG_CTRL), BLE_BLELL_RECEIVE_TRIG_CTRL_backup);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_LE_RF_TEST_MODE), BLE_BLELL_LE_RF_TEST_MODE_backup);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_COMMAND_REGISTER), BLE_BLELL_COMMAND_REGISTER_backup);
    CyDelayUs(120);
    DTM_test_inprogress = false;
}
#if 0
static char HexToAscii(uint8 value, uint8 nibble)
{
    if(nibble == 1)
    {
        /* bit-shift the result to the right by four bits */
        value = value & 0xF0;
        value = value >> 4;
        
        if (value > 9)
        {
            value = value - 10 + 'A'; /* convert to ASCII character */
        }
        else
        {
            value = value + '0'; /* convert to ASCII number */
        }
    }
    else if (nibble == 0)
    {
        /* extract the lower nibble */
        value = value & 0x0F;
        
        if (value >9)
        {
            value = value - 10 + 'A'; /* convert to ASCII character */
        }
        else
        {
            value = value + '0'; /* convert to ASCII number */
        }
    }
    else
    {
        value = ' ';  /* return space for invalid inputs */
    }
    
    return value;
}
#endif

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void print_test_status()
{
    if (prev_command == TXP)
    {
#if (UART_INTERFACE_ENABLED == 1)
        int32_t temp = Timer_ReadCounter();
        if (temp == 0)
        {
            temp = Timer_ReadPeriod();
        }
        if ((prev_cmd_args[2] > 2) || (prev_cmd_args[2] == -1))
        {
            temp += 2;
        }
        //UART_print_int(temp);
        //UART_UartPutString("\r\n");
#endif  // UART_INTERFACE_ENABLED

#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
        CyBle_L2capChannelDataWrite(cyBle_connHandle.bdHandle, MTK_local_CID, (uint8_t*)&tx_count, 2);
#endif  // CYBLE_MTK_DUT
    }
    else if (prev_command == RXP)
    {
#if (UART_INTERFACE_ENABLED == 1) && ((CYBLE_MTK_DUT == 1) || (CYBLE_MTK_HOST == 1))
        //UART_print_int(rx_count);
        //UART_UartPutString("\r\n");
#endif  // CYBLE_MTK_HOST && CYBLE_MTK_DUT

#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
        CyBle_L2capChannelDataWrite(cyBle_connHandle.bdHandle, MTK_local_CID, (uint8_t*)&rx_count, 2);
#endif  // CYBLE_MTK_DUT
    }
    else if (prev_command == SPL)
    {
#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
        uint32_t temp;
        temp = get_packet_length();
        CyBle_L2capChannelDataWrite(cyBle_connHandle.bdHandle, MTK_local_CID, (uint8_t*)&temp, 4);
#endif  // CYBLE_MTK_DUT

#if (UART_INTERFACE_ENABLED == 1)
        //UART_print_int(get_packet_length());
        //UART_UartPutString("\r\n");
#endif  // UART_INTERFACE_ENABLED
    }
    else if (prev_command == SPT)
    {
#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
        uint32_t temp;
        temp = get_packet_type();
        CyBle_L2capChannelDataWrite(cyBle_connHandle.bdHandle, MTK_local_CID, (uint8_t*)&temp, 4);
#endif  // CYBLE_MTK_DUT

#if (UART_INTERFACE_ENABLED == 1)
        //UART_print_int(get_packet_type());
        //UART_UartPutString("\r\n");
#endif  // UART_INTERFACE_ENABLED
    }
    else if (prev_command == TXC)
    {
#if (UART_INTERFACE_ENABLED == 1)
        //UART_print_int(msCount_backup);
        //UART_UartPutString("\r\n");
#endif  // UART_INTERFACE_ENABLED

#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
        CyBle_L2capChannelDataWrite(cyBle_connHandle.bdHandle, MTK_local_CID, (uint8_t*)&msCount_backup, 4);
#endif  // CYBLE_MTK_DUT
    }
    else if (prev_command == CUS)
    {
#if (CYBLE_MTK_HOST == 1)
#endif  // CYBLE_MTK_HOST

#if (CYBLE_MTK_DUT == 1) && (UART_INTERFACE_ENABLED == 1)
        int i;
//        UART_UartPutString((const char*)TX_buffer);
//        UART_UartPutString("\r\n");
        for (i = 0; i < packet_count; i++)
        {
            //UART_print_int((int32_t)TX_buffer[i]);
            //UART_UartPutString("\r\n");
        }
#endif  // UART_INTERFACE_ENABLED && CYBLE_MTK_DUT
    }
    else if (prev_command == STC)
    {
#if (CYBLE_MTK_DUT == 1) && (UART_INTERFACE_ENABLED == 1)
        //UART_print_int(packet_count);
        //UART_UartPutString("\r\n");
#endif  // UART_INTERFACE_ENABLED && CYBLE_MTK_DUT
    }
    else if (prev_command == RSX)
    {
        //UART_print_int(RSSIValue);
        //UART_UartPutString("dBm\n\r");
    }
    else if (prev_command == WBA)
    {
        if (WBAResult)
        {
            //UART_UartPutString("SUCCESS\r\n");
        }
        else
        {
            //UART_UartPutString("ERROR\r\n");
        }
    }
    else if (prev_command == RBA)
    {
        if (RBAResult)
        {
            int32 i;

            for (i = (BDA_LENGTH - 1); i >= 0 ; i--)
            {
                //UART_UartPutChar(HexToAscii(BDAddress[i], 1));
                //UART_UartPutChar(HexToAscii(BDAddress[i], 0));
                //UART_UartPutString(" ");
            }
            //UART_UartPutString("\r\n");
        }
        else
        {
            //UART_UartPutString("ERROR\r\n");
        }
    }
    else if (prev_command == STR)
    {
        if (STRResult)
        {
            //UART_UartPutString("SUCCESS\r\n");
        }
        else
        {
            //UART_UartPutString("ERROR\r\n");
        }
    }
    else if (prev_command == LTR)
    {
        if (LTRResult)
        {
            int32 TrimRegister = CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_BB_XO_CAPTRIM));
            char temp[10];
            int32_t i = 0;

            temp[i] = '\0';
            sprintf(temp, "%x", (int)TrimRegister);
            //UART_UartPutString(temp);
            //UART_UartPutString("\r\n");
        }
        else
        {
            //UART_UartPutString("ERROR\r\n");
        }
    }
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
static void wait_for_disconnection()
{
    do
    {
        CyBle_ProcessEvents();
    } while(is_L2CAP_enabled || (CyBle_GetState() != CYBLE_STATE_DISCONNECTED));
}
#endif  // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED

/************************************************************************************************
 *
 *
 ************************************************************************************************/
#if (CYBLE_MTK_HOST == 1) && (CYBLE_INTERFACE_ENABLED == 1)
static void handle_DUT_redirection(int32_t args_length)
{
    if ((command == DUT) && (cmd_args[0] == 0))
    {
        Red_LED_Write(0);
        redirect_to_DUT = false;
        Blue_LED_Write(1);
        Green_LED_Write(0);
        Red_LED_Write(1);
    }
    else if ((command != NO_CMD) && (command != DUT))
    {
        uint8_t temp[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        uint8_t *tmp, i;

        Red_LED_Write(0);
        temp[0] = command;
        temp[1] = args_length;
        for (i = 0; i < args_length; i++)
        {
            tmp = (uint8_t *)&cmd_args[i];
            temp[(i * 4) + 2] = tmp[0];
            temp[(i * 4) + 3] = tmp[1];
            temp[(i * 4) + 4] = tmp[2];
            temp[(i * 4) + 5] = tmp[3];
        }

        if (is_L2CAP_enabled)
        {
            data_write_complete = false;
            CyBle_L2capChannelDataWrite(cyBle_connHandle.bdHandle, MTK_local_CID, temp, 14);
            Red_LED_Write(1);
            if (command == TXC)
            {
                Red_LED_Write(0);
                while (data_write_complete == false)
                {
                    CyBle_ProcessEvents();
                }
                Ble_StopScan();
                Ble_Disconnect_L2CAP_Channel();
                redirect_to_DUT = false;
                Blue_LED_Write(1);
                if (cmd_args[2] >= 0)
                {
                    ms_timer_start(cmd_args[2]);
                }
            }
        }
        else
        {
            redirect_to_DUT = false;
            Blue_LED_Write(1);
            Green_LED_Write(1);
            Red_LED_Write(1);
        }
    }
    if (data_received == true)
    {
        data_received = false;

#if (UART_INTERFACE_ENABLED == 1)
        UART_print_int(RRS_result);
        UART_UartPutString("\r\n");
#endif  // UART_INTERFACE_ENABLED
    }
}
#endif  // CYBLE_MTK_HOST


static void execute_RRS(bool enable_output_printing)
{
    if (prev_command == TXP)
    {
        stop_DTM_tests();
        Timer_Stop();
        clear_timer_interrupt();
        CySysTickStop();
    }
    else if (prev_command == RXP)
    {
        if (DTM_test_inprogress == true)
        {
            CySysTickStop();
            stop_DTM_tests();
            if (rx_count_read == false)
            {
                rx_count = (uint16_t)CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_DTM_RX_PKT_COUNT));
            }
        }
    }
#if (CYBLE_MTK_HOST == 1) || ((CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 0))
    else if (prev_command == TXC)
    {
        if ((prev_cmd_args[2] >= 0) && (msCount > 0))
        {
            msCount_backup = msCount;
            msCount = 1;
            while (msCount);
            msCount_backup = prev_cmd_args[2] - msCount_backup;
        }
        else if (prev_cmd_args[2] == -1)
        {
            msCount_backup = time_elapsed;
            initialize_BLE(false);
            recover_from_TXC();
        }
        else if ((prev_cmd_args[2] >= 0) && (msCount == 0))
        {
            msCount_backup = prev_cmd_args[2] - msCount_backup;
        }
    }
#endif  // CYBLE_MTK_HOST && (CYBLE_MTK_DUT && !CYBLE_INTERFACE_ENABLED)
#if (CYBLE_MTK_DUT == 1) && (CYBLE_TESTS_ENABLED == 1)
    else if (prev_command == STC)
    {
        Ble_StopAdvertisement();
        is_L2CAP_enabled = false;
        data_write_complete = true;
    }
#endif  // CYBLE_TESTS_ENABLED

    if (enable_output_printing == true)
    {
        print_test_status();
    }
#if (CYBLE_MTK_HOST == 1) || ((CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 0))
#if (CYBLE_INTERFACE_ENABLED == 1)
    Ble_StopScan();
    Ble_Disconnect_L2CAP_Channel();
#endif  // CYBLE_INTERFACE_ENABLED
    initialize_BLE(true);
    msCount = 0;
#endif  // CYBLE_MTK_HOST || (CYBLE_MTK_DUT && !CYBLE_INTERFACE_ENABLED)
}

static bool first_command;

UART_CMDS_T get_command_txp(int32_t *argument_array, int32_t *num_args)
{
    if (first_command)
    {
        argument_array[0] = 17; // channel 17
        argument_array[1] = 0;  // tx power = 0dbm
        argument_array[2] = 1000; // number of packets
        *num_args = 3;
        first_command = false;
        return TXP;
    }
    
    return NO_CMD;
}

UART_CMDS_T get_command_txc(int32_t *argument_array, int32_t *num_args)
{
    if (first_command)
    {
        argument_array[0] = 17; // channel 17
        argument_array[1] = 0;  // tx power = 0dbm
        argument_array[2] = INT32_MAX; // timeout in milliseconds (24 days)
        *num_args = 3;
        first_command = false;
        return TXC;
    }
    
    return NO_CMD;
}

UART_CMDS_T get_command_rx(int32_t *argument_array, int32_t *num_args)
{
    if (first_command)
    {
        argument_array[0] = 17; // channel 17
        argument_array[1] = INT32_MAX; // timeout in milliseconds (24 days)
        *num_args = 2;
        first_command = false;
        return RXP;
    }
    
    return NO_CMD;
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
uint8_t MTK_mode()
{
#if (CYBLE_MTK_DUT == 1) && (UART_INTERFACE_ENABLED == 1) && (CYBLE_INTERFACE_ENABLED == 1)
    uint8_t ble_cmd = NO_CMD;
#endif  // UART_INTERFACE_ENABLED || CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED

#if (CYBLE_MTK_DUT == 1) || (CYBLE_MTK_HOST == 1) || (UART_INTERFACE_ENABLED == 1)
    int32_t args_length = 0;
#endif  // CYBLE_MTK_DUT || UART_INTERFACE_ENABLED

    MTK_init();
    first_command = true;

    while(1)
    {
#if (UART_INTERFACE_ENABLED == 1)
#if 0
        command = CI_get_command(cmd_args, &args_length);
#endif
        //command = get_command_txp(cmd_args, &args_length);
        //command = get_command_txc(cmd_args, &args_length);
        command = get_command_rx(cmd_args, &args_length);
        if ((DTM_test_inprogress == true) && ((command == TXP) || (command == RXP) || (command == TXC)))
        {
            execute_RRS(false);
        }

#endif  // UART_INTERFACE_ENABLED

#if (CYBLE_TESTS_ENABLED == 1)
#if (CYBLE_MTK_HOST == 1) && (CYBLE_INTERFACE_ENABLED == 1)
        if (DTM_test_inprogress == false)
        {
            CyBle_ProcessEvents();
            Ble_StartScan();
            Ble_Connect_L2CAP_Channel();
        }
#endif  // CYBLE_MTK_HOST && CYBLE_INTERFACE_ENABLED

#if (CYBLE_MTK_DUT == 1)
    if (prev_command == STC)
    {
        if ((packet_count < prev_cmd_args[0]) || (prev_cmd_args[0] == -1))
        {
            CyBle_ProcessEvents();
            Ble_StartAdvertisement();
            if ((is_L2CAP_enabled == true) && (data_write_complete == true))
            {
                int i;
                data_write_complete = false;
                CyBle_L2capChannelDataWrite(cyBle_connHandle.bdHandle, MTK_local_CID, (uint8_t*)TX_buffer, 20);
                for (i = 0; i < 20; i++)
                {
                    TX_buffer[i]++;
                }
            }
            else if (is_L2CAP_enabled == true)
            {
                CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
    //            if (CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP) == CYBLE_BLESS_DEEPSLEEP)
    //            {
    //                UART_Sleep();
    //                CyGlobalIntDisable;
    //                if((CyBle_GetBleSsState() == CYBLE_BLESS_STATE_ECO_ON) || (CyBle_GetBleSsState() == CYBLE_BLESS_STATE_DEEPSLEEP))
    //                {
    ////                    CySysPmDeepSleep();
    //                }
    //                CyGlobalIntEnable;
    //                UART_Wakeup();
    //            }
    //            else if(CyBle_GetBleSsState() != CYBLE_BLESS_STATE_EVENT_CLOSE)
    //            {
    //                CySysPmSleep();
    //            }
            }
        }
    }
#endif  // CYBLE_MTK_DUT
#endif  // CYBLE_TESTS_ENABLED

#if (CYBLE_MTK_HOST == 1) && (CYBLE_INTERFACE_ENABLED == 1)
        if (DTM_test_inprogress == false)
        {
            CyBle_ProcessEvents();
            Ble_StartScan();
            Ble_Connect_L2CAP_Channel();
        }
#endif  // CYBLE_MTK_HOST && CYBLE_INTERFACE_ENABLED

#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
        CyBle_ProcessEvents();
        Ble_StartAdvertisement();

#if (UART_INTERFACE_ENABLED == 1)
        ble_cmd = BLE_get_command(cmd_args, &args_length);
        if (ble_cmd != NO_CMD)
            command = ble_cmd;
#else
        command = BLE_get_command(cmd_args, &args_length);
#endif  // UART_INTERFACE_ENABLED

#endif  // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED

#if (CYBLE_MTK_HOST == 1) && (CYBLE_INTERFACE_ENABLED == 1)
        if(redirect_to_DUT == true)
        {
            handle_DUT_redirection(args_length);
        }
        else if (command == TXP)
#endif  // CYBLE_MTK_HOST

#if ((CYBLE_MTK_DUT == 1) || ((CYBLE_MTK_HOST == 1) && (UART_INTERFACE_ENABLED == 1) && (CYBLE_INTERFACE_ENABLED == 0)) || (NO_INPUT_ENABLED == 1))
        if (command == TXP)
#endif  // CYBLE_MTK_DUT || UART_INTERFACE_ENABLED
        {
            backup_cmd_args();
#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(0);
#if (CYBLE_INTERFACE_ENABLED == 1)
            Ble_StopScan();
            Ble_Disconnect_L2CAP_Channel();
#endif  // CYBLE_INTERFACE_ENABLED
#endif  // CYBLE_MTK_HOST

#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
            Ble_StopAdvertisement();
            wait_for_disconnection();
#endif  // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED

            initialize_BLE_RF(true);
            CyDelay(DTM_TX_START_DELAY);
            DTM_tx_start(cmd_args[0], translate_power(cmd_args[1]), cmd_args[2]);
            DTM_TX_end_delay = ((cmd_args[2] * UNIT_PACKET_TX_TIMEOUT) / _1000_US) + (10 * DTM_TX_START_DELAY);

#if ((CYBLE_MTK_HOST == 1) && (CYBLE_INTERFACE_ENABLED == 1)) || ((CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 0))
            if (cmd_args[2] >= 0)
            {
                ms_timer_start(DTM_TX_end_delay);
            }
#endif  // (CYBLE_MTK_HOST && CYBLE_INTERFACE_ENABLED) || (CYBLE_MTK_DUT && !CYBLE_INTERFACE_ENABLED)

#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
            CyDelay(DTM_TX_end_delay);
            stop_DTM_tests();
            tx_count = Timer_ReadCounter();
            if (tx_count == 0)
            {
                tx_count = Timer_ReadPeriod();
            }
            if ((prev_cmd_args[2] > 2) || (prev_cmd_args[2] == -1))
            {
                tx_count += 2;
            }
            initialize_BLE(true);
#endif  // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED
        }
        else if (command == RXP)
        {
            backup_cmd_args();
#if (CYBLE_INTERFACE_ENABLED == 1)
            if (cmd_args[1] >= 0)
            {
                DTM_RX_end_delay = ((cmd_args[1] * UNIT_PACKET_TX_TIMEOUT) / _1000_US) + (2 * DTM_TX_START_DELAY);
            }
            else
            {
                DTM_RX_end_delay = -1;
            }
#endif  // (CYBLE_INTERFACE_ENABLED)

#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(0);
#if (CYBLE_INTERFACE_ENABLED == 1)
            Ble_StopScan();
            Ble_Disconnect_L2CAP_Channel();
#endif  // CYBLE_INTERFACE_ENABLED
#endif  // CYBLE_MTK_HOST

#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
            Ble_StopAdvertisement();
            wait_for_disconnection();
#endif  // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED

            initialize_BLE_RF(true);
            DTM_rx_start(cmd_args[0]);
            ms_timer_start(cmd_args[1]);

#if ((CYBLE_MTK_HOST == 1) && (CYBLE_INTERFACE_ENABLED == 1))
            if (DTM_RX_end_delay >= 0)
            {
                ms_timer_start(DTM_RX_end_delay + DTM_RX_END_DELAY_BUFFER);
            }
#endif  // (CYBLE_MTK_HOST && CYBLE_INTERFACE_ENABLED)

#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
            CyDelay(DTM_RX_end_delay + DTM_RX_END_DELAY_BUFFER);
            stop_DTM_tests();
            rx_count = (uint16_t)CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_DTM_RX_PKT_COUNT));
            initialize_BLE(true);
#endif  // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED
        }
        else if (command == RRS)
        {
#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(0);
#endif  // CYBLE_MTK_HOST

            execute_RRS(true);
            backup_cmd_args();

#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(1);
#endif  // CYBLE_MTK_HOST
        }
        else if (command == PST)
        {
#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(0);
#endif  // CYBLE_MTK_HOST

            print_test_status();

#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(1);
#endif  // CYBLE_MTK_HOST
        }
        else if (command == SPL)
        {
#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(0);
#endif  // CYBLE_MTK_HOST

            backup_cmd_args();
            set_packet_length(cmd_args[0]);

#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(1);
#endif  // CYBLE_MTK_HOST
        }
        else if (command == SPT)
        {
#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(0);
#endif  // CYBLE_MTK_HOST

            backup_cmd_args();
            set_packet_type(cmd_args[0]);

#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(1);
#endif  // CYBLE_MTK_HOST
        }
        else if (command == VER)
        {
#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(0);
#endif  // CYBLE_MTK_HOST

#if (UART_INTERFACE_ENABLED == 1)
//            UART_UartPutString(CYBLE_MTK_VERSION);
//            UART_UartPutString("\r\n");
#endif  // UART_INTERFACE_ENABLED

#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(1);
#endif  // CYBLE_MTK_HOST
        }
        else if (command == TXC)
        {
            backup_cmd_args();
#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(0);
#if (CYBLE_INTERFACE_ENABLED == 1)
            Ble_StopScan();
            Ble_Disconnect_L2CAP_Channel();
#endif  // CYBLE_INTERFACE_ENABLED
#endif  // CYBLE_MTK_HOST

#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
            Ble_StopAdvertisement();
            wait_for_disconnection();
#endif  // CYBLE_MTK_DUT && CYBLE_INTERFACE_ENABLED

            transmit_carrier_wave(cmd_args[0], translate_power(cmd_args[1]));

#if (CYBLE_MTK_HOST == 1) || ((CYBLE_INTERFACE_ENABLED == 0) && (UART_INTERFACE_ENABLED == 1))
            time_elapsed = 0;
            if (cmd_args[2] > -1)
            {
                ms_timer_start(cmd_args[2]);
            }
            else
            {
                ms_timer_start(0);
            }
#endif  // CYBLE_MTK_HOST

#if (CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 1)
            msCount_backup = cmd_args[2];
            if (cmd_args[2] >= 0)
            {
                CyDelay(cmd_args[2]);
                initialize_BLE(true);
                recover_from_TXC();
            }
#endif  // CYBLE_MTK_DUT
        }
#if (CYBLE_MTK_HOST == 1)
        else if (command == DUT)
        {
            Red_LED_Write(0);

#if (CYBLE_INTERFACE_ENABLED == 1)
            if ((cmd_args[0] == 1) && is_L2CAP_enabled)
            {
                redirect_to_DUT = true;
                Green_LED_Write(1);
                Blue_LED_Write(0);
            }
#endif  // CYBLE_INTERFACE_ENABLED

            Red_LED_Write(1);
        }
        else if (command == PCS)
        {
            Red_LED_Write(0);

#if (UART_INTERFACE_ENABLED == 1) && (CYBLE_INTERFACE_ENABLED == 1)
            if (is_L2CAP_enabled == true)
            {
                UART_UartPutString("CONNECTED\r\n");
            }
            else
            {
                UART_UartPutString("DISCONNECTED\r\n");
            }
#endif  // UART_INTERFACE_ENABLED && CYBLE_INTERFACE_ENABLED

            Red_LED_Write(1);
        }
        else if (command == DCW)
        {
#if (UART_INTERFACE_ENABLED == 1) && (CYBLE_INTERFACE_ENABLED == 1)
            Red_LED_Write(0);

            backup_cmd_args();
            DTM_test_inprogress = true;
            Ble_StopScan();
            Ble_Disconnect_L2CAP_Channel();

            initialize_BLE_RF(true);
            CyDelay(DTM_TX_START_DELAY);
            if (cmd_args[0] >= 0)
            {
                DTM_TX_end_delay = ((cmd_args[0] * UNIT_PACKET_TX_TIMEOUT) / _1000_US) + (10 * DTM_TX_START_DELAY);
            }
            else
            {
                DTM_TX_end_delay = -1;
            }

#if ((CYBLE_MTK_HOST == 1) && (CYBLE_INTERFACE_ENABLED == 1)) || ((CYBLE_MTK_DUT == 1) && (CYBLE_INTERFACE_ENABLED == 0))
            if (cmd_args[0] >= 0)
            {
                ms_timer_start(DTM_TX_end_delay);
            }
#endif  // (CYBLE_MTK_HOST && CYBLE_INTERFACE_ENABLED) || (CYBLE_MTK_DUT && !CYBLE_INTERFACE_ENABLED)
#endif  // UART_INTERFACE_ENABLED && CYBLE_INTERFACE_ENABLED
        }
#endif  // CYBLE_MTK_HOST
        else if (command == WHO)
        {
#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(0);
#endif  // CYBLE_MTK_HOST

#if (UART_INTERFACE_ENABLED == 1)
            //UART_UartPutString(WHO_AM_I);
            //UART_UartPutString("\r\n");
#endif  // UART_INTERFACE_ENABLED

#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(1);
#endif  // CYBLE_MTK_HOST
        }
#if (CYBLE_TESTS_ENABLED == 1)
        else if (command == STC)
        {
            int i;
            execute_RRS(false);
            backup_cmd_args();
            packet_count = 0;
            for (i = 0; i < 20; i++)
            {
                TX_buffer[i] = 0;
            }
        }
#endif  // CYBLE_TESTS_ENABLED
        else if (command == CUS)
        {
#if 0            
#if (CYBLE_MTK_DUT == 1)
            int i;
#endif  // CYBLE_MTK_DUT

            backup_cmd_args();
#if (CYBLE_MTK_HOST == 1)
#endif  // CYBLE_MTK_HOST

#if (CYBLE_MTK_DUT == 1)
            for (i = 0; i < 21; i++)
            {
                TX_buffer[i] = '\0';
            }
            process_custom_command(cmd_args, TX_buffer, &packet_count);
#endif  // CYBLE_MTK_DUT
#endif
        }
        else if (command == ARU)
        {
            execute_RRS(true);
            CyBle_Stop();
            UART_Stop();
            Timer_ISR_Stop();
            Timer_Stop();
            return 0xAA;
        }
        else if (command == RSX)
        {
            int i;

            RSSIValue = -85;

            backup_cmd_args();
#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(0);
#endif
            initialize_BLE_RF(true);
            DTM_rx_start(cmd_args[0]);

            for (i = 0; i < 1000; i++)
            {
                int32 tempRSSI;
                tempRSSI = CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_RSSI))
                                                            >> POSTFILT_VALUE;
                tempRSSI += -85;

                if (RSSIValue < tempRSSI)
                {
                    RSSIValue = tempRSSI;
                }
            }

            stop_DTM_tests();
#if (CYBLE_MTK_HOST == 1)
            Red_LED_Write(1);
#endif
        }
        else if (command == WBA)
        {
            #if 0
            uint32 buffer[2];

            backup_cmd_args();

			buffer[0]  = (cmd_args[2] << 24) & 0xFF000000;
			buffer[0] |= (cmd_args[3] << 16) & 0x00FF0000;
			buffer[0] |= (cmd_args[4] << 8)  & 0x0000FF00;
			buffer[0] |= (cmd_args[5])       & 0x000000FF;
			buffer[1]  = (cmd_args[0] << 8)  & 0x0000FF00;
			buffer[1] |= (cmd_args[1])       & 0x000000FF;

            while (0 != (UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID)) {};
            WBAResult = WriteUserSFlashRow(0, 0, buffer, 2);
            #endif
        }
        else if (command == RBA)
        {
            backup_cmd_args();
            RBAResult = ReadUserSflash(0, 0, BDAddress, BDA_LENGTH);
        }
        else if (command == RTR)
        {
            uint32 TrimRegister;
            backup_cmd_args();
            TrimRegister = CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_BB_XO_CAPTRIM));
#if (UART_INTERFACE_ENABLED == 1)
            char temp[10];
            int32_t i = 0;

            temp[i] = '\0';
            sprintf(temp, "%x", (int)TrimRegister);
            //UART_UartPutString(temp);
            //UART_UartPutString("\r\n");
#endif  // UART_INTERFACE_ENABLED
        }
        else if (command == WTR)
        {
            backup_cmd_args();
            CY_SET_XTND_REG32((void CYFAR *)CYREG_BLE_BLERD_BB_XO_CAPTRIM, cmd_args[0]);
        }
        else if (command == STR)
        {
            #if 0
            uint32 TrimRegister;
            backup_cmd_args();
            TrimRegister = CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_BB_XO_CAPTRIM));
            while (0 != (UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID)) {};
            STRResult = WriteUserSFlashRow(1, 0, &TrimRegister, 1);
            #endif
        }
        else if (command == LTR)
        {
            uint8 TrimRegister[4];
            backup_cmd_args();
            LTRResult = ReadUserSflash(1, 0, TrimRegister, 4);
            int32 TrimValue = (TrimRegister[1] << 8) | TrimRegister[0];
            CY_SET_XTND_REG32((void CYFAR *)CYREG_BLE_BLERD_BB_XO_CAPTRIM, TrimValue);
        }
    }
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
static void set_power(CYBLE_BLESS_PWR_LVL_T power)
{
    CYBLE_BLESS_PWR_IN_DB_T blessPwrAdv;

    blessPwrAdv.bleSsChId = CYBLE_LL_ADV_CH_TYPE; // Select ADV ch grp
    blessPwrAdv.blePwrLevelInDbm = power;

    (void)CyBle_SetTxPowerLevel(&blessPwrAdv);
    blessPwrAdv.bleSsChId = CYBLE_LL_CONN_CH_TYPE;// Select CONN ch grp
    blessPwrAdv.blePwrLevelInDbm = power;
    (void)CyBle_SetTxPowerLevel(&blessPwrAdv);
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
void transmit_carrier_wave(uint32_t channel, CYBLE_BLESS_PWR_LVL_T power)
{
    uint32_t cfg2, cfgctrl, sy;
	uint16_t frequency;
	frequency =  2402 + channel * 2;

    stop_DTM_tests();
    set_power(power);

    BLE_BLELL_RECEIVE_TRIG_CTRL_backup = CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_RECEIVE_TRIG_CTRL));
//    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_RECEIVE_TRIG_CTRL), 0x9020);
    BLE_BLELL_LE_RF_TEST_MODE_backup = CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_LE_RF_TEST_MODE));
//    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_LE_RF_TEST_MODE), 0xFC00 | channel);
    BLE_BLERD_DBUS_backup = CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_DBUS));
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_DBUS), 0xE000 | frequency);
//    BLE_BLELL_COMMAND_REGISTER_backup = CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_COMMAND_REGISTER));
//    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_COMMAND_REGISTER), 0x46);

    CyDelayUs(120);

    BLE_BLERD_CFG2_backup = CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_CFG2));
    cfg2 = BLE_BLERD_CFG2_backup | 0x1000;
    cfg2 &= ~0x03FF;
    cfg2 |= 0x0200;

    BLE_BLERD_CFGCTRL_backup = CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_CFGCTRL));
    cfgctrl = BLE_BLERD_CFGCTRL_backup | 0x8008;
    cfgctrl &= ~0x0010;

    BLE_BLERD_SY_backup = CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_SY));
	sy = BLE_BLERD_SY_backup | 0x4000;

    BLE_BLERD_CFG1_backup = CY_GET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_CFG1));

    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_CFG2), cfg2);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_CFG1), 0xBB48);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_CFGCTRL), cfgctrl);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLERD_SY), sy);

    DTM_test_inprogress = true;
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
void set_packet_length(uint32_t length)
{
    MTK_packet_length = length;
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
void set_packet_type(uint32_t type)
{
    MTK_packet_type = type;
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
uint32_t get_packet_length()
{
    return MTK_packet_length;
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
uint32_t get_packet_type()
{
    return MTK_packet_type;
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
void DTM_tx_start(uint32_t channel, CYBLE_BLESS_PWR_LVL_T power, uint32_t num_packets)
{
    stop_DTM_tests();
    set_power(power);

    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_LE_RF_TEST_MODE), ((MTK_packet_length << 10) |
                                                                        (MTK_packet_type << 7) | channel));
    Timer_SetOneShot(1);
    if (num_packets > 2)
    {
        Clock_1_SetDivider(14999);
        if (num_packets <= 65535)
        {
            num_packets -= 2;
        }
        else
        {
            num_packets = 65535;
            Timer_SetOneShot(0);
        }
    }
    else
    {
        Clock_1_SetDivider(7400);
    }

    Timer_Stop();
    Timer_WritePeriod(num_packets);
    Timer_WriteCounter(0);
    Timer_Start();
    Timer_Stop();
    Timer_WritePeriod(num_packets);
    Timer_WriteCounter(0);
    Timer_Start();
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_COMMAND_REGISTER), 0x46);
    DTM_test_inprogress = true;
}

/************************************************************************************************
 *
 *
 ************************************************************************************************/
void DTM_rx_start(uint32_t channel)
{
    stop_DTM_tests();

    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_LE_RF_TEST_MODE), ((MTK_packet_length << 10) |
                                                                        (MTK_packet_type << 7) | channel));
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_BLE_BLELL_COMMAND_REGISTER),0x47);
    DTM_test_inprogress = true;
    rx_count_read = false;
}

/*******************************************************************************
* Function Name: WriteUserSFlashRow
********************************************************************************
* Summary:
*        This routine calls the PSoC 4 BLE device supervisory ROM APIs to update
* the user configuration area of Supervisory Flash (SFlash).  
*
* Parameters:
*  userRowNUmber - User config SFlash row number to which data is to be written
*  dataPointer - Pointer to the data to be written. This API writes one row of
*                user config SFlash row at a time.
*
* Return:
*  uint32 - state of the user config SFlash write operation.
*
*******************************************************************************/
#if defined (__GNUC__)
#pragma GCC optimize ("O0")
#endif /* End of #if defined (__GNUC__) */
bool WriteUserSFlashRow(uint8 userRowNumber, uint16 dataOffset, uint32 *srcBuffer, uint32 length)
{
    uint16 localCount, tempIndex = 0;
	volatile uint32 retValue=0;
	volatile uint32 cmdDataBuffer[(CY_FLASH_SIZEOF_ROW/4) + 2];
	volatile uint32 reg1,reg2,reg3,reg4,reg5,reg6;

	/* Store the clock settings temporarily */
    reg1 =	CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_SELECT));
    reg2 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_CONFIG));
    reg3 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM4));
    reg4 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM5));
    reg5 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM1));
    reg6 =  CY_GET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM2));

	/* Initialize the clock necessary for flash programming */
	CY_SET_REG32(CYREG_CPUSS_SYSARG, 0x0000e8b6);
	CY_SET_REG32(CYREG_CPUSS_SYSREQ, 0x80000015);

	/******* Initialize SRAM parameters for the LOAD FLASH command ******/
	/* byte 3 (i.e. 00) is the Macro_select */
	/* byte 2 (i.e. 00) is the Start addr of page latch */
	/* byte 1 (i.e. d7) is the key 2  */
	/* byte 0 (i.e. b6) is the key 1  */
  	cmdDataBuffer[0]=0x0000d7b6;

	/****** Initialize SRAM parameters for the LOAD FLASH command ******/
	/* byte 3,2 and 1 are null */
	/* byte 0 (i.e. 7F) is the number of bytes to be written */
	cmdDataBuffer[1]=0x0000007F;	 

	/* Initialize the SRAM buffer with data bytes */
    for(localCount = 0; localCount < (CY_FLASH_SIZEOF_ROW / 4); localCount++)    
	{
        if ((localCount < dataOffset) || (localCount >= (dataOffset + length)))
        {
    	    cmdDataBuffer[localCount + 2] = 0;
        }
        else
        {
    	    cmdDataBuffer[localCount + 2] = srcBuffer[tempIndex++];
        }
	}

	/* Write the following to registers to execute a LOAD FLASH bytes */
	CY_SET_REG32(CYREG_CPUSS_SYSARG, &cmdDataBuffer[0]);
	CY_SET_REG32(CYREG_CPUSS_SYSREQ, LOAD_FLASH);

    /****** Initialize SRAM parameters for the WRITE ROW command ******/
	/* byte 3 & 2 are null */
	/* byte 1 (i.e. 0xeb) is the key 2  */
	/* byte 0 (i.e. 0xb6) is the key 1  */
	cmdDataBuffer[0] = 0x0000ebb6;

	/* byte 7,6 and 5 are null */
	/* byte 4 is desired SFlash user row 
	 * Allowed values 0 - row 4
	                  1 - row 5 
					  2 - row 6
					  3 - row 7 */
	cmdDataBuffer[1] = (uint32) userRowNumber;

	/* Write the following to registers to execute a WRITE USER SFlash ROW command */
	CY_SET_REG32(CYREG_CPUSS_SYSARG, &cmdDataBuffer[0]);
	CY_SET_REG32(CYREG_CPUSS_SYSREQ, WRITE_USER_SFLASH_ROW);

	/* Read back SYSARG for the result. 0xA0000000 = SUCCESS; */
	retValue = CY_GET_REG32(CYREG_CPUSS_SYSARG);
	
	/* Restore the clock settings after the flash programming is done */
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_SELECT),reg1);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_CONFIG),reg2);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM4),reg3);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_PWR_BG_TRIM5),reg4);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM1),reg5);
    CY_SET_XTND_REG32((void CYFAR *)(CYREG_CLK_IMO_TRIM2),reg6);  
	
	return (retValue == 0xA0000000)?true:false;
}

bool ReadUserSflash(uint8 userRowNumber, uint16 dataOffset,
                        uint8 *dataPointer, uint16 dataSize)
{
    uint8 * sflashPtr;
    uint32 dataIndex;

    if ((userRowNumber > 3) || (dataOffset > USER_SFLASH_ROW_SIZE))
        return false;

    /* User SFlash read is direct memory read using pointers */
    sflashPtr = (uint8 *)(CYREG_SFLASH_MACRO_0_FREE_SFLASH0 + dataOffset +
                                        (userRowNumber * USER_SFLASH_ROW_SIZE));

    /* Read all the 512 bytes of user configurable SFlash content and display
    *  on UART console */
    for(dataIndex = 0; dataIndex < dataSize; dataIndex++)
    {
        *dataPointer++ = *sflashPtr++;
    }

    return true;
}

#if defined (__GNUC__)
#pragma GCC reset_options
#endif /* End of #if defined (__GNUC__) */

#endif // ENABLE_MTK

/* [] END OF FILE */
