/** @file nRF_Modes.c
 *
 * @author Wassim FILALI
 * Directly inferred from the Datasheet
 * 
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 12.12.2015
 * $Revision:
 *
 */

#include "nRF_Modes.h"
#include "nRF_Configuration.h"
#include "nRF_IRQ.h"

#include "nRF_SPI.h"
#include "nRF_LowLevel.h"

#include "nRF_RegText.h"

//for delay
#include "ClockUartLed.h"



BYTE nRF_Mode = nRF_Mode_Uninitialised;

//Waiting power on reset and initialising SPI
BYTE nRF_Config()
{
	BYTE status;
	BYTE ConfigVal = 0;
	
	if(nRF_Mode == nRF_Mode_Uninitialised)
	{
		nRF_Init();
	}

	SPI_Write_Register(EN_AA,SPI_Write_Register_EN_AA);//disable auto acknowledgement for all pipes
	SPI_Write_Register(SETUP_RETR,SPI_Write_Register_SETUP_RETR);//disable retransmission
	SPI_Write_Register(RX_PW_P0,SPI_Write_Register_RX_PW_P0);//set pipe 0 width to 1
	
	//read previous config so that PWR_UP stay unchanged
	ConfigVal = SPI_Read_Register(CONFIG);
	ConfigVal &= bit_Mask_Reserved;//whatever read on reserved, Only 0 is written
	
	//Mask the interrupts 
	ConfigVal |= nRF_Config_Interruptions_Mask_Set;
	ConfigVal &= nRF_Config_Interruptions_Mask_ReSet;
	
	status = SPI_Write_Register(CONFIG,ConfigVal);
	
	nRF_Config_Printf("nRF_Config\n\rConfig updated with : ");
	nRF_Config_PrintfHex(ConfigVal);
	nRF_Config_Printf(" ; reread ");
    ConfigVal = SPI_Read_Register(CONFIG);
	nRF_Config_PrintfHex(ConfigVal);
	nRF_Config_Printf(" ; ");
	nRF_Config_PrintStatus(status);

	#if(AtLeastOneIRQ_Is_Enabled == 1)
	nRF_IRQ_Config();//GPIO interrupt and enabling STM8 interrupts
	#endif
	
	if(		( status & (bit_RX_DR | bit_TX_DS | bit_MAX_RT) ) != 0	)
	{
		nRF_Config_Printf("Clearing the status, ");
		nRF_ClearStatus();
		nRF_Config_Printf("New ");
        status = SPI_Read_Register(STATUS);		
		nRF_Config_PrintStatus(status);
	}

	return status;
}

//Waiting power on reset and initialising SPI
BYTE nRF_Init()
{
	BYTE res = E_OK;
	if(nRF_Mode == nRF_Mode_Uninitialised)
	{
		//After Power on reset wait 100ms (nRF24L01P Product Specification 1.0 page22)
		delay_1ms_Count(100);
		SPI_Init();
		
		nRF_Mode = nRF_Mode_PowerDown;//mode in which SPI is active
	}
	else
	{
		res = E_Already_Initialised;
	}
	
	return res;
}

 
// ----------------------------- Power Down -----------------------------
//in Power down mode:
// - minimal power consumption
// - SPI kept active
BYTE nRF_SetMode_PowerDown()
{
	BYTE status;
	BYTE ConfigVal = 0;
	
	if(nRF_Mode == nRF_Mode_Uninitialised)
	{
		nRF_Init();
	}
	//For state machine coherency, let us keep this pin low when on Power Down Mode
	CE_Pin_LowDisable();
	
	ConfigVal = SPI_Read_Register(CONFIG);
	ConfigVal &= bit_Mask_Reserved;//whatever read on reserved, Only 0 is written
	
	ConfigVal &= bit_neg_PWR_UP;
	
	status = SPI_Write_Register(CONFIG,ConfigVal);

	nRF_Mode = nRF_Mode_PowerDown;
	
	return status;
}

// ----------------------------- Standby I -----------------------------
//in Standby I Mode:
// - minimize average current consumption
// - maintains short startup times
// go to active modes and back by setting the CE
//delay from PowerDown to Standby after setting PWR_UP depend on External Crystal Ls
BYTE nRF_SetMode_Standby_I()
{
	BYTE status;
	BYTE ConfigVal = 0;
	
	if(nRF_Mode == nRF_Mode_Uninitialised)
	{
		nRF_Init();
	}
	
	//If we enter here from Power Down, then CE already disabled, otherwise goes from 1 to 0
	CE_Pin_LowDisable();
	
	ConfigVal = SPI_Read_Register(CONFIG);
	ConfigVal &= bit_Mask_Reserved;//whatever read on reserved, Only 0 is written
	
	ConfigVal |= bit_PWR_UP;
	
	status = SPI_Write_Register(CONFIG,ConfigVal);

	//max delay is 4.5ms for Ls = 90mH (ToDo what is Ls value of nRF24L01P module ?)
	delay_1ms_Count(5);
	
	nRF_Mode = nRF_Mode_Standby;

	return status;
}
// ----------------------------- RX -----------------------------
//start demodulating and looking for a matching address and valid CRC
//if RX fifo is full packet is discarded
//Goes out of RX
// - when Standby or Power Down
// - when managed by Shockburst

BYTE nRF_SetMode_RX()
{
	BYTE status;
	BYTE ConfigVal = 0;
	
	ConfigVal = SPI_Read_Register(CONFIG);

	if((ConfigVal & bit_PWR_UP) == 0x00)//We are in Power Down mode, will have to set and wait 5ms
	{
		nRF_SetMode_Standby_I();
		ConfigVal = SPI_Read_Register(CONFIG);
	}
	ConfigVal &= bit_Mask_Reserved;//whatever read on reserved, Only 0 is written
	
	ConfigVal |= bit_PRIM_RX;//here setting the RX Mode Flag
	
	status = SPI_Write_Register(CONFIG,ConfigVal);
	
	CE_Pin_HighEnable();
	
	//RX settings takes 130us
	delay_100us();
	delay_10us();
	delay_10us();
	delay_10us();
	
	nRF_Mode = nRF_Mode_Rx;

	//Debug section
	#if (Enable_Debug_nRF_SetMode_RX == 1)
	nRF_SetMode_RX_Printf("SetMode_Rx: ");
	nRF_SetMode_RX_Printf("Config updated with : ");
	nRF_SetMode_RX_PrintfHex(ConfigVal);
	nRF_SetMode_RX_Printf(" ; reread ");
    ConfigVal = SPI_Read_Register(CONFIG);
	nRF_SetMode_RX_PrintfHex(ConfigVal);
	nRF_SetMode_RX_Printf(" ; ");
	status = SPI_Read_Register(STATUS);			
	nRF_SetMode_RX_PrintStatus(status);
	#endif
	
	
	return status;
}


// ----------------------------- Standby II ----------------------------
//note that setting CE with TX fifo empty goes to StandbyII
//   then filling TX fifo is the same as filling it before StandbyII.
//   The TX Settings is always a necessary with 130 us

// ----------------------------- TX -----------------------------
BYTE nRF_SetMode_TX()
{
	BYTE status,fifos;
	BYTE ConfigVal = 0;

	ConfigVal = SPI_Read_Register(CONFIG);

	if((ConfigVal & bit_PWR_UP) == 0x00)//We are in Power Down mode, will have to set and wait 5ms
	{
		nRF_SetMode_Standby_I();
		ConfigVal = SPI_Read_Register(CONFIG);
	}
	ConfigVal &= bit_Mask_Reserved;//whatever read on reserved, Only 0 is written

	ConfigVal &= bit_neg_PRIM_RX;//clear PRIM_RX for TX
	
	status = SPI_Write_Register(CONFIG,ConfigVal);
	
	fifos = SPI_Read_Register(FIFO_STATUS);
	
	if((fifos & bit_TX_EMPTY)== 0)//data is in Tx Fifo
	{
		//TX settings takes 130us
		delay_100us();
		delay_10us();
		delay_10us();
		delay_10us();
	}//else we are not in TX but in Standby II mode

	nRF_Mode = nRF_Mode_Tx;

	return status;
}

BYTE nRF_ClearStatus()
{
	return SPI_Write_Register(STATUS,(bit_RX_DR | bit_TX_DS | bit_MAX_RT) );
}

