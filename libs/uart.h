/** @file uart.h
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 20.09.2015
 * $Revision:
 *
 */

#include "commonTypes.h"

//First configure, important that this file is included before the other headers that will conditionally declare the extern variables
//#include "uart_config.h" for flags,...,UART_CALLBACK_POLLING
#include "uart_config.h"


//for InitialiseUART(); putc(char c);  printf(char const *message);
#include "uart_stm8x.h"

#define printf_byte	UARTPrintfHex
#define printf_hex	UARTPrintfHex
#define printf_uint	UARTPrintf_uint
#define printf_tab	UARTPrintfHexTable
#define printf_eol();	UARTPrintfLn("");
#define printf_ln(X);	UARTPrintfLn(X);

void UARTPrintfLn(char const *message);

void UARTPrintfHex(unsigned char val);

void UARTPrintfHexTable(unsigned char *pval,unsigned char length);

void UARTPrintfHexLn(unsigned char val);

void UARTPrintf_sint(signed int num);

void UARTPrintf_uint(U16_t num);

void print_data_tab(unsigned char *pval,unsigned char length);

#if UART_CALLBACK_POLLING == 1
void uart_rx_user_poll();
#endif
