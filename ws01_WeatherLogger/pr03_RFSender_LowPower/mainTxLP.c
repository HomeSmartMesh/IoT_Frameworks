
//Test Led is Port B Pin 5

#include <iostm8s103f3.h>
#include <intrinsics.h>

#include "ClockUartLed.h"

//for direct SPI_Read_Register()
#include "nRF_SPI.h"

//for nRF_SetMode_TX()
#include "nRF_Modes.h"

#include "nRF_Tx.h"
#include "nRF_RegText.h"

#include "nRF_IRQ.h"

#include "temp_ds18b20.h"


void Print_ConfigAndStatus_Registers()
{
	BYTE ConfigVal;
        UARTPrintf("  ");
	ConfigVal = SPI_Read_Register(CONFIG);
	UARTPrintf(" ; CONFIG: ");
	UARTPrintfHex(ConfigVal);
	UARTPrintf(" ; ");
	BYTE status = SPI_Read_Register(STATUS);		
	nRF_PrintStatus(status);
}


void InitialiseAWU()
{
  AWU_CSR1_AWUEN = 0;     // Disable the Auto-wakeup feature.
  AWU_APR_APR = 11;             //0x0F 11 -> 6.3 sec
  AWU_TBR_AWUTB = 0x0F;// was 1
  AWU_CSR1_AWUEN = 1;     // Enable the Auto-wakeup feature.
}

void SendandSleep()
{
  BYTE dataTest[2];
  dataTest[0] = 0x57;
  dataTest[1] = 0x68;
  nRF_SetMode_TX();
  nRF_Transmit(dataTest,2);
  nRF_SetMode_PowerDown();
}


BYTE LedState = 0;

#pragma vector = AWU_vector
__interrupt void AWU_IRQHandler(void)
{
  volatile unsigned char reg;
  
  /*if(LedState == 0)
  {
    Test_Led_On();
    LedState = 1;  
  }
  else
  {
    
    LedState++;
    if(LedState == 2)
    {
      Test_Led_Off();  
      LedState = 0;
    }
  }*/
  
  SendandSleep();
  
  reg = AWU_CSR1;     // Reading AWU_CSR1 register clears the interrupt flag.
}


int main( void )
{
    //BYTE DS18B20_ROM[8];
    //BYTE temperature_2B[2];
    //BYTE counter = 0;


    __disable_interrupt();

    InitialiseSystemClock();
    FLASH_CR1_HALT = 1;    //Switch the Flash to power down in Active halt state, increse wakeup by few us
    FLASH_CR1_AHALT = 1;    //Switch the Flash to power down in Active halt state, increse wakeup by few us

    Initialise_TestLed_GPIO_B5();

    Test_Led_On();
    delay_1ms_Count(500);
    Test_Led_Off();

    InitialiseUART();

    /*Initialise_OneWire_Pin_A2();
    
    UARTPrintf("\r\n_________________________\n\r");
    UARTPrintf("Node(1) - Temperature\n\r");
    
    if(DS18B20_Single_ReadRom(DS18B20_ROM))
    {
      UARTPrintf("ROM = ");
      UARTPrintfHexTable(DS18B20_ROM,8);
      UARTPrintfLn("");
    }
    else
    {
      UARTPrintfLn("no sensor available\n\r");
    }*/
	
	
    
    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();
	
    InitialiseAWU();

    __enable_interrupt();
	
	
    //The TX Mode is independently set from nRF_Config() and can be changed on run time
    //nRF_SetMode_TX();
    UARTPrintfLn("into the while\n\r");

    while (1)
    {
      __halt();
	  /*
        if(DS18B20_Single_ReadTemperature(temperature_2B))
        {
          UARTPrintf("Temperature : ");
          UARTPrintfHex(temperature_2B[1]);
          UARTPrintf(" ");
          UARTPrintfHex(temperature_2B[0]);
          UARTPrintf(" = ");
          UARTPrint_DS18B20_Temperature(temperature_2B);
          UARTPrintfLn("");
        }
        else
        {
            UARTPrintfLn("Could not read temperature...\n\r");
        }

        counter++;
        status = nRF_Transmit(temperature_2B,2);
        UARTPrintf("Sent: ");
        UARTPrintfHex(counter);
        UARTPrintf("\n\r");
        
        Print_ConfigAndStatus_Registers();
        
        Test_Led_Off();
        delay_1ms_Count(4900);
        
        Test_Led_On();
        delay_1ms_Count(100);
		*/
    }
}
