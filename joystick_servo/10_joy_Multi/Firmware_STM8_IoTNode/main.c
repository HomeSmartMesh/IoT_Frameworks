
//Test Led is Port B Pin 5

#include <intrinsics.h>
#include <iostm8s103f3.h>

#include "ClockUartLed.h"
#include "commonTypes.h"

#define EEPROM_Offset 0x4000

#define PulsePIO PA_ODR_ODR2

unsigned char ServoPos_H[3];
unsigned char ServoPos_L[3];

unsigned char rxIndex;
unsigned char rxTable[11];
unsigned char cmd;
unsigned char interruptJustHappened = 0;
#define RX_FRAME_SIZE 6

void check_Rx_H_Frame()
{
  if(rxTable[0] == 'H')
  {
    if(rxTable[3] == '1')
    {
      ServoPos_H[0] = rxTable[1];
      ServoPos_L[0] = rxTable[2];
      UARTPrintfHex(ServoPos_H[0]);
      UARTPrintfHex(ServoPos_L[0]);
      UARTPrintf("Ack");
      UARTPrintf("\n");
    }
    else if (rxTable[3] == '2')
    {
      ServoPos_H[1] = rxTable[1];
      ServoPos_L[1] = rxTable[2];
      UARTPrintfHex(ServoPos_H[1]);
      UARTPrintfHex(ServoPos_L[1]);
      UARTPrintf("Ok");
      UARTPrintf("\n");
    }
    else if (rxTable[3] == '3')
    {
      ServoPos_H[2] = rxTable[1];
      ServoPos_L[2] = rxTable[2];
      UARTPrintfHex(ServoPos_H[2]);
      UARTPrintfHex(ServoPos_L[2]);
      UARTPrintf("Good");
      UARTPrintf("\n");
    }

  }
}

void check_UART_State()
{
  if(interruptJustHappened == 1)//resetting the state machine after two loops
  {
    interruptJustHappened = 0;
  }
  else
  {
  if(rxIndex!=0)
  {
    rxIndex = 0;
    UARTPrintf("State Cleared\r\n");
  }
  }
}

#pragma vector = UART1_R_RXNE_vector
__interrupt void IRQHandler_UART(void)
{
  unsigned char rx;
  while(UART1_SR_RXNE)
  {
    rxTable[rxIndex++] = UART1_DR;
    if(rxIndex>10)
    {
       rxIndex--;
    }
  }
  //we do not expect two consecutive frames anyway as not enough time to process them
  if(rxIndex >= RX_FRAME_SIZE)//process the beginning
  {
    check_Rx_H_Frame();
    rxIndex = 0;
  }

  if((UART1_SR&0x0F) != 0x00)//Any of the errors
  {
    UART1_SR = 0;
    rx = UART1_DR;//read DR to clear the errors
  }
  interruptJustHappened = 1;
}


#pragma vector = TIM2_OVR_UIF_vector
__interrupt void IRQHandler_Timer2(void)
{

  TIM2_SR1_UIF = 0;
  TIM2_SR1 = 0;//clear all other pending flags

  TIM2_CCR3H = ServoPos_H[0];
  TIM2_CCR3L = ServoPos_L[0];
  
  TIM2_CCR2H = ServoPos_H[1];
  TIM2_CCR2L = ServoPos_L[1];
  
  TIM2_CCR1H = ServoPos_H[2];
  TIM2_CCR1L = ServoPos_L[2];
  
}
void Initialise_GPIO_A2()
{
    PulsePIO = 0;//0 default
    PA_DDR_DDR2 = 1;//output
    PA_CR1_C12 = 0;//Push Pull
    PA_CR2_C22 = 1;//Up to 10 MHz

//D3
    PD_DDR_DDR3 = 1;//output
    PD_CR1_C13 = 0;//Push Pull
    PD_CR2_C23 = 1;//Up to 10 MHz

    PC_DDR_DDR5 = 1;//output
    PC_CR1_C15 = 0;//Push Pull
    PC_CR2_C25 = 1;//Up to 10 MHz
}



void InitStartTimerPWM()
{
  // 256 * 62500 * 1/16MHz = 1
  
 //CLK_PCKENR1_PCKEN15 = 1;//Timer 2 Clock enable - all already enabled
 
 TIM2_CR1_ARPE = 0;//use preload register
 
 TIM2_PSCR_PSC = 0x04;//0x00 to 0x0F : fCK_CNT = fCK_PSC/2^(PSC[3:0]); 1 => 2, 4 => 16 : 1MHz,  0x0F => 32768 : 
 
 //20 000 us = 78*256 + 32
 TIM2_ARRH = 78;
 TIM2_ARRL = 32;

 //1520 us = 5*256 + 240
 //1720 us = 6*256 + 184

 //------------TMR2 - Channel 1------------
 TIM2_CCR1H = 6;
 TIM2_CCR1L = 184;
 
 //use Timer2 PWM channel 3 on pin A3/D2
 TIM2_CCMR1_CC1S = 0x00;//CC1 channel configured as output
 TIM2_CCMR1_OC1M = 0x06;//PWM1 mode 1 till match then 0
 TIM2_CCMR1_OC1PE = 0x01;//Preload register enabled for TIM2_CCR1
 
 TIM2_CCER1_CC1P = 0;//Default active high
 TIM2_CCER1_CC1E = 1;//Enable output on pin

 
 //------------TMR2 - Channel 3------------
 TIM2_CCR3H = 6;
 TIM2_CCR3L = 184;
 
 //use Timer2 PWM channel 3 on pin A3/D2
 TIM2_CCMR3_CC3S = 0x00;//CC1 channel configured as output
 TIM2_CCMR3_OC3M = 0x06;//PWM1 mode 1 till match then 0
 TIM2_CCMR3_OC3PE = 0x01;//Preload register enabled for TIM2_CCR1
 
 TIM2_CCER2_CC3P = 0;//Default active high
 TIM2_CCER2_CC3E = 1;//Enable output on pin

 //------------TMR2 - Channel 2------------
 TIM2_CCR2H = 6;
 TIM2_CCR2L = 184;
 
 //use Timer2 PWM channel 3 on pin A3/D2
 TIM2_CCMR2_CC2S = 0x00;//CC1 channel configured as output
 TIM2_CCMR2_OC2M = 0x06;//PWM1 mode 1 till match then 0
 TIM2_CCMR2_OC2PE = 0x01;//Preload register enabled for TIM2_CCR1
 
 TIM2_CCER1_CC2P = 0;//Default active high
 TIM2_CCER1_CC2E = 1;//Enable output on pin

 
 TIM2_IER_UIE = 1;//interrupt used for smoothing
 TIM2_CR1_CEN = 1;//Counter Enable
}

int main( void )
{
    ServoPos_H[0] = 6;
    ServoPos_L[0] = 184;
    ServoPos_H[1] = 6;
    ServoPos_L[1] = 184;
    ServoPos_H[2] = 6;
    ServoPos_L[2] = 184;

    BYTE counter = 0;
    Initialise_GPIO_A2();
    InitialiseSystemClock();

    InitialiseUART();

    UARTPrintf("\r\nProjects\\servos\\ws_01_Servos_Test\n\r");
    
    InitStartTimerPWM();
    
    UART1_CR2_RIEN = 1;     // Enable UART Rx Interrupts
    __enable_interrupt();
    
    while (1)
    {
        check_UART_State();
        counter++;
        delay_ms(100);
    }
}
