/** @file cmdutils.c

 @author Wassim FILALI  STM8 S

 @compiler IAR STM8


 $Date: 18.11.2016
 $Revision:

*/

#include "cmdutils.h"

BYTE strcmp (BYTE * s1, const char * s2)
{
    for(; *s1 == *s2; ++s1, ++s2)
        if(*s1 == 0)
            return 0;
    return *(unsigned char *)s1 < *(unsigned char *)s2 ? -1 : 1;
}

BYTE strbegins (BYTE * s1, const char * s2)
{
    for(; *s1 == *s2; ++s1, ++s2)
        if(*s2 == 0)
            return 0;
    return (*s2 == 0)?0:1;
}

BYTE get_hex_char(BYTE c)
{
	BYTE res = 0;
	if(c <= '9')
	{
		res = c - '0';
	}
	else if(c <= 'F')
	{
		res = c - 'A' + 10;
	}
	else if(c <= 'f')
	{
		res = c - 'a' + 10;
	}
	return res;
}

BYTE get_hex(BYTE* buffer,BYTE pos)
{
	BYTE hex_val;
	pos+=2;//skip "0x"
	hex_val = get_hex_char(buffer[pos++]);
	hex_val <<= 4;
	hex_val |= get_hex_char(buffer[pos]);
	return hex_val;
}

BYTE line_length(BYTE*rxData,BYTE max_size)
{
	for(BYTE i=0;i<max_size;i++)
	{
		if(*rxData++ == '\n')
		{
			return i+1;//+1 is to keep the '\n'
		}
	}
	return max_size;
}
