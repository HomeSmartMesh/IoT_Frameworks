/** @file nRF_RegText.c
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 * $Date: 13.12.2015
 * $Revision:
 *
 */

#include "nRF_RegText.h"
#include "nRF_SPI.h"
#include "ClockUartLed.h"

void nRF_PrintStatus(BYTE status)
{
	UARTPrintf("STATUS: ");
	UARTPrintfHex(status);
	UARTPrintf(" : ");

	if(status & bit_RX_DR)
	{
		UARTPrintf("Data Ready i; ");
	}
	if(status & bit_TX_DS)
	{
		UARTPrintf("Data Sent i; ");
	}
	if(status & bit_MAX_RT)
	{
		UARTPrintf("Max Ret i; ");
	}
	if(	(status & bit_Mask_RXPNO) == bit_RXPNO_NotUsed)
	{
		UARTPrintf("Rx Pipe not used ; ");
	}
	else if(	(status & bit_Mask_RXPNO) == bit_RXPNO_RxFFEmpty)
	{
		//UARTPrintf("Rx Fifo Empty ; ");// this is the default state not log for it
	}
	else 
	{
		BYTE PipeNb = (status & bit_Mask_RXPNO)>>1;
		UARTPrintf("Rx Pipe Nb ");
		UARTPrintfHex(PipeNb);
		UARTPrintf(" ; ");
	}
	if(status & bit_TX_FULL)
	{
		UARTPrintf("TX Fifo Full;");
	}
	UARTPrintf("\n\r");
}

void nRF_PrintConfig()
{
	
}
