/** @file uart.c
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

//---------------------------- CONFIG ----------------------------
#include "deviceType.h"

#include "uart_config.h"

//---------------------------- rest of includes ----------------------------
#include "uart.h"

#include "clock_led.h"

void UARTPrintfLn(char const *ch)
{
	printf(ch);
	putc(UART_EOL_C);
	#if UART_Through_RF_Delay == 1
	//require at least 7 ms to process the reception from the end side
	//expect longer delay for longer commands
	delay_ms(50);
	#endif
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

void print_data_tab(unsigned char *pval,unsigned char length)
{
  for(int i=0;i<length;i++)
  {
    putc(pval[i]);
  }
}

void UARTPrintf_uint(U16_t num)
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
	putc(UART_EOL_C);
}


