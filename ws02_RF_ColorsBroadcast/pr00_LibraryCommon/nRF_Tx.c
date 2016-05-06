/** @file nRF_Tx.c
 *
 * @author Wassim FILALI
 * Inferred from the Datasheet
 * 
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 12.12.2015
 * $Revision:
 *
 */

#include "nRF_Tx.h"
//for nRF_Mode
#include "nRF_Modes.h"
//for SPI_Write_Buf
#include "nRF_SPI.h"
#include "ClockUartLed.h"
#include "nRF_LowLevel.h"

//returns the status byte
BYTE nRF_Transmit(BYTE* payload, BYTE size)
{
	BYTE status;
	status = SPI_Command(FLUSH_TX,0x00);
	//unused result status
	
	if(nRF_Mode != nRF_Mode_Tx)
	{
		nRF_SetMode_TX();
	}
	status = SPI_Write_Buf(WR_TX_PLOAD,payload,size);
	
	CE_Pin_HighEnable();//pulse for more than 10 us
	delay_10us();
	delay_10us();
	CE_Pin_LowDisable();

	return status;
}
