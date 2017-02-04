/** @file uart_stm8x.c
 *
 * @author Wassim FILALI  STM8L
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 26.10.2016
 * $Revision:
 *
 */

//---------------------------- TYPES ----------------------------
//Important include dependency should be the independent files first, and the file that use them after !!!
//for BYTE
#include "commonTypes.h"

//---------------------------- CONFIG ----------------------------
//important include configuration before including files using this configuration
//to configure the UART RX
#include "uart_config.h"
//to know which DEVICE_STM8 we have
#include "deviceType.h"

//---------------------------- rest of includes ----------------------------
#include "uart_stm8x.h"



#if DEVICE_STM8L == 1
	#include <iostm8l151f3.h>
#elif DEVICE_STM8S == 1
	#include <iostm8s103f3.h>
#else
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif

//this file is valid for both deviceTypes
#if UART_USE_RX_INETRRUPT == 1
#include <intrinsics.h>

void uart_rx_user_callback(BYTE *buffer,BYTE size);

#endif


#if DEVICE_STM8L == 1

void uart_init_impl()
{
        //Enable USART Peripheral Clock
        CLK_PCKENR1_PCKEN15 = 1;
	//  Reset the UART registers to the reset values.
	//
	USART1_CR1 = 0;
	USART1_CR2 = 0;
	USART1_CR4 = 0;
	USART1_CR3 = 0;
	USART1_CR5 = 0;
	USART1_GTR = 0;
	USART1_PSCR = 0;
	//
	//  Now setup the port to 115200,n,8,1.
	//
	USART1_CR1_M = 0;        //  8 Data bits.
	USART1_CR1_PCEN = 0;     //  Disable parity.
	//CR3_STOP = 0 =>  1 stop bit.
        
        //BR1 [11:4]=0x08 BR2[15:12 - 3:0]=0x0a
        //0x008a => 138       
        //  Set the baud rate registers to 115200 baud 
        //  based upon a 16 MHz system clock.
	USART1_BRR2 = 0x0B;      //  0xB from Datasheet - Set the baud rate registers to 115200 baud
	USART1_BRR1 = 0x08;      //  based upon a 16 MHz system clock.
	//
	//  Disable the transmitter and receiver.
	//
	USART1_CR2_TEN = 0;      //  Disable transmit.
	USART1_CR2_REN = 0;      //  Disable receive.
	//
	//  Set the clock polarity, lock phase and last bit clock pulse.
	//
	USART1_CR3_CPOL = 0;//1
	USART1_CR3_CPHA = 0;//1
	USART1_CR3_LBCL = 0;//1
	//
	//  Turn on the UART transmit, receive and the UART clock.
	//
	USART1_CR2_TEN = 1;
	USART1_CR2_REN = 0;
	USART1_CR3_CLKEN = 0;//Clock Pin Disabled (UART not USART)
}

void putc_impl(char c)
{
	while (USART1_SR_TXE == 0);          //  Wait for transmission to complete.
	USART1_DR = c;
}

//
//  Send a message to the debug port (UART1).
//
void printf_impl(char const *ch)
{
	while (*ch)
	{
	while (USART1_SR_TXE == 0);          //  Wait for transmission to complete.
	USART1_DR = (unsigned char) *ch;     //  Put the next character into the data transmission register.
	ch++;                               //  Grab the next character.
	}
}

#elif DEVICE_STM8S == 1
void uart_init_impl()
{
	//Enable UART Clock Peripheral
	CLK_PCKENR1 |= 0x20;// USART1
	//
	//  Clear the Idle Line Detected bit in the status register by a read
	//  to the UART1_SR register followed by a Read to the UART1_DR register.
	//
	unsigned char tmp = UART1_SR;
	tmp = UART1_DR;
	//
	//  Reset the UART registers to the reset values.
	//
	UART1_CR1 = 0;
	UART1_CR2 = 0;
	UART1_CR4 = 0;
	UART1_CR3 = 0;
	UART1_CR5 = 0;
	UART1_GTR = 0;
	UART1_PSCR = 0;
	//
	//  Now setup the port to 115200,n,8,1.
	//
	UART1_CR1_M = 0;        //  8 Data bits.
	UART1_CR1_PCEN = 0;     //  Disable parity.
	UART1_CR3_STOP = 0;     //  1 stop bit.
	UART1_BRR2 = 0x0B;      //  0xB from Datasheet - Set the baud rate registers to 115200 baud
	UART1_BRR1 = 0x08;      //  based upon a 16 MHz system clock.
	//
	//  Disable the transmitter and receiver.
	//
	UART1_CR2_TEN = 0;      //  Disable transmit.
	UART1_CR2_REN = 0;      //  Disable receive.

	//
	//  Turn on the UART transmit, receive and the UART clock.
	//
	UART1_CR2_TEN = 1;
	UART1_CR2_REN = 1;
	UART1_CR3_CKEN = 0;//Clock Pin Disabled (UART not USART)
	
	#if UART_USE_RX_INETRRUPT == 1
    UART1_CR2_RIEN = 1;     // Enable UART Rx Interrupts
    __enable_interrupt();
	#endif
}

void putc_impl(char c)
{
	while (UART1_SR_TXE == 0);          //  Wait for transmission to complete.
	UART1_DR = c;
}

//
//  Send a message to the debug port (UART1).
//
void printf_impl(char const *ch)
{
	while (*ch)
	{
	while (UART1_SR_TXE == 0);          //  Wait for transmission to complete.
	UART1_DR = (unsigned char) *ch;     //  Put the next character into the data transmission register.
	ch++;                               //  Grab the next character.
	}
}

#if UART_USE_RX_INETRRUPT == 1
BYTE uart_BUFFER[UART_FRAME_SIZE];
BYTE uart_index = 0;
BYTE uart_oveflow = 0;
BYTE uart_echo = 1;

	#if UART_CALLBACK_POLLING == 1
	BYTE uart_rx_user_callback_pending = 0;
	BYTE uart_rx_user_callback_performed = 0;
	#endif

#pragma vector = UART1_R_RXNE_vector
__interrupt void uart_irq(void)
{
	BYTE rx;
	#if UART_CALLBACK_POLLING == 1
	if(uart_rx_user_callback_performed)
	{
		uart_rx_user_callback_performed = 0;
		uart_index = 0;
	}
	#endif
	
	
	if((UART1_SR&0x0F) != 0x00)//Any of the errors
	{
		UART1_SR = 0;
		rx = UART1_DR;//read DR to clear the errors
	}
	else
	{
		while(UART1_SR_RXNE)
		{
			//keep overwriting last Byte on overflow
			if(uart_index>=UART_FRAME_SIZE)
			{
				uart_oveflow = 1;
				uart_index--;
			}
			rx = UART1_DR;
			if(uart_echo == 1)
			{
				//reflect typed characters
				while (UART1_SR_TXE == 0);          //  Wait for transmission to complete.
				UART1_DR = rx;
			}
			//manage control characters
			if(rx == 0x7F)//DEL
			{
				uart_index--;
			}
			else
			{
				uart_BUFFER[uart_index] = rx;
				//after the next line increment, the index reflexts the data size for this cycle
				uart_index++;
			}
		}

		//Frame complete condition
		if(rx == UART_EOL_C)
		{
			if(uart_oveflow)
			{
				uart_oveflow = 0;//discards all data as it is a useless corrupted format
				uart_index = 0;//restart from the beginning
			}
			else
			{
				//as this must be quite time consuming, it is left to the user the responsibility to call it from another context
				#if UART_CALLBACK_POLLING == 1
				uart_rx_user_callback_pending = 1;
				#else
				uart_rx_user_callback(uart_BUFFER,uart_index);
				uart_index = 0;
				#endif
			}
		}
	}

}

#endif


#else  
	#error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif
