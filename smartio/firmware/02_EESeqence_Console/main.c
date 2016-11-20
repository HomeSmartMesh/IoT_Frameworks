
//Test Led is Port B Pin 5

#include <iostm8s103f3.h>
#include <intrinsics.h>

#include "ClockUartLed.h"

unsigned char rxIndex;
unsigned char rxTable[10];
unsigned char cmd;
unsigned char interruptJustHappened;
#define RX_FRAME_SIZE 4
#define EEPROM_Offset 0x4000

unsigned char isCommand;

void Initialise_ULN_Outputs()
{
  PD_DDR_bit.DDR1 = 1;// 1: Output : Rx Pin
  PD_CR1_bit.C11 = 1;//   0: Push Pull
      
  PD_DDR_bit.DDR2 = 1;// 1: Output : Rx Pin
  PD_CR1_bit.C12 = 1;//   0: Push Pull

  PD_DDR_bit.DDR3 = 1;// 1: Output : Rx Pin
  PD_CR1_bit.C13 = 1;//   0: Push Pull

  PC_DDR_bit.DDR3 = 1;// 1: Output : Rx Pin
  PC_CR1_bit.C13 = 1;//   0: Push Pull

  PC_DDR_bit.DDR4 = 1;// 1: Output : Rx Pin
  PC_CR1_bit.C14 = 1;//   0: Push Pull

  PC_DDR_bit.DDR5 = 1;// 1: Output : Rx Pin
  PC_CR1_bit.C15 = 1;//   0: Push Pull

  PC_DDR_bit.DDR6 = 1;// 1: Output : Rx Pin
  PC_CR1_bit.C16 = 1;//   0: Push Pull

  PC_DDR_bit.DDR7 = 1;// 1: Output : Rx Pin
  PC_CR1_bit.C17 = 1;//   0: Push Pull

}


void Reset_ULN_Output()
{
  PD_ODR_bit.ODR1 = 0;
  PD_ODR_bit.ODR2 = 0;
  PD_ODR_bit.ODR3 = 0;

  PC_ODR_bit.ODR3 = 0;
  PC_ODR_bit.ODR4 = 0;
  PC_ODR_bit.ODR5 = 0;
  PC_ODR_bit.ODR6 = 0;
  PC_ODR_bit.ODR7 = 0;
}

void Apply_Cmd_To_ULN_Output(unsigned char cmd)
{
  PD_ODR_bit.ODR1 = (cmd>>5)&0x01;
  PD_ODR_bit.ODR2 = (cmd>>6)&0x01;
  PD_ODR_bit.ODR3 = (cmd>>7)&0x01;

  PC_ODR_bit.ODR3 = (cmd>>0)&0x01;
  PC_ODR_bit.ODR4 = (cmd>>1)&0x01;
  PC_ODR_bit.ODR5 = (cmd>>2)&0x01;
  PC_ODR_bit.ODR6 = (cmd>>3)&0x01;
  PC_ODR_bit.ODR7 = (cmd>>4)&0x01;
}

unsigned char Char2Hex(unsigned char c1,unsigned char c2)
{
  unsigned char Command,ComHigh,ComLow;
  ComHigh = c1 - '0';
  if(ComHigh > 9)
    ComHigh -= 7;
  ComLow = c2 - '0';
  if(ComLow > 9)
    ComLow -= 7;
  Command = (ComHigh << 4) + ComLow;
  return Command;
}

void check_Rx_Frame(unsigned char rxIndex)
{
  if(rxIndex == 2)
  {
    /*if(FLASH_IAPSR_DUL == 0)//DUL is at 0x08 ; then unlock EE write protection
    {
      FLASH_DUKR = 0xAE;
      FLASH_DUKR = 0x56;
    }*/
    if(rxTable[0] == 'S')
    {
      isCommand = 1;
      UARTPrintf("EEPROM Sequencing Started\r\n");
    }
    else if (rxTable[0] == 'P')
    {
      isCommand = 0;
      UARTPrintf("EEPROM Sequencing Stopped\r\n");
    }
  }
  else
  {
    if(rxTable[0] == 'S')
      if(rxTable[1] == 'e')
      {
        unsigned char Command;
        Command = Char2Hex(rxTable[2],rxTable[3]);
        Apply_Cmd_To_ULN_Output(Command);
          UARTPrintf("Ack txt ");
          UARTPrintfHex(Command);
          UARTPrintf("\r\n");
      }
  }
}

#pragma vector = UART1_R_RXNE_vector
__interrupt void IRQHandler_UART(void)
{
  unsigned char rx;
  while(UART1_SR_RXNE)
  {
    rx = UART1_DR;
    putc(rx);//echo typed characters
    if(rxIndex<10)
    {
      rxTable[rxIndex++] = rx;
    }
    if(rx == '\r')
    {
      putc('\n');//add new line after return
      check_Rx_Frame(rxIndex);
      rxIndex = 0;
    }
    else if(rxIndex == 10)
    {
      check_Rx_Frame(rxIndex);
      rxIndex = 0;
    }
  }
  if((UART1_SR&0x0F) != 0x00)//Any of the errors
  {
    UART1_SR = 0;
    rx = UART1_DR;//read DR to clear the errors
  }
  interruptJustHappened = 1;
}

void DumpEE()
{
  unsigned char *pTable = (unsigned char *)EEPROM_Offset;
  for(int i=0;i<10;i++)
  {
    UARTPrintfHex(i*16);
    UARTPrintf(" : ");
    UARTPrintfHexTable(pTable,16);
    UARTPrintf("\n\r");
    pTable += 16;
  }
}

//Every section has 4 Bytes
// Byte1 = Command Type (0xFF Apply value Byte on all IO)
// Byte2 = Value
// 
unsigned int ProcessEE(unsigned char index)
{
  unsigned char *pTable = (unsigned char *)EEPROM_Offset;
  unsigned int delay = 900;

  pTable+=(index*4);
  if(*pTable == 0xFF)
  {
    pTable++;
    Apply_Cmd_To_ULN_Output(*pTable);
    UARTPrintf("@ ");
    UARTPrintfHex(index*4);
    UARTPrintf(" : All IO ");
    UARTPrintfHex(*pTable);
    UARTPrintf(" : delay ");
    pTable++;
    delay = *pTable;
    UARTPrintf_uint(delay);
    delay = delay *100;
    UARTPrintf(" x100ms\n\r");
  }
  
  return delay;
}

unsigned char isEEOutputCommands()
{
  unsigned char *pTable = (unsigned char *)EEPROM_Offset;
  unsigned char val = 0;
  if(*pTable++ == 'O')
    if(*pTable++ == 'c')
      if(*pTable++ == 'm')
        if(*pTable++ == 'd')
        {
          val = 1;
          UARTPrintf("Ocmd found in EEProm\r\n");
        }
  return val;
}

int main( void )
{
    rxIndex = 0;
    unsigned char index = 1;
    interruptJustHappened = 0;
    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();
    
    Initialise_ULN_Outputs();
    
    Reset_ULN_Output();
    
    InitialiseUART();
    
    UARTPrintf("\r\nD:\\Projects\\STM8\\IO Expander\\Firmware_IAR\\02_UART_Parallel\n\r");
    
    DumpEE();
    isCommand = isEEOutputCommands();
    
    //unsigned char status = UART1_SR;//to clear error flags
    //unsigned char rx = UART1_DR;

    UART1_CR2_RIEN = 1;     // Enable UART Rx Interrupts
    __enable_interrupt();

 while (1)
    {
      /*if(interruptJustHappened == 1)//resetting the state machine after two loops
      {
        interruptJustHappened = 0;
      }
      else
      {
        if(rxIndex!=0)
        {
          //rxIndex = 0;
          //check_Rx_Frame();
          //UARTPrintf("State Cleared\r\n");
        }
      }*/
      
      if (isCommand)
      {
        unsigned int delay = ProcessEE(index++);
        delay_ms(delay);
      }
      if(index == 5)
      {
        index = 1;
      }
      Test_Led_On();
      delay_ms(100);
      Test_Led_Off();
    }
}
