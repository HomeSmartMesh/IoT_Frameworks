/** @file ClockUartLed.h
 *
 * @author Wassim FILALI taken over from http://blog.mark-stevens.co.uk/
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 20.09.2015
 * $Revision:
 *
 */

#include "ClockUartLed.h"

#include <iostm8s103f3.h>

#define Test_Led_PB5 PB_ODR_ODR5

void InitialiseUART()
{
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
	UART1_BRR2 = 0x0a;      //  Set the baud rate registers to 115200 baud
	UART1_BRR1 = 0x08;      //  based upon a 16 MHz system clock.
	//
	//  Disable the transmitter and receiver.
	//
	UART1_CR2_TEN = 0;      //  Disable transmit.
	UART1_CR2_REN = 0;      //  Disable receive.
	//
	//  Set the clock polarity, lock phase and last bit clock pulse.
	//
	UART1_CR3_CPOL = 1;
	UART1_CR3_CPHA = 1;
	UART1_CR3_LBCL = 1;
	//
	//  Turn on the UART transmit, receive and the UART clock.
	//
	UART1_CR2_TEN = 1;
	UART1_CR2_REN = 1;
	UART1_CR3_CKEN = 1;
}

void putc(char c)
{
	UART1_DR = c;
	while (UART1_SR_TXE == 0);          //  Wait for transmission to complete.
}

//
//  Send a message to the debug port (UART1).
//
void UARTPrintf(char const *ch)
{
	while (*ch)
	{
	UART1_DR = (unsigned char) *ch;     //  Put the next character into the data transmission register.
	while (UART1_SR_TXE == 0);          //  Wait for transmission to complete.
	ch++;                               //  Grab the next character.
	}
}

void UARTPrintfLn(char const *ch)
{
  UARTPrintf(ch);
  putc('\r');
  putc('\n');
}

void UARTPrintf4Bits(unsigned char val4bits)
{
	char res;
	if(val4bits < 10)
	{
		res = '0' + val4bits;
	}
	else
	{
                val4bits = val4bits-10;
		res = 'A' + val4bits;
	}
	putc(res);
}

void UARTPrintfHex(unsigned char val)
{
	unsigned char MSB,LSB;
	putc('0');
	putc('x');
	LSB = val & 0x0F;
	MSB = (val>>4) & 0x0F;
	UARTPrintf4Bits(MSB);
	UARTPrintf4Bits(LSB);
}

void UARTPrintfHexTable(unsigned char *pval,unsigned char length)
{
    putc('0');
    putc('x');
  for(int i=0;i<length;i++)
  {
    unsigned char MSB,LSB;
    LSB = pval[i] & 0x0F;
    MSB = (pval[i]>>4) & 0x0F;
    UARTPrintf4Bits(MSB);
    UARTPrintf4Bits(LSB);
    putc(' ');
  }
  
}


void UARTPrintf_uint(unsigned int num)
{
    if (num == 0)
    {
        putc('0');
        return;
    }   
    
    if (num > 9) UARTPrintf_uint(num/10);
    putc('0'+ (num%10));
    
    /*while (num != 0)
    {
        putc ('0'+ (num%10));
        num /= 10;
    }*/
}

void UARTPrintf_sint(signed int num)
{
    if (num < 0)
    {
        putc('-');
        num = -num;
    }
    else if (num == 0)
    {
        putc('0');
        return;
    }   
    if (num > 9) UARTPrintf_uint(num/10);
    putc('0'+ (num%10));
}

void UARTPrintfHexLn(unsigned char val)
{
	unsigned char MSB,LSB;
	putc('0');
	putc('x');
	LSB = val & 0x0F;
	MSB = (val>>4) & 0x0F;
	UARTPrintf4Bits(MSB);
	UARTPrintf4Bits(LSB);
	putc('\n');
	putc('\r');
}


void InitialiseSystemClock()
{
	CLK_ICKR = 0;                       //  Reset the Internal Clock Register.
	CLK_ICKR_HSIEN = 1;                 //  Enable the HSI.
	CLK_ECKR = 0;                       //  Disable the external clock.
	while (CLK_ICKR_HSIRDY == 0);       //  Wait for the HSI to be ready for use.
	CLK_CKDIVR = 0;                     //  Ensure the clocks are running at full speed.
	CLK_PCKENR1 = 0xff;                 //  Enable all peripheral clocks.
	CLK_PCKENR2 = 0xff;                 //  Ditto.
	CLK_CCOR = 0;                       //  Turn off CCO.
	CLK_HSITRIMR = 0;                   //  Turn off any HSIU trimming.
	CLK_SWIMCCR = 0;                    //  Set SWIM to run at clock / 2.
	CLK_SWR = 0xe1;                     //  Use HSI as the clock source.
	CLK_SWCR = 0;                       //  Reset the clock switch control register.
	CLK_SWCR_SWEN = 1;                  //  Enable switching.
	while (CLK_SWCR_SWBSY != 0);        //  Pause while the clock switch is busy.
}
//
// Delay loop
//
// Actual delay depends on clock settings
// and compiler optimization settings.
//
void delay(unsigned int n)
{
    while (n-- > 0);
}

void delay_1ms_Count(unsigned int n)
{
    while (n-- > 0)
	{
		delay_1ms();
	}
}

#define Test_Led_PB5 PB_ODR_ODR5

void Initialise_TestLed_GPIO_B5()
{
      //D3 output
     //PD_DDR_bit.DDR3 = 1;
     //push pull
    //PD_CR1_bit.C13 = 1;
    //PD_ODR_bit.ODR3 = 1;

    //
    // Data Direction Register
    //
    // 0: Input
    // 1: Output
    //
    PB_DDR_bit.DDR5 = 1;
    //PC_DDR = 0xFF;
    
    //
    // Control Register 1
    //
    // Input mode:
    //   0: Floating input
    //   1: Input with pull-up
    //
    // Output mode:
    //   0: Pseudo open drain
    //   1: Push-pull
    //
    PB_CR1_bit.C15 = 0;
    //PC_CR1 = 0x00;
    
    //
    // Control Register 2
    //
    // Input mode:
    //   0: External interrupt disabled
    //   1: External interrupt enabled
    //
    // Output mode:
    //   0: Output speed up to  2 MHz
    //   1: Output speed up to 10 MHz
    //
    //PB_CR2_bit.C25 = 0;
}

void Test_Led_On()
{
  Test_Led_PB5 = 0;
}
void Test_Led_Off()
{
  Test_Led_PB5 = 1;
}
