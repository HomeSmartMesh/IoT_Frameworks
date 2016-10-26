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
#include "uart.h"

void nRF_PrintStatus(BYTE status)
{
	printf("STATUS: ");
	UARTPrintfHex(status);
	printf(" : ");

	if(status & bit_RX_DR)
	{
		printf("Data Ready i; ");
	}
	if(status & bit_TX_DS)
	{
		printf("Data Sent i; ");
	}
	if(status & bit_MAX_RT)
	{
		printf("Max Ret i; ");
	}
	if(	(status & bit_Mask_RXPNO) == bit_RXPNO_NotUsed)
	{
		printf("Rx Pipe not used ; ");
	}
	else if(	(status & bit_Mask_RXPNO) == bit_RXPNO_RxFFEmpty)
	{
		//printf("Rx Fifo Empty ; ");// this is the default state not log for it
	}
	else 
	{
		BYTE PipeNb = (status & bit_Mask_RXPNO)>>1;
		printf("Rx Pipe Nb ");
		UARTPrintfHex(PipeNb);
		printf(" ; ");
	}
	if(status & bit_TX_FULL)
	{
		printf("TX Fifo Full;");
	}
	printf("\n\r");
}

void nRF_PrintConfig()
{
	
}
