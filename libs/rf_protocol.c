/** @file rf_protocol.c
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

#include "rf_protocol.h"

//for printf
#include "uart.h"



void rf_get_tx_alive_3B(BYTE NodeId, BYTE* tx_data)
{
      tx_data[0]= rf_pid_0xF5_alive;
      tx_data[1]= NodeId;
      tx_data[2]= tx_data[0] ^ NodeId;
}

//Rx 3 Bytes
void rx_alive(BYTE src_NodeId)
{
	printf("NodeId:");
	UARTPrintf_uint(src_NodeId);
	printf_ln(";is:Alive");
}

// Reset
void rf_get_tx_reset_3B(BYTE NodeId, BYTE* tx_data)
{
      tx_data[0]= rf_pid_0xC9_reset;
      tx_data[1]= NodeId;
      tx_data[2]= tx_data[0] ^ NodeId;
}
void rx_reset(BYTE src_NodeId)
{
	printf("NodeId:");
	UARTPrintf_uint(src_NodeId);
	printf_ln(";was:Reset");
}

//Rx 5 Bytes
void rx_light(BYTE src_NodeId,BYTE *rxPayload,BYTE rx_PayloadSize)
{
	unsigned int SensorVal;
	printf("NodeId:");
	printf_uint(src_NodeId);
	printf(";Light:");
	SensorVal = rxPayload[0];
	SensorVal <<= 4;//shift to make place for the 4 LSB
	SensorVal = SensorVal + (0x0F & rxPayload[1]);
	printf_uint(SensorVal);
	printf_eol();
}

//Rx 4 Bytes
void rx_magnet(BYTE src_NodeId,BYTE *rxPayload,BYTE rx_PayloadSize)
{
	printf("NodeId:");
	printf_uint(src_NodeId);
	printf(";Magnet:");
	if(rxPayload[0] == 0)
	{
		printf_ln("Low");
	}
	else
	{
		printf_ln("High");
	}
}
