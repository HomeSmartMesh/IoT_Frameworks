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
        //As CE is low, will alwasy lead to Standby, while nRF_Mode could be Standby_II
	status = SPI_Command(FLUSH_TX,0x00);
	//unused result status

        //Write the buffer to make sure we can go in Tx Mode
	status = SPI_Write_Buf(WR_TX_PLOAD,payload,size);

        
        if (nRF_Mode == nRF_Mode_Rx)//To switch from Rx to TX CE should go Low
        {
          CE_Pin_LowDisable();
          delay_10us();//minimal transition delay
          nRF_Try_SetMode_TX();//Switch to TX : PRIM_RX = 0
        }
        
        //Create favorable conditions for Transmitting
        CE_Pin_HighEnable();//pulse for more than 10 us
        delay_10us();
        delay_10us();

        //if PRIM_RX == 1
        if ( (nRF_Mode != nRF_Mode_Tx) && (nRF_Mode != nRF_Mode_Standby_TX) )
        {
          //this function will wait for the 130 us as the fifo is not empty
          nRF_Try_SetMode_TX();
        }
        else//as TX_Mode was deprecated any way, we just do not need TX configuration
        {
          // TX Settings

          delay_100us();
          delay_10us();
          delay_10us();
          delay_10us();
          nRF_Mode = nRF_Mode_Tx;
          
        }
        
        //in some time
        //TX finished with one packet CE == 0 -> Standby_TX

        CE_Pin_LowDisable();
        
        
	return status;
}
