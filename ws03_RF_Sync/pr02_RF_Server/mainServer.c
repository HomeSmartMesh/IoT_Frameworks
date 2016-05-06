
//Test Led is Port B Pin 5

#include <intrinsics.h>
#include <iostm8s103f3.h>

#include "ClockUartLed.h"

//for direct SPI_Read_Register()
#include "nRF_SPI.h"

//for nRF_SetMode_TX()
#include "nRF_Modes.h"

#include "nRF_Tx.h"
#include "nRF_RegText.h"

#include "nRF_IRQ.h"

#include "WS2812B.h"

#define EEPROM_Offset 0x4000
#define EE_NODE_ID       (char *) EEPROM_Offset;
#define EE_TimeSlot       (char *) (EEPROM_Offset+1);
BYTE NodeId;

#define PulsePIO PA_ODR_ODR2
BYTE Tx_Data[4];
BYTE RF_Cycle;

void SendSyncPulse()
{
    PulsePIO = 1;
    delay_1ms_Count(4);//wait till Transmision completes
    nRF_Transmit(Tx_Data,4);
    PulsePIO = 0;
    nRF_SetMode_RX();//Back to Reception
}

#pragma vector = TIM2_OVR_UIF_vector
__interrupt void IRQHandler_Timer2(void)
{
  TIM2_SR1_UIF = 0;
  TIM2_SR1 = 0;//clear all other pending flags

  //transmit with Freq RF Cycle
  if(RF_Cycle == 0)
  {
    SendSyncPulse();
  }
  RF_Cycle++;
  if(RF_Cycle == 100)//last one reached
  {
    RF_Cycle = 0;
  }
}

//User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
  if(rx_DataSize == 4)
  {
    if(rxData[0] == 0x49)
    {
      PulsePIO = 1;
      delay_1ms_Count(1);
      PulsePIO = 0;
    }
    else
    {
      UARTPrintfLn("Mismatching id");
      UARTPrintf(" Data : ");
      UARTPrintfHexTable(rxData,rx_DataSize);
      UARTPrintf("\n\r");
    }
  }
  
}

void InitStartTimer()
{
  // 256 * 62500 * 1/16MHz = 1
  
 //CLK_PCKENR1_PCKEN15 = 1;//Timer 2 Clock enable - all already enabled
 
 TIM2_CR1_ARPE = 1;//use preload register
 
 TIM2_PSCR_PSC = 0x02;//0x00 to 0x0F : fCK_CNT = fCK_PSC/2^(PSC[3:0]); 1 => 8MHz, 2 => 4MHz,  0x0F => 488.28125 Hz => 2.048 ms
 
 TIM2_ARRH = 156;
 TIM2_ARRL = 64;//156*256 + 64 = 160 000 * 1/4 MHz => 40 ms
 
 TIM2_IER_UIE = 1;//Interrupt Enable Register, Update Interrupt Enable
 
 TIM2_CR1_CEN = 1;//Counter Enable
}

void Initialise_GPIO_A2()
{
    PA_DDR_DDR2 = 1;//output
    PA_CR1_C12 = 0;//Push Pull
    PA_CR2_C22 = 1;//Up to 10 MHz
}

int main( void )
{
	
    NodeId = *EE_NODE_ID;

    BYTE counter = 0;

    RF_Cycle = 0;

    Tx_Data[0] = 0x23;//Master Sync
    Tx_Data[1] = NodeId;
    Tx_Data[2] = 0x00;
    Tx_Data[3] = 0x00;

    Initialise_GPIO_A2();
    InitialiseSystemClock();

    InitialiseUART();
	
    UARTPrintf("\r\n_________________________\n\r");
    UARTPrintf("Node(");
    UARTPrintf_uint(NodeId);
    UARTPrintf(") - RF Sync Server\n\r");

    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();
    nRF_SetMode_RX();

    InitStartTimer();
    
    __enable_interrupt();

    while (1)
    {
        counter++;
        delay_1ms_Count(1000);
    }
}
