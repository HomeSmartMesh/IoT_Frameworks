/** @file nRF_IRQ.c
 *
 * @author Wassim FILALI, inspired from nRF24L01P-EK
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 14.12.2015
 * $Revision:
 *
*/
#include <intrinsics.h>

#include "deviceType.h"

#if DEVICE_STM8L == 1
	#include <iostm8l151f3.h>
#elif DEVICE_STM8S == 1
	#include <iostm8s103f3.h>
#else
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif

#include "nRF_IRQ_stm8x.h"

#include "nRF_Configuration.h"

#include "commonTypes.h"

//for direct SPI_Read_Register()
#include "nRF_SPI.h"

//for nRF_SetMode_TX()
#include "nRF.h"

#include "nRF_Tx.h"
#include "nRF_RegText.h"

//nRF24L01P module IRQ on pin8 of RF module and D2 on STM8

void userRxCallBack(BYTE *rxData,BYTE rx_DataSize);

BYTE RxData[RF_RX_DATASIZE];


//---------------------------------------------------------------------------------------------
#if(Enable_RX_IRQ == 1)
void ProcessUserData_inIRQ()
{
	BYTE rx_size = RxData[31];
	userRxCallBack(RxData,rx_size);
	#if (Enable_Debug_IRQHandler_PortD_nRF == 1)
		  IRQ_Printf("  Received Packet: ");
		  UARTPrintfHexTable(RxData,rx_size);
		  IRQ_Printf("");
	#endif

}
#endif
//---------------------------------------------------------------------------------------------


#if DEVICE_STM8L == 1




#if(AtLeastOneIRQ_Is_Enabled == 1)

void nRF_IRQ_Config()
{
	/*
	__disable_interrupt();

    // 0: Input 1: Output
    PC_DDR_DDR4 = 0;

	//   0: Floating input   1: with pull-up
    PC_CR1_C14 = 0;

    //   0: External Interrupt Disabled   1: External Interrupt enabled
    PC_CR2_C24 = 1;
   
	//  Set up the interrupt.
	//EXTI_CR1_PCIS = 0;      //  Falling edge and low level
	//EXTI_CR2_TLIS = 0;      //  Falling edge only.
	__enable_interrupt();
	*/
}


//interrupt vectors mapping in CD00226640 - STM8S103F3.pdf (DocID15441 Rev 12) Page 43
//careful !!!! the IRQ number is not the vector number. The interrupt vector address = Interrupt vector number N * 4 + Interrupt base address
//IRQ no6, EXTI3, PortD external interrupts, 0x00 8020 = Vector 8
//EXTI2_vector = 8
#pragma vector = EXTI3_vector
__interrupt void IRQHandler_PortD_nRF(void)
{
	IRQ_Printf("(IRQ)");

	BYTE status = SPI_Read_Register(STATUS);		
	IRQ_PrintStatus(status);
	
	#if(Enable_RX_IRQ == 1)
	if(( status & (bit_RX_DR) ) != 0)//Rx IRQ was triggered
	{
		BYTE fifo_Status;
		do
		{
                        //TODO check the size before reading the Hard coded value
			SPI_Read_Buf(RD_RX_PLOAD,RxData,RF_RX_DATASIZE);
			ProcessUserData_inIRQ();
			
			nRF_ClearStatus( bit_RX_DR );
			
			fifo_Status = SPI_Read_Register(FIFO_STATUS);
			IRQ_Printf("  FifoStatus: ");
			IRQ_PrintfHex(fifo_Status);
			IRQ_Printf("");
		}while((fifo_Status & 0x01) == 0 );
	}
	#endif
	#if(Enable_TX_IRQ == 1)	
	if(	( status & (bit_TX_DS ) ) != 0	)
	{
		nRF_ClearStatus(bit_TX_DS);
	}
	#endif
	#if(Enable_RT_IRQ == 1)	
	if(	( status & (bit_MAX_RT) ) != 0	)//check other IRQs
	{
		nRF_ClearStatus(bit_MAX_RT);
	}
	#endif

}

#endif /*(AtLeastOneIRQ_Is_Enabled == 1)*/

//#end of DEVICE_STM8L
#elif DEVICE_STM8S == 1

#if(AtLeastOneIRQ_Is_Enabled == 1)

void nRF_IRQ_Config()
{
	__disable_interrupt();

    // 0: Input 1: Output
    PD_DDR_DDR2 = 0;

	//   0: Floating input   1: with pull-up
    PD_CR1_C12 = 0;

    //   0: External Interrupt Disabled   1: External Interrupt enabled
    PD_CR2_C22 = 1;
   
	//  Set up the interrupt.
	EXTI_CR1_PDIS = 0;      //  Falling edge and low level
	EXTI_CR2_TLIS = 0;      //  Falling edge only.
	__enable_interrupt();
}


//interrupt vectors mapping in CD00226640 - STM8S103F3.pdf (DocID15441 Rev 12) Page 43
//careful !!!! the IRQ number is not the vector number. The interrupt vector address = Interrupt vector number N * 4 + Interrupt base address
//IRQ no6, EXTI3, PortD external interrupts, 0x00 8020 = Vector 8
//EXTI2_vector = 8
#pragma vector = EXTI3_vector
__interrupt void IRQHandler_PortD_nRF(void)
{
	IRQ_Printf("(IRQ)");

	BYTE status = SPI_Read_Register(STATUS);		
	IRQ_PrintStatus(status);
	
	#if(Enable_RX_IRQ == 1)
	if(( status & (bit_RX_DR) ) != 0)//Rx IRQ was triggered
	{
            BYTE fifo_Status;
            do
            {
                    SPI_Read_Buf(RD_RX_PLOAD,RxData,RF_RX_DATASIZE);
                    ProcessUserData_inIRQ();
                    
                    nRF_ClearStatus( bit_RX_DR );
                    
                    fifo_Status = SPI_Read_Register(FIFO_STATUS);
                    IRQ_Printf("  FifoStatus: ");
                    IRQ_PrintfHex(fifo_Status);
                    IRQ_Printf("");
            }while((fifo_Status & 0x01) == 0 );
            //done reading, flush Rx to reset size
            SPI_Cmd_FlushRx();
	}
	#endif
	#if(Enable_TX_IRQ == 1)	
	if(	( status & (bit_TX_DS ) ) != 0	)
	{
		nRF_ClearStatus(bit_TX_DS);
	}
	#endif
	#if(Enable_RT_IRQ == 1)	
	if(	( status & (bit_MAX_RT) ) != 0	)//check other IRQs
	{
		nRF_ClearStatus(bit_MAX_RT);
	}
	#endif

}

#endif /*(AtLeastOneIRQ_Is_Enabled == 1)*/

#else	/*DEVICE_STM8S*/
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif
