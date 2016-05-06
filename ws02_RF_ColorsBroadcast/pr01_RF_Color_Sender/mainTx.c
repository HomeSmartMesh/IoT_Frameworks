
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
    RGBColor_t MyColors[5];
    
    MyColors[0] = RED;
    MyColors[1] = WHITE;
    MyColors[2].R = 37;MyColors[2].G = 55;MyColors[2].B = 255;
    MyColors[3] = GREEN;
    MyColors[4].R = 255;MyColors[4].G = 137;MyColors[4].B = 20;
    RGBColor_t ColorSend = BLACK;
    BYTE Tx_Data[4];
    BYTE counter = 0;

    InitialiseSystemClock();

    InitialiseUART();

    RGBLeds_PIO_Init();
    SwitchOffLed();//Switch Off on startup
    
    UARTPrintf("\r\n_________________________\n\r");
    UARTPrintf("Node(1) - Colors Broadcast\n\r");
    
    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();

    //The TX Mode is independently set from nRF_Config() and can be changed on run time
    nRF_SetMode_TX();

    while (1)
    {
        counter++;
        if(counter == 5)
        {
          counter = 0;
        }
        //ColorSend = ColorScale(counter,255,GREEN,BLUE);
        ColorSend = MyColors[counter];
        FlashColors(0,ColorSend);
        Tx_Data[0]=ColorSend.R;
        Tx_Data[1]=ColorSend.G;
        Tx_Data[2]=ColorSend.B;
        Tx_Data[3]=0x59;
        status = nRF_Transmit(Tx_Data,4);
        UARTPrintf("Sent: ");
        UARTPrintfHex(counter);
        UARTPrintf("Color: ");
        UARTPrintfHexTable(Tx_Data,3);
        UARTPrintf("\n\r");
        
        delay_1ms_Count(2000);
    }
}
