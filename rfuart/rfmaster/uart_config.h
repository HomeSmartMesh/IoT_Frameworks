/** @file uart_config.h
 *
 * @author Wassim FILALI
 * 
 *
 *
 * $Date: 12.11.2016
 * $Revision:
 *
 */


//----------------------------------- Interruptions ---------------------------------------
//Flag: UART_USE_RX_INETRRUPT
// 1 : Enabled
// declares the IRQ and requires teh user to declare a uart_callback() function

#define UART_USE_RX_INETRRUPT 1

#define UART_FRAME_SIZE 32

#define UART_EOF_C	13
