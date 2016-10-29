
//Test Led is Port B Pin 5


#include "uart.h"
#include "clock_led.h"

//for nRF_Config() nRF_SetMode_RX() 
#include "nRF.h"

#include "temp_ds18b20.h"

unsigned int SensorVal;

//User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
  //if(rx_DataSize == 2)//useless as rx_DataSize still not dynamically identified
  if(rxData[0]==0x35)
  {
    BYTE crc = rxData[0];
    for(int i=1;i<4;i++)
    {
      crc ^= rxData[i];
    }
    if(crc == rxData[4])
    {
      printf("NodeId:");
      UARTPrintf_uint(rxData[1]);
      printf(",Temperature:");
      UARTPrint_DS18B20_Temperature(rxData+2);
      UARTPrintfLn("");
    }
    else
    {
      printf("Protocol Id: 0x35, CRC Fail\n");
    }
    
  }
  else if(rxData[0]==0x75)
  {
    BYTE crc = rxData[0] ^ rxData[1];
    if(crc == rxData[2])
    {
      printf("NodeId:");
      UARTPrintf_uint(rxData[1]);
      printf(",is:Alive\r\n");
    }
    else
    {
      printf("Protocol Id: 0x75, CRC Fail\n");
    }
  }
  else if(rxData[0]==0x3B)//Light
  {
    BYTE crc = rxData[0] ^ rxData[1];
    if(crc == rxData[4])
    {
      printf("NodeId:");
      UARTPrintf_uint(rxData[1]);
      printf(",Light: ");
      SensorVal = rxData[2];
      SensorVal <<= 4;//shift to make place for the 4 LSB
      SensorVal = SensorVal + (0x0F & rxData[3]);
      UARTPrintf_uint(SensorVal);
      printf("\n");
    }
    else
    {
      printf("Protocol Id: 0x3B, CRC Fail\n");
    }
  }
  else if(rxData[0]==0xC5)
  {
    BYTE crc = rxData[0] ^ rxData[1] ^ rxData[2];
    if(crc == rxData[3])
    {
      printf("NodeId:");
      UARTPrintf_uint(rxData[1]);
      printf(",is:");
      if(rxData[2] == 0)
      {
        printf("Low\r\n");
      }
      else
      {
        printf("High\r\n");
      }
    }
    else
    {
      printf("Protocol Id: 0xC5, CRC Fail\r\n");
    }
  }
  else 
  {
    printf("Unexpected Protocol Id: ");
    UARTPrintfHex(rxData[0]);
    UARTPrintfLn("");
  }
  
}

int main( void )
{
	
    BYTE AliveActiveCounter = 0;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();

    Test_Led_Off();

    uart_init();
	
    printf("\r\n__________________________________________________\n\r");
    printf("sensors_logger\\firmware_rf_dongle\\rf_receiver_logger\\\n\r");

    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();

    //The RX Mode is independently set from nRF_Config() and can be changed on run time
    nRF_SetMode_RX();

    while (1)
    {

		AliveActiveCounter++;//Why are you counting ?
		
		Test_Led_Off();
		delay_1ms_Count(4900);
		
		Test_Led_On();
		delay_1ms_Count(100);
    }
}
