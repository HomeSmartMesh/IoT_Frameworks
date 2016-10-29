/** @file rx_protocol.c
 *
 * @author Wassim FILALI  STM8L
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 29.10.2016 - creation
 * $Revision:
 *
*/

//for BYTE
#include "commonTypes.h"

//for printf
#include "uart.h"



void rx_alive(BYTE *rxData,BYTE rx_DataSize)
{
	BYTE crc = rxData[0] ^ rxData[1];
	if(crc == rxData[2])
	{
		printf("NodeId:");
		UARTPrintf_uint(rxData[1]);
		printf(",is:Alive\r\n");
	}
	else
	{
		printf("Protocol Id: 0x75, CRC Fail\n");
	}
}

void rx_light(BYTE *rxData,BYTE rx_DataSize)
{
	unsigned int SensorVal;
	BYTE crc = rxData[0] ^ rxData[1];
	if(crc == rxData[4])
	{
	  printf("NodeId:");
	  UARTPrintf_uint(rxData[1]);
	  printf(",Light: ");
	  SensorVal = rxData[2];
	  SensorVal <<= 4;//shift to make place for the 4 LSB
	  SensorVal = SensorVal + (0x0F & rxData[3]);
	  UARTPrintf_uint(SensorVal);
	  printf("\n");
	}
	else
	{
	  printf("Protocol Id: 0x3B, CRC Fail\n");
	}
}

void rx_magnet(BYTE *rxData,BYTE rx_DataSize)
{
	BYTE crc = rxData[0] ^ rxData[1] ^ rxData[2];
	if(crc == rxData[3])
	{
		printf("NodeId:");
		UARTPrintf_uint(rxData[1]);
		printf(",is:");
		if(rxData[2] == 0)
		{
			printf("Low\r\n");
		}
		else
		{
			printf("High\r\n");
		}
	}
    else
    {
      printf("Protocol Id: 0xC5, CRC Fail\r\n");
    }
}
