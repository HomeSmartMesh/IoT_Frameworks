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
//if the following flag is enabled then the user must instanciate the call back userRxCallBack(BYTE *rxData,BYTE rx_DataSize);
#define	Enable_RX_IRQ	0
//Transmission IRQ
#define	Enable_TX_IRQ	0
//Retransmission IRQ
#define	Enable_RT_IRQ	0
//CRC - default CRC is one byte
#define	Enable_CRC  	0

//----------------------------- nRF Communication Configuration ---------------------------------
//data size, from 1 till 32 bytes payload
#define RF_MAX_DATASIZE	13
//disable auto acknowledgement for all pipes
#define SPI_Write_Register_EN_AA		0x00
//disable retransmission
#define SPI_Write_Register_SETUP_RETR	0x00
 //RF data rate : 1=> 1Mb, 2=> 2Mb, 250=> 250Kb
#define RF_Data_Rate    	2

//Address width in bytes 3, 4 or 5 Bytes 
#define RF_Address_Width 5

//--------------------------------Debug log Enable or disable-----------------------------------
// set to 1 or 0
#define Enable_Debug_IRQHandler_PortD_nRF 		        0
#define Enable_Debug_nRF_Config 				0
#define Enable_Debug_nRF_SetMode_RX				0
//-------------------------------------------------------------------------------------------------
//-------------------------------- End of User Editable Section -----------------------------------
//-------------------------------------------------------------------------------------------------


//--------Configuration inherited from previous parameters not to be edited -----------------
//Sets the address width
#if RF_Address_Width == 3
    #define SPI_Write_Register_SETUP_AW 0x01
#elif RF_Address_Width == 4
    #define SPI_Write_Register_SETUP_AW 0x02
#elif RF_Address_Width == 5
    #define SPI_Write_Register_SETUP_AW 0x03
#else
    #error unsupported address width
#endif

//Sets the Pipe 0 width
#define SPI_Write_Register_RX_PW_P0		RF_MAX_DATASIZE
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

#define nRF_Config_Interruptions_Mask (bit_MASK_RX_DR | bit_MASK_TX_DS | bit_MASK_MAX_RT)
#define nRF_Config_Interruptions_Mask_Set	( Enable_RX_IRQ_Config | Enable_TX_IRQ_Config | Enable_RT_IRQ_Config)

#if( (Enable_RX_IRQ == 1) || (Enable_TX_IRQ == 1) || (Enable_RT_IRQ == 1) )
#define AtLeastOneIRQ_Is_Enabled	1
#else
#define AtLeastOneIRQ_Is_Enabled	0	
#endif
//-----------------------------------------------------------------------------------------

//-----------------------nRF_Modes configuration application, not to be edited------------
//----------------------------------------------------------------------------------------
#if (Enable_Debug_nRF_Config == 1)
#define nRF_Config_Printf(x) 			printf(x)
#define nRF_Config_PrintfHex(x)	        printf_hex(x)
#define nRF_Config_PrintStatus(x)		nRF_PrintStatus(x)
#else
#define nRF_Config_Printf(x) 			(void)0
#define nRF_Config_PrintfHex(x)	        (void)0
#define nRF_Config_PrintStatus(x)       (void)0
#endif

#if (Enable_Debug_nRF_SetMode_RX == 1)
#define nRF_SetMode_RX_Printf(x) 		printf(x)
#define nRF_SetMode_RX_PrintfHex(x)		printf_hex(x)
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
#define IRQ_Printf(x) 					printf(x)
#define IRQ_PrintfHex(x)				printf_hex(x)
#define IRQ_PrintStatus(x)				nRF_PrintStatus(x)
#else
#define IRQ_Printf(x) 					(void)0
#define IRQ_PrintfHex(x)				(void)0
#define IRQ_PrintStatus(x)				(void)0
#endif
//---------------------------------------------------------------------------------------------
