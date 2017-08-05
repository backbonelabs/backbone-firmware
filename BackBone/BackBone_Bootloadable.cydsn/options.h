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

#ifndef OPTIONS_H_
#define OPTIONS_H_

#define NO  (0u)
#define YES (1u)

#define DEBUG_UART_ENABLED (YES)

/* YES - use printf, NO - use UART_PutString
 * e.g. DBG_PRINTF("message %d\r\n", i) will transform to UART_PutString("message %d\r\n") */
#define DEBUG_UART_USE_PRINTF_FORMAT (NO)

#define PRINT_BOUNDING_DATA (NO)

//#define ENABLE_MTK

#endif /* OPTIONS_H_ */
