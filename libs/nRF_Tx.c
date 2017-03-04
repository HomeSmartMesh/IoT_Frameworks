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

//----------------------- config includes -----------------------
#include "nRF_Configuration.h"
//-------------------------- includes ---------------------------
#include "nRF_Tx.h"
//for nRF_Mode
#include "nRF.h"
//for SPI_Write_Buf
#include "nRF_SPI.h"
#include "clock_led.h"
#include "spi_stm8x.h"

//returns the status byte
BYTE nRF_Transmit(BYTE* payload, BYTE size)
{
	BYTE status;
	//as per definition in http://www.technolab.ddns.net/display/SSN/RF-UART
	//as long as the length cannot be explicitely retreived
	//it will be available at the 32nd and last location
	if(size>=RF_MAX_DATASIZE)
	{
		//non supported size, do nothing
		return 0;
	}
	
	//this will intrrupt any previously on going or blocked Tx (lost link)
	//This Flush is dubtfull, not necessary
	//and breaking quick successive transmissions
	status = SPI_Command(FLUSH_TX);

	//Assert Data Sent before new transmission to poll TX status
	status |= bit_TX_DS;
	SPI_Write_Register(STATUS,status);
	
	if(nRF_Mode != nRF_Mode_Tx)
	{
		nRF_SetMode_TX();
	}
	status = SPI_Write_Buf(WR_TX_PLOAD,payload,size);
	
	//This pin setting do not support multiple successive transmissions
	//that fill tx data in multiple buffers
	//because once first buffer is sent, a check is made on CE
	//to see if second continues
	//the bad case is write two buffers and give two pulses while
	//the first tx is still ongoing, then it stops as CE is low
	CE_Pin_HighEnable();//pulse for more than 10 us
	delay_10us();
	delay_10us();
	CE_Pin_LowDisable();

	return status;
}

BYTE nRF_Transmit_Wait_Down(BYTE* payload, BYTE size)
{
	BYTE status;
	status = nRF_Transmit(payload,size);
	nRF_Wait_Transmit();
	nRF_SetMode_PowerDown();

	return status;
}

BYTE nRF_Transmit_Wait_Rx(BYTE* payload, BYTE size)
{
	BYTE status;
	status = nRF_Transmit(payload,size);
	nRF_Wait_Transmit();
	nRF_SetMode_RX();

	return status;
}

//waits for Tx by polling STATUS bit TX_DS, counts till 255 then comes back
BYTE nRF_Wait_Transmit()
{
	BYTE cycles = 0;
	BYTE status;
	do
	{
		status = SPI_Read_Register(STATUS);
		cycles++;
	}while(((status &bit_TX_DS) == 0)&(cycles!=255));
	
	return cycles;
}