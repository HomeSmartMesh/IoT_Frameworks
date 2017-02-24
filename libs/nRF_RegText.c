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
#include "spi_stm8x.h"
#include "nRF.h"

void nRF_PrintStatus(BYTE status)
{
	printf("STATUS: ");
	printf_hex(status);
	printf(" : ");

	if(status & bit_RX_DR)
	{
		printf("Data Ready i; ");
	}
	else
	{
		printf("No Data Ready i; ");
	}
		
	if(status & bit_TX_DS)
	{
		printf("Data Sent i; ");
	}
	else
	{
		printf("No Data Sent i; ");
	}
	if(status & bit_MAX_RT)
	{
		printf("Max Ret i; ");
	}
	else
	{
		printf("No Max Ret i; ");
	}
	if(	(status & bit_Mask_RXPNO) == bit_RXPNO_NotUsed)
	{
		printf("Rx Pipe not used ; ");
	}
	else if(	(status & bit_Mask_RXPNO) == bit_RXPNO_RxFFEmpty)
	{
		printf("Rx Fifo Empty ; ");
	}
	else
	{
		BYTE PipeNb = (status & bit_Mask_RXPNO)>>1;
		printf("Rx Pipe Nb ");
		printf_hex(PipeNb);
		printf(" ; ");
	}
	if(status & bit_TX_FULL)
	{
		printf("TX Fifo Full;");
	}
	else
	{
		printf("Available TX Fifo Locations;");
	}
	printf_eol();
}

void nRF_PrintConfig(BYTE config)
{
	printf("CONFIG: ");
	printf_hex(config);
	printf(" : ");

	if((config & bit_MASK_RX_DR) == bit_MASK_RX_DR )
	{
		printf("RX IRQ Masked; ");
	}
	else
	{
		printf("RX IRQ Active; ");
	}
	if((config & bit_MASK_TX_DS) == bit_MASK_TX_DS )
	{
		printf("TX IRQ Masked; ");
	}
	else
	{
		printf("TX IRQ Active; ");
	}
	if((config & bit_MASK_MAX_RT) == bit_MASK_MAX_RT )
	{
		printf("Max RT IRQ Masked; ");
	}
	else
	{
		printf("Max RT IRQ Active; ");
	}

	if((config & bit_EN_CRC) == bit_EN_CRC )
	{
		printf("CRC Enabled EN_AA; ");
	}
	else
	{
		printf("CRC Disabled; ");
	}
	if((config & bit_CRCO) == bit_CRCO )
	{// => 1
		printf("CRC 2 Byte; ");
	}
	else
	{
		printf("CRC 1 Bytes; ");
	}
	if((config & bit_PWR_UP) == bit_PWR_UP )
	{
		printf("Power Up; ");
	}
	else
	{
		printf("Power Down; ");
	}
	if((config & bit_PRIM_RX) == bit_PRIM_RX )
	{
		printf("P RX Mode; ");
	}
	else
	{
		printf("P TX Mode; ");
	}
	printf_eol();
}

void nRF_PrintAddW()
{
	BYTE addw = SPI_Read_Register(SETUP_AW);
	printf("Address Width : ");
	if((addw & bit_MASK_SETUP_AW) == 0x01 )
	{
		printf_ln("3 bytes");
	}
	else if((addw & bit_MASK_SETUP_AW) == 0x02 )
	{
		printf_ln("4 bytes");
	}
	else if((addw & bit_MASK_SETUP_AW) == 0x03 )
	{
		printf_ln("5 bytes");
	}
}

void nRF_PrintInfo()
{
	nRF_PrintStatus(SPI_Read_Register(STATUS));
	nRF_PrintConfig(SPI_Read_Register(CONFIG));
	printf_ln((CE_Pin_getstate() == 1)?"ChipEnable High":"ChipEnable Low");
	nRF_PrintChannel();
	nRF_PrintAddW();
	printf("Rx Address P0: ");printf_hex(nRF_GetRxAddress(0));printf_eol();
	printf("Tx Address   : ");printf_hex(nRF_GetTxAddress());printf_eol();
}
