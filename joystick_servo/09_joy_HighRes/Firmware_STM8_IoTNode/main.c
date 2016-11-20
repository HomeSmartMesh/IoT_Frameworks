
//Test Led is Port B Pin 5

#include <intrinsics.h>
#include <iostm8s103f3.h>

#include "ClockUartLed.h"
#include "commonTypes.h"

#define EEPROM_Offset 0x4000

#define PulsePIO PA_ODR_ODR2

BYTE usefilter = 0;
int ServoTargetPos = 1520;
int ServoPos = 1520;

unsigned char rxIndex;
unsigned char rxTable[11];
unsigned char cmd;
unsigned char interruptJustHappened = 0;
#define RX_FRAME_SIZE 6

void Apply_Cmd_To_Servo(BYTE pos)
{
  int pulse16 = pos;
  pulse16 = 1000 + pulse16*4;
  ServoTargetPos = pulse16;
  
}
void Apply_HR_Cmd_To_Servo(unsigned int val)
{
  ServoTargetPos = val;
}

void check_Rx_Frame()
{
  if(rxTable[0] == 'S')
  {
    if(rxTable[1] == 'r')
    {
      if((rxTable[2] ^ rxTable[3]) == 0xFF)
      {
        cmd = rxTable[2];
        Apply_Cmd_To_Servo(cmd);
        UARTPrintf("Y\n");
      }
    }
  }
  /*else if(rxTable[0] == 'H')
  {
    if(rxTable[3] == 'R')
    {
      unsigned int val = rxTable[1];
      val = val * 256 + rxTable[2];
      Apply_HR_Cmd_To_Servo(val);
      UARTPrintf("Ack Rx ");        
      UARTPrintf_uint(val);
      UARTPrintf("\n");
    }
  }*/
}
void check_Rx_H_Frame()
{
  if(rxTable[0] == 'H')
  {
    if(rxTable[3] == 'R')
    {
      unsigned int val = rxTable[1];
      val = val * 256 + rxTable[2];
      Apply_HR_Cmd_To_Servo(val);
      UARTPrintf("Ack Rx ");        
      UARTPrintf_uint(val);
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
  int pulse16_H,pulse16_L;

  TIM2_SR1_UIF = 0;
  TIM2_SR1 = 0;//clear all other pending flags

  if(usefilter == 1)
  {
    if(ServoPos<ServoTargetPos) 
    {
      ServoPos++;
    }
    else if(ServoPos>ServoTargetPos)
    {
      ServoPos--;
    }
    pulse16_H = ServoPos / 256;
    pulse16_L = ServoPos - pulse16_H * 256;
  }
  else
  {
    ServoPos = ServoTargetPos;
    pulse16_H = ServoPos / 256;
    pulse16_L = ServoPos - pulse16_H * 256;
  }

  TIM2_CCR3H = pulse16_H;
  TIM2_CCR3L = pulse16_L;
  
}
void Initialise_GPIO_A2()
{
    PulsePIO = 0;//0 default
    PA_DDR_DDR2 = 1;//output
    PA_CR1_C12 = 0;//Push Pull
    PA_CR2_C22 = 1;//Up to 10 MHz
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
 TIM2_CCR3H = 5;
 TIM2_CCR3L = 240;
 
 //use Timer2 PWM channel 3 on pin A3
 TIM2_CCMR3_CC3S = 0x00;//CC1 channel configured as output
 TIM2_CCMR3_OC3M = 0x06;//PWM1 mode 1 till match then 0
 TIM2_CCMR3_OC3PE = 0x01;//Preload register enabled for TIM2_CCR1
 
 TIM2_CCER2_CC3P = 0;//Default active high
 
 TIM2_CCER2_CC3E = 1;//Enable output on pin
 
 TIM2_IER_UIE = 1;//interrupt used for smoothing
 
 TIM2_CR1_CEN = 1;//Counter Enable
}

int main( void )
{

    BYTE counter = 0;
    Initialise_GPIO_A2();
    InitialiseSystemClock();

    InitialiseUART();

    UARTPrintf("\r\nProjects\\servos\\ws_01_Servos_Test\n\r");
    
    //Timer initialisation
    Apply_Cmd_To_Servo(0);
    
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
