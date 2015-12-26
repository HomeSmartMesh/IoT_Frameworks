
//Test Led is Port B Pin 5


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


int main( void )
{
    BYTE DS18B20_ROM[8];
    BYTE temperature_2B[2];
	
    BYTE counter = 0;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();

    Test_Led_Off();

    InitialiseUART();

    Initialise_OneWire_Pin_A2();
    
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
    }
    
    
    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();

    //The TX Mode is independently set from nRF_Config() and can be changed on run time
    nRF_SetMode_TX();

    while (1)
    {
        if(DS18B20_Single_ReadTemperature(temperature_2B))
        {
          UARTPrintf("Temperature = ");
          UARTPrintfHex(temperature_2B[1]);
          UARTPrintf(" ");
          UARTPrintfHex(temperature_2B[0]);
          UARTPrintf(" ");
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
    }
}
