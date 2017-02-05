#include <iostm8l151f3.h>
#include <intrinsics.h>

//should be before other files that use it such as clock_led.h
#include "deviceType.h"
#include "nRF_Configuration.h"

#include "nRF_SPI.h"
//for nRF_SetMode_TX()
#include "nRF.h"

#include "nRF_Tx.h"

#include "uart.h"
#include "clock_led.h"


#define EEPROM_Offset 0x1000
#define NODE_ID       (char *) EEPROM_Offset;
unsigned char NodeId;

//to format the tx data
#include "rf_protocol.h"

BYTE tx_data[RF_RX_DATASIZE];

//---------------------- Active Halt Mode :
// - CPU and Peripheral clocks stopped, RTC running
// - wakeup from RTC, or external/Reset


//------------------------------ Node Config ---------------------------------
#define NODE_MAGNET_B_SET               0
#define NODE_MAGNET_D_SET               1
#define NODE_MAGNET_D_INTERRUPT         1
#define NODE_I2C_SET                    0
#define NODE_MAX44009_SET               0
//----------------------------------------------------------------------------



void rf_send_alive()
{
	rf_get_tx_alive_3B(NodeId, tx_data);
	nRF_Transmit(tx_data,3);
}

void RfSwitch(unsigned char state)
{
      unsigned char Tx_Data[4];
      Tx_Data[0]=0xC5;
      Tx_Data[1]=NodeId;
      Tx_Data[2]=state;
      Tx_Data[3]= Tx_Data[0] ^ NodeId ^ state;
      nRF_Transmit(Tx_Data,4);
}


void LogMagnets()
{
      delay_100us();
      unsigned char Magnet_B0,Magnet_D0;
      Magnet_B0 = PB_IDR_IDR0;
      Magnet_D0 = PD_IDR_IDR0;
      //printf(" LVD0 ");
      UARTPrintf_uint(Magnet_D0);
      //printf(" ; HH B0 ");
      UARTPrintf_uint(Magnet_B0);
      printf("\n");
      delay_100us();
      delay_100us();
}

void i2c_user_Rx_Callback(BYTE *userdata,BYTE size)
{
	/*printf("I2C Transaction complete, received:\n\r");
	UARTPrintfHexTable(userdata,size);
	printf("\n\r");*/
        
}

void i2c_user_Tx_Callback(BYTE *userdata,BYTE size)
{
  /*
	printf("I2C Transaction complete, Transmitted:\n\r");
	UARTPrintfHexTable(userdata,size);
	printf("\n\r");
        */
}

void i2c_user_Error_Callback(BYTE l_sr2)
{
	if(l_sr2 & 0x01)
	{
		printf("[I2C Bus Error]\n\r");
	}
	if(l_sr2 & 0x02)
	{
		printf("[I2C Arbitration Lost]\n\r");
	}
	if(l_sr2 & 0x04)
	{
		printf("[I2C no Acknowledge]\n\r");//this is ok for the slave
	}
	if(l_sr2 & 0x08)
	{
		printf("[I2C Bus Overrun]\n\r");
	}
}

//B0 : bit 0 - pin interrupt
#pragma vector = EXTI0_vector
__interrupt void IRQHandler_Pin0(void)
{
  if(EXTI_SR1_P0F == 1)
  {
    //printf("Pin0_Interrupt ");LogMagnets();
    RfSwitch(PD_IDR_IDR0);//D0 is Top
  }
  EXTI_SR1 = 0xFF;//acknowledge all interrupts pins
}


#pragma vector = RTC_WAKEUP_vector
__interrupt void IRQHandler_RTC(void)
{
  if(RTC_ISR2_WUTF)
  {
    RTC_ISR2_WUTF = 0;
    
    rf_send_alive();
    
    //LogMagnets();
  }
  
}


void SMT8L_Switch_ToHSI()
{
  CLK_SWCR_SWEN = 1;                  //  Enable switching.
  CLK_SWR = 0x01;                     //  Use HSI as the clock source.
  while (CLK_SWCR_SWBSY != 0);        //  Pause while the clock switch is busy.
}


void Init_Magnet_PB0()
{
    PB_DDR_bit.DDR0 = 0;//  0: Input
    PB_CR1_bit.C10 = 0; //  0: Floating
#if MAGNET_B_INTERRUPT == 1
    PB_CR2_bit.C20 = 1; // Exernal interrupt enabled
    EXTI_CR1_P0IS = 3;//Rising and Falling edges, interrupt on events - bit 0
#endif
    //EXTI_CR3_PBIS = 00;//Falling edge and low level - Port B
}

void Init_Magnet_PD0()
{
    PD_DDR_bit.DDR0 = 0;//  0: Input
    PD_CR1_bit.C10 = 0; //  0: Floating

#if MAGNET_D_INTERRUPT == 1
    PD_CR2_bit.C20 = 1; // Exernal interrupt enabled
    EXTI_CR1_P0IS = 3;//Rising and Falling edges, interrupt on events - bit 0
#endif
}


void Initialise_Test_GPIO_A2()
{
    PA_DDR_bit.DDR2 = 1;//  1: Output
    PA_CR1_bit.C12 = 1; //  1: Push-pull
}

void GPIO_B3_High()
{
    PB_ODR_bit.ODR3 = 1;
}

void GPIO_B3_Low()
{
    PB_ODR_bit.ODR3 = 0;
}

void PingColor()
{
      unsigned char Tx_Data[5];
      Tx_Data[0]=128;
      Tx_Data[1]=255;
      Tx_Data[2]=100;
      Tx_Data[3]=0x59;
      nRF_Transmit(Tx_Data,4);
}
void PingUart(unsigned char index)
{
      printf("Ping Color STM8L ");
      UARTPrintf_uint(index);
      printf(" \n");
}

void configure_All_PIO()
{
	//A0 - SWIM
	//PA_DDR_bit.DDR3 = 1;//output
	//PA_ODR_bit.ODR3 = 0;//Low
	//A1 - NRST
	//PA_DDR_bit.DDR3 = 1;//output
	//PA_ODR_bit.ODR3 = 0;//Low
	//A2 - UART-Tx
	PA_DDR_bit.DDR2 = 1;//output
	PA_ODR_bit.ODR2 = 0;//Low
	//A3 - unconnected
	PA_DDR_bit.DDR3 = 1;//output
	PA_ODR_bit.ODR3 = 0;//Low

	//B0 - Magnet-1 Side
#if NODE_MAGNET_B_SET != 1
	PB_DDR_bit.DDR0 = 0;//output
	PB_ODR_bit.ODR0 = 0;//Low
#endif
#if NODE_MAX44009_SET != 1
      	//B1 - Light-IRQ
	PB_DDR_bit.DDR1 = 1;//output
	PB_ODR_bit.ODR1 = 0;//Low
#endif
	//B2 - unconnected
	PB_DDR_bit.DDR2 = 1;//output
	PB_ODR_bit.ODR2 = 0;//Low
        //B3 - nRF CE_Pin_LowDisable()
	PB_DDR_bit.DDR3 = 1;//output
	PB_ODR_ODR3 = 0;
        //B4 - nRF CSN High Disable
	PB_DDR_bit.DDR4 = 1;//output
	PB_ODR_ODR4 = 1;
        //B5 - nRF SPI-SCK
	PB_DDR_bit.DDR5 = 1;//output
	PB_ODR_ODR5 = 0;
        //B6 - nRF SPI-MOSI
	PB_DDR_bit.DDR6 = 1;//output
	PB_ODR_ODR6 = 1;
        //B7 - nRF SPI-MISO
	PB_DDR_bit.DDR7 = 0;//input
	//PB_ODR_ODR7 = 1;

#if NODE_I2C_SET != 1
	//C0 - I²C SDA
	PC_DDR_bit.DDR0 = 1;//output
	PC_ODR_bit.ODR0 = 0;//Low
	//C1 - I²C SCL
	PC_DDR_bit.DDR1 = 1;//output
	PC_ODR_bit.ODR1 = 0;//Low
#endif
        //C2-C3 : do not exist
	//C4 - nRF IRQ
	PC_DDR_bit.DDR4 = 0;//input
	//PC_ODR_bit.ODR4 = 0;
        //C5 - Osc 1
	PC_DDR_bit.DDR5 = 1;//output
	PC_ODR_bit.ODR5 = 0;//Low
	//C6 - Osc 2
	PC_DDR_bit.DDR6 = 1;//output
	PC_ODR_bit.ODR6 = 0;//Low

	//D0 - Magnet-2 Top
#if NODE_MAGNET_D_SET != 1
	PD_DDR_bit.DDR0 = 1;//output
	PD_ODR_bit.ODR0 = 0;//Low
#endif

}


int main( void )
{
	NodeId = *NODE_ID;

        configure_All_PIO();
#if NODE_MAGNET_B_SET == 1
  Init_Magnet_PB0();
#endif
#if NODE_MAGNET_D_SET == 1
  Init_Magnet_PD0();
#endif
	
	Initialise_STM8L_Clock();			//here the RTC clock source is set to LSI
	Initialise_STM8L_RTC_LowPower(30);//sleep period 30 sec
    
	//SYSCFG_RMPCR1_USART1TR_REMAP = 1; // Remap 01: USART1_TX on PA2 and USART1_RX on PA3
	//uart_init();//UART Disabled
	//Applies the compile time configured parameters from nRF_Configuration.h
	nRF_Config();

    
	__enable_interrupt();
    //
    // Main loop
    //
    while (1)
    {
      __halt();
      
    }
}
