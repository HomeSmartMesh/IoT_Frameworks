/** @file spi_stm8x.c
 *
 * @author Wassim FILALI, inspired from nRF24L01P-EK
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 20.09.2015
 * $Date: 27.10.2016
 * $Revision:
 *
 */

#include "spi_stm8x.h"

#include "deviceType.h"

#if DEVICE_STM8L == 1
	#include <iostm8l151f3.h>
#elif DEVICE_STM8S == 1
	#include <iostm8s103f3.h>
#else
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif


#if DEVICE_STM8L == 1
//----------------------------------------------------------------------------------------------------------
//	PB7 SPI_MISO
//	PB6 SPI_MOSI 
//	PB5 SPI_SCK
//	PB4	GPIO_OUT CSN Chip Select Negative
//	PB3 GPIO_OUT CE  Chip Enable (this signal is active high and used to activate the chip in RX or TX mode)
//----------------------------------------------------------------------------------------------------------
 
 
void SPI_Init_ChipEnable_Pin()
{
	//To be disabled before configuration Low is Disable
	PB_ODR_ODR3 = 0;
	
    // 0: Input 1: Output
    PB_DDR_DDR3 = 1;
    
    //   0: Pseudo open drain   1: Push-pull
    PB_CR1_C13 = 1;
    //   0: Output speed up to  2 MHz   1: Output speed up to 10 MHz
    PB_CR2_C23 = 1;
}
void CE_Pin_LowDisable()
{
	PB_ODR_ODR3 = 0;
}

void CE_Pin_HighEnable()
{
	PB_ODR_ODR3 = 1;
}

void SPI_Init_ChipSelect_Pin()
{
	//To be disabled before configuration High is Disable
	PB_ODR_ODR4 = 1;
    // 0: Input 1: Output
    PB_DDR_DDR4 = 1;
    //   0: Pseudo open drain   1: Push-pull
    PB_CR1_C14 = 1;
    //   0: Output speed up to  2 MHz   1: Output speed up to 10 MHz
    PB_CR2_C24 = 1;
}

void SPI_Init_SCK_MOSI_MISO_Pins()
{
    //For the STM8L the HW managed pins directions have to be configured
    // 0: Input 1: Output
    PB_DDR_DDR5 = 1;
    //   0: Pseudo open drain   1: Push-pull
    PB_CR1_C15 = 1;
    //   0: Output speed up to  2 MHz   1: Output speed up to 10 MHz
    PB_CR2_C25 = 1;
    // 0: Input 1: Output
    PB_DDR_DDR6 = 1;
    //   0: Pseudo open drain   1: Push-pull
    PB_CR1_C16 = 1;
    //   0: Output speed up to  2 MHz   1: Output speed up to 10 MHz
    PB_CR2_C26 = 1;
    // 0: Input 1: Output
    PB_DDR_DDR7 = 0;
}


BYTE CE_Pin_getstate()
{
	return PB_IDR_IDR4;
}

void CSN_Pin_LowSelect()
{
	PB_ODR_ODR4 = 0;
}

void CSN_Pin_HighDisable()
{
	PB_ODR_ODR4 = 1;
}

//Initialise the Configuration registers 1 and 2. Also resets the polynomial to the default
void SPI_Init_Peripheral()
{
        const BYTE SPI_CR1_BR_fDiv256 = 0x07;

        //Enable SPI1 Peripheral Clock
        CLK_PCKENR1_PCKEN14 = 1;
	
	SPI1_CR1 = 0x00;//default reset value
	//SPI_CR1_LSBFIRST = 0;
	SPI1_CR1_BR = SPI_CR1_BR_fDiv256;
	//SPI_CR1_CPOL = 0; Low when Idle
	//SPI_CR1_CPHA = 0; 1st edge
	
	SPI1_CR2 = 0x00;
	//SPI_CR2_BDM = 0;// 2 lines
	SPI1_CR2_SSM = 1;//Software Slave Management Enabled
	SPI1_CR2_SSI = 1;//Master Mode

	//Set the SPI in Master Mode
	SPI1_CR1_MSTR = 1;
	
	SPI1_CRCPR = 0x07;//CRC Polynomial reset value is 0x07
	
	//Enable the SPI Peripheral
	SPI1_CR1_SPE = 1;
        
        //Enable SPI_Clock
        
	
}

//RM0016 Note :  Do not use the BSY flag to handle each data transmission or reception. 
//It is better to use TXE and RXNE flags instead.
BYTE SPI_WriteRead(BYTE Data)
{
	//Wait while (Tx Buffer not Empty);
	while(SPI1_SR_TXE == 0);
	SPI1_DR = Data;
	//while (SPI_GetFlagStatus(SPI_FLAG_BSY)== SET);// Do not use Busy Flag in every transaction
	//Wait while Rx Buffer Empty
	while(SPI1_SR_RXNE == 0);
	
	return SPI1_DR;
}

#elif DEVICE_STM8S == 1
 
//----------------------------------------------------------------------------------------------------------
//	PC7 SPI_MISO
//	PC6 SPI_MOSI 
//	PC5 SPI_SCK
//	PC4	GPIO_OUT CSN Chip Select Negative
//	PC3 GPIO_OUT CE  Chip Enable (this signal is active high and used to activate the chip in RX or TX mode)
//----------------------------------------------------------------------------------------------------------
 
void SPI_Init_SCK_MOSI_MISO_Pins()
{
    //HW managed pins directions do not need to be configured for the STM8S
}
 
void SPI_Init_ChipEnable_Pin()
{
	//To be disabled before configuration Low is Disable
	PC_ODR_ODR3 = 0;
	
    // 0: Input 1: Output
    PC_DDR_DDR3 = 1;
    
    //   0: Pseudo open drain   1: Push-pull
    PC_CR1_C13 = 1;
    //   0: Output speed up to  2 MHz   1: Output speed up to 10 MHz
    PC_CR2_C23 = 1;
}

void CE_Pin_LowDisable()
{
	PC_ODR_ODR3 = 0;
}

void CE_Pin_HighEnable()
{
	PC_ODR_ODR3 = 1;
}

void SPI_Init_ChipSelect_Pin()
{
	//To be disabled before configuration High is Disable
	PC_ODR_ODR4 = 1;
    // 0: Input 1: Output
    PC_DDR_DDR4 = 1;
    //   0: Pseudo open drain   1: Push-pull
    PC_CR1_C14 = 1;
    //   0: Output speed up to  2 MHz   1: Output speed up to 10 MHz
    PC_CR2_C24 = 1;
}

BYTE CE_Pin_getstate()
{
	return PC_IDR_IDR3;
}

void CSN_Pin_LowSelect()
{
	PC_ODR_ODR4 = 0;
}

void CSN_Pin_HighDisable()
{
	PC_ODR_ODR4 = 1;
}

//Initialise the Configuration registers 1 and 2. Also resets the polynomial to the default
void SPI_Init_Peripheral()
{
	const BYTE SPI_CR1_BR_fDiv256 = 0x07;
	
	SPI_CR1 = 0x00;//default reset value
	//SPI_CR1_LSBFIRST = 0;
	SPI_CR1_BR = SPI_CR1_BR_fDiv256;
	//SPI_CR1_CPOL = 0; Low when Idle
	//SPI_CR1_CPHA = 0; 1st edge
	
	SPI_CR2 = 0x00;
	//SPI_CR2_BDM = 0;// 2 lines
	SPI_CR2_SSM = 1;//Software Slave Management Enabled
	SPI_CR2_SSI = 1;//Master Mode

	//Set the SPI in Master Mode
	SPI_CR1_MSTR = 1;
	
	SPI_CRCPR = 0x07;//CRC Polynomial reset value is 0x07
	
	//Enable the SPI Peripheral
	SPI_CR1_SPE = 1;
	
}

//RM0016 Note :  Do not use the BSY flag to handle each data transmission or reception. 
//It is better to use TXE and RXNE flags instead.
BYTE SPI_WriteRead(BYTE Data)
{
	//Wait while (Tx Buffer not Empty);
	while(SPI_SR_TXE == 0);
	SPI_DR = Data;
	//while (SPI_GetFlagStatus(SPI_FLAG_BSY)== SET);// Do not use Busy Flag in every transaction
	//Wait while Rx Buffer Empty
	while(SPI_SR_RXNE == 0);
	
	return SPI_DR;
}
#else
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif
