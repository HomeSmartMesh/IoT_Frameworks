/** @file uart_stm8x.h
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 26.10.2016
 * $Revision:
 *
 */

void uart_init();

void putc(char c);

void printf(char const *message);

//if interrupt mode, otherwise not declared
#if UART_USE_RX_INETRRUPT == 1
		extern	BYTE uart_echo;
	//published externally only if the user needs to be polling himself
		//#include "uart_config.h" for UART_CALLBACK_POLLING and then conditionally for UART_FRAME_SIZE
	#include "uart_config.h"
	#if UART_CALLBACK_POLLING == 1
		
		extern	BYTE uart_rx_user_callback_pending;
		extern	BYTE uart_rx_user_callback_performed;
		extern	BYTE uart_BUFFER[UART_FRAME_SIZE];
		extern	BYTE uart_index;
	#endif
#endif
