
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

#define EEPROM_Offset 0x4000
#define NODE_ID       (char *) EEPROM_Offset;
BYTE NodeId;

void Print_ConfigAndStatus_Registers()
{
	BYTE ConfigVal;
        UARTPrintf("  ");
	ConfigVal = SPI_Read_Register(CONFIG);
	UARTPrintf("nRF CONFIG: ");
	UARTPrintfHex(ConfigVal);
	UARTPrintf(" ; ");
	BYTE status = SPI_Read_Register(STATUS);		
	nRF_PrintStatus(status);
}


int main( void )
{
    BYTE DS18B20_ROM[8];
    BYTE temperature_2B[2];
    BYTE Tx_Data[5];
	
    BYTE counter = 0;

    NodeId = *NODE_ID;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();

    Test_Led_Off();

    InitialiseUART();

    Initialise_OneWire_Pin();
    
    UARTPrintf("\r\n_________________________\n\r");
    UARTPrintf("ws01_WeatherLogger\\01_Temperature_RF_Sender\\\n\r");
    UARTPrintf("Node(");
    UARTPrintf_uint(NodeId);
    UARTPrintf(") - Temperature\n\r");
    
    UARTPrintf("\r\nRF Protocol:\n");
    UARTPrintf("Protocol Id = 0x35, Node Id, Temperature Byte1, Temperature Byte2, CRC\n\r");

     delay_ms(500);//debug sensor unreadable

    if(DS18B20_Single_ReadRom(DS18B20_ROM))
    {
      UARTPrintf("DS18B20 ROM = ");
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
        Test_Led_On();
        delay_ms(100);

      //Loop counter
        UARTPrintf("Loop Counter");
        UARTPrintf_uint(counter);
        UARTPrintf("\n\r");
      //Temperature
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
            Tx_Data[0] = 0x09;
            Tx_Data[1] = 0x10;
            Tx_Data[2] = 0x11;
            Tx_Data[3] = 0x12;
            nRF_Transmit(Tx_Data,4);
        }
      //Transmission
        Tx_Data[0] = 0x35;//Protocol Identifier
        Tx_Data[1] = NodeId;//Node Identifier
        Tx_Data[2] = temperature_2B[0];//Temperature Byte 1
        Tx_Data[3] = temperature_2B[1];//Temperature Byte 2
        Tx_Data[4] = Tx_Data[0];//init CRC
        for(int i=1;i<4;i++)
        {
          Tx_Data[4] ^= Tx_Data[i];
        }
        status = nRF_Transmit(Tx_Data,5);
        UARTPrintf("Data Sent: ");
        for(int i=0;i<5;i++)
        {
          UARTPrintfHex(Tx_Data[i]);
          UARTPrintf(" ");
        }
        UARTPrintfLn("");
        
        Print_ConfigAndStatus_Registers();
        
        Test_Led_Off();
        delay_ms(4900);
        
        counter++;
    }
}
