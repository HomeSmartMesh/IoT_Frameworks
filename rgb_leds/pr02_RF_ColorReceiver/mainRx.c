
//Test Led is Port B Pin 5


#include "ClockUartLed.h"

//for direct SPI_Read_Register()
#include "nRF_SPI.h"

//for nRF_SetMode_TX()
#include "nRF_Modes.h"

#include "nRF_Tx.h"
#include "nRF_RegText.h"

#include "nRF_IRQ.h"

#include "WS2812B.h"

//User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
  if(rx_DataSize == 4)
  {
    if(rxData[3] == 0x59)
    {
      RGBColor_t ColorRx;
      ColorRx.R = rxData[0];
      ColorRx.G = rxData[1];
      ColorRx.B = rxData[2];
      FlashColors(0,ColorRx);
      UARTPrintf("ColorRx : ");
      UARTPrintfHexTable(rxData,3);
      UARTPrintfLn("");
    }
    else
    {
      UARTPrintfLn("Mismatching id");
    }
  }
  else
  {
    UARTPrintf("Unexpected Format. Data Size: ");
    UARTPrintf_uint(rx_DataSize);
    UARTPrintf(" Data : ");
    UARTPrintfHexTable(rxData,rx_DataSize);
    UARTPrintf("\n\r");
  }
  
}

int main( void )
{
    RGBColor_t ColorRx;

    BYTE AliveActiveCounter = 0;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();
    Test_Led_Off();

    RGBLeds_PIO_Init();
    RGB_SwitchOff(0,1);//First led, one led
    
    InitialiseUART();
	
    UARTPrintf("\r\n_________________________\n\r");
    UARTPrintf("Color Receiver\n\r");

    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();

    //The RX Mode is independently set from nRF_Config() and can be changed on run time
    nRF_SetMode_RX();

    ColorRx.R = 40;
    ColorRx.G = 200;
    ColorRx.B = 160;
    FlashColors(5,ColorRx);
    UARTPrintf("Color Flashed 40,200,160\n\r");
    while (1)
    {

		AliveActiveCounter++;//Why are you counting ?
		
		Test_Led_Off();
		delay_ms(900);
		
		Test_Led_On();
		delay_ms(100);
    }
}
