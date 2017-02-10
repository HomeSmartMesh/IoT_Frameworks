/** @file nRF_Configuration.h
 *
 * @author Wassim FILALI
 * 
 *
 *
 * $Date: 17.12.2015
 * $Revision:
 *
 */

//-------------------------------------------------------------------------------------------------
//----------------------------------- User Editable Section ---------------------------------------
//-------------------------------------------------------------------------------------------------

//----------------------------------- Interruptions ---------------------------------------
//whether to use the Reception IRQ 1, if not then 0
#define	Enable_RX_IRQ	0
//Transmission IRQ
#define	Enable_TX_IRQ	0
//Retransmission IRQ
#define	Enable_RT_IRQ	0

//----------------------------- nRF Communication Configuration ---------------------------------
//data size, from 1 till 32 bytes payload
#define RX_DataSize	1
//disable auto acknowledgement for all pipes
#define SPI_Write_Register_EN_AA		0x00
//disable retransmission
#define SPI_Write_Register_SETUP_RETR	0x00
 
//--------------------------------Debug log Enable or disable-----------------------------------
// set to 1 or 0
#define Enable_Debug_IRQHandler_PortD_nRF 		        0
#define Enable_Debug_nRF_Config 				0
#define Enable_Debug_nRF_SetMode_RX				0
//-------------------------------------------------------------------------------------------------
//-------------------------------- End of User Editable Section -----------------------------------
//-------------------------------------------------------------------------------------------------


//--------Configuration inherited from previous parameters not to be edited -----------------
//Sets the Pipe 0 width
#define SPI_Write_Register_RX_PW_P0		RX_DataSize
//add the mask to the or bits to disable it. Masks are : bit_MASK_RX_DR | bit_MASK_TX_DS | bit_MASK_MAX_RT
#if(Enable_RX_IRQ == 1)
#define Enable_RX_IRQ_Config	0x00
#else
#define Enable_RX_IRQ_Config	bit_MASK_RX_DR
#endif
#if(Enable_TX_IRQ == 1)
#define Enable_TX_IRQ_Config	0x00
#else
#define Enable_TX_IRQ_Config	bit_MASK_TX_DS
#endif
#if(Enable_RT_IRQ == 1)
#define Enable_RT_IRQ_Config	0x00
#else
#define Enable_RT_IRQ_Config	bit_MASK_MAX_RT
#endif

#define nRF_Config_Interruptions_Mask_Set	( Enable_RX_IRQ_Config | Enable_TX_IRQ_Config | Enable_RT_IRQ_Config)
#define nRF_Config_Interruptions_Mask_ReSet	(bit_neg_EN_CRC)

#if( (Enable_RX_IRQ == 1) || (Enable_TX_IRQ == 1) || (Enable_RT_IRQ == 1) )
#define AtLeastOneIRQ_Is_Enabled	1
#else
#define AtLeastOneIRQ_Is_Enabled	0	
#endif
//-----------------------------------------------------------------------------------------

//-----------------------nRF_Modes configuration application, not to be edited------------
//----------------------------------------------------------------------------------------
#if (Enable_Debug_nRF_Config == 1)
#define nRF_Config_Printf(x) 			UARTPrintf(x)
#define nRF_Config_PrintfHex(x)	        UARTPrintfHex(x)
#define nRF_Config_PrintStatus(x)		nRF_PrintStatus(x)
#else
#define nRF_Config_Printf(x) 			(void)0
#define nRF_Config_PrintfHex(x)	        (void)0
#define nRF_Config_PrintStatus(x)       (void)0
#endif

#if (Enable_Debug_nRF_SetMode_RX == 1)
#define nRF_SetMode_RX_Printf(x) 		UARTPrintf(x)
#define nRF_SetMode_RX_PrintfHex(x)		UARTPrintfHex(x)
#define nRF_SetMode_RX_PrintStatus(x)	nRF_PrintStatus(x)
#else
#define nRF_SetMode_RX_Printf(x) 		(void)0
#define nRF_SetMode_RX_PrintfHex(x)		(void)0
#define nRF_SetMode_RX_PrintStatus(x)   (void)0
#endif
//----------------------------------------------------------------------------------------

//configuration application section, not to be edited
//---------------------------------------------------------------------------------------------
#if (Enable_Debug_IRQHandler_PortD_nRF == 1)
#define IRQ_Printf(x) 					UARTPrintf(x)
#define IRQ_PrintfHex(x)				UARTPrintfHex(x)
#define IRQ_PrintStatus(x)				nRF_PrintStatus(x)
#else
#define IRQ_Printf(x) 					(void)0
#define IRQ_PrintfHex(x)				(void)0
#define IRQ_PrintStatus(x)				(void)0
#endif
//---------------------------------------------------------------------------------------------
