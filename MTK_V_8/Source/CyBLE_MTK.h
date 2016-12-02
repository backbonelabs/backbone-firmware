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
#ifndef __CYBLE_MTK_H__
#define __CYBLE_MTK_H__

#include <project.h>
#include "uart_cmd_int.h"
#include "stdbool.h"

#define CYBLE_MTK_VERSION                               "1.8.6078"

/* SFlash row size for 128KB flash BLE device. For other PSoC 4 BLE devices
 * with higher flash size, this example project might need some modification.
 * Please check the device datasheet and TRM before using this code on non 128KB
 * flash devices */
#if (CYBLE_FLASH_SIZE == 1)
    #define USER_SFLASH_ROW_SIZE                        (128u)
#else
    #define USER_SFLASH_ROW_SIZE                        (256u)
#endif

/* Starting value to be stored in user SFlash to demonstrate SFlash write API */
#define SFLASH_STARTING_VALUE                           (0x00)
/* Total number of user SFlash rows supported by the device */
#define USER_SFLASH_ROWS                                (4u)
/* Starting address of user SFlash row for 128KB PSoC 4 BLE device */
//#define USER_SFLASH_BASE_ADDRESS        (0x0FFFF200u)

#define LOAD_FLASH                                      0x80000004
#define WRITE_USER_SFLASH_ROW                           0x80000018
#define USER_SFLASH_WRITE_SUCCESSFUL                    0xA0000000

//#define UART_CI_DEBUG
#define CAP_TRIM                                        0x3FFA

#define CYBLE_MTK_HOST                                  1
#define CYBLE_MTK_DUT                                   !CYBLE_MTK_HOST

#if (CYBLE_MTK_HOST == 1)
    #define UART_INTERFACE_ENABLED                          1
    #define CYBLE_INTERFACE_ENABLED                         1
    #define CYBLE_TESTS_ENABLED                             0
    #define WHO_AM_I                                        "HOST"
#endif  // CYBLE_MTK_HOST

#if (CYBLE_MTK_DUT == 1)
    #define UART_INTERFACE_ENABLED                          1
    #define CYBLE_INTERFACE_ENABLED                         0
    #define CYBLE_TESTS_ENABLED                             1
    #define MTK_DUT_ADDRESS                                 {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}

    #define WHO_AM_I                                        "DUT"
#endif  // CYBLE_MTK_DUT

#define DEFAULT_DTM_RX_TEST_DELAY                       625     // 1000 packets (in ms)
#define _1000_US                                        1000    // in us
#define UNIT_PACKET_TX_TIMEOUT                          625     // 1 packet (in us)
#define DEFAULT_DTM_TX_END_DELAY                        625     // 1000 packets (in ms)
#define DTM_TX_START_DELAY                              10      // in ms
#define DTM_RX_END_DELAY_BUFFER                         50      // in ms
#define DEFAULT_MTK_PACKET_LENGTH                       0x25
#define DEFAULT_MTK_PACKET_TYPE                         0x00

/* L2CAP Connection Parameters */
#if (CYBLE_MTK_HOST == 1)
    #define MTK_L2CAP_REMOTE_PSM_ID                         (0xF9)
    #define MTK_L2CAP_PSM_ID                                (0xF7)
#endif  // CYBLE_MTK_HOST

#if (CYBLE_MTK_DUT == 1)
    #define MTK_L2CAP_REMOTE_PSM_ID                         (0xF7)
    #define MTK_L2CAP_PSM_ID                                (0xF9)
#endif  // CYBLE_MTK_DUT

#define MTK_L2CAP_PSM_CREDIT_WATER_MARK                 (0x00)
#define MTK_L2CAP_MTU_SIZE                              (100)
#define MTK_L2CAP_MPS_SIZE                              (100)
#define MTK_L2CAP_CREDIT                                (65535)

/* BLE Modules states */
#define BLE_STATE_STOPPED                               ((uint8)CYBLE_STATE_STOPPED)
#define BLE_STATE_INITIALIZING                          ((uint8)CYBLE_STATE_INITIALIZING)
#define BLE_STATE_CONNECTED                             ((uint8)CYBLE_STATE_CONNECTED)
#define BLE_STATE_DISCONNECTED                          ((uint8)CYBLE_STATE_DISCONNECTED)
#define BLE_STATE_ADVERTISING                           ((uint8)CYBLE_STATE_ADVERTISING)
#define BLE_STATE_CONNECTED_PATH_LOSS                   ((uint8)0xFF)
#define BLE_STATE_DISCONNECTED_LINK_LOSS                ((uint8)0xFE)

#define ms_timer_stop                                   CySysTickStop

extern void     MTK_init(void);
extern uint8_t  MTK_mode(void);

extern void     set_packet_length(uint32_t length);
extern uint32_t get_packet_length(void);
extern void     set_packet_type(uint32_t type);
extern uint32_t get_packet_type(void);
extern void     DTM_tx_start(uint32_t channel, CYBLE_BLESS_PWR_LVL_T power, uint32_t num_packets);
extern void     DTM_rx_start(uint32_t channel);
extern void     stop_DTM_tests(void);
extern void     transmit_carrier_wave(uint32_t channel, CYBLE_BLESS_PWR_LVL_T power);
extern void     recover_from_TXC(void);
extern bool     WriteUserSFlashRow(uint8 userRowNumber, uint16 dataOffset, uint32 *dataBuffer, uint32 length);
extern bool     ReadUserSflash(uint8 userRowNumber, uint16 dataOffset, uint8 *dataPointer, uint16 dataSize);

#ifdef UART_CI_DEBUG
    extern void     UART_print_int(int32_t input);
#endif  // UART_CI_DEBUG

#endif  // __CYBLE_MTK_H__
/* [] END OF FILE */
