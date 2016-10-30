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

#include "deviceType.h"

#include "uart.h"


void UARTPrintfLn(char const *ch)
{
  printf(ch);
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
	putc('\n');
}

