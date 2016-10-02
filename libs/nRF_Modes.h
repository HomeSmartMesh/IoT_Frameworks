/** @file nRF_Modes.h
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

//for BYTE
#include "commonTypes.h"
 

#define nRF_Mode_Uninitialised	0x00
#define nRF_Mode_PowerDown		0x01
#define nRF_Mode_Standby 		0x02
#define nRF_Mode_Tx				0x03
#define nRF_Mode_Rx 			0x04

BYTE nRF_Config();

BYTE nRF_Init();//Waiting power on reset and initialising SPI
 
// The Radio control operational modes are following the description from
// nRF24L01P Product Specification 1.0 page 21

BYTE nRF_SetMode_PowerDown();

BYTE nRF_SetMode_Standby_I();

BYTE nRF_SetMode_RX();

BYTE nRF_SetMode_TX();

BYTE nRF_ClearStatus();



//Global variable to manage the current mode

extern BYTE nRF_Mode;
extern BYTE ConfigVal;
