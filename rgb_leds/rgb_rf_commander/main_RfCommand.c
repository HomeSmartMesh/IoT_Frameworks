/*
	main_RFCommand.c for 
		IoT_Frameworks
			\rg_leds
				\rgb_leds_rf

	started	    27.02.2017
	
*/

// ----------------------- Target Board ---------------------
// Fixed Node v2
// Port B Pin 5 => Test Led is
// SPI          => nRF
// UART         
// Port A Pin 3 => RGB WS2812B
// ---------------- Test with 60 RGB LED Bar ---------------- 

#include "uart.h"
#include "clock_led.h"

//for nRF_Config() nRF_SetMode_RX() 
#include "nRF.h"
//for nRF_Transmit
#include "nRF_Tx.h"

#include "WS2812B.h"
//for NB_LEDS
#include "rgb_config.h"

//for rx_pids and callbacks
#include "rf_messages.h"

#define EEPROM_Offset 0x4000
#define EE_NODE_ID       (char *) EEPROM_Offset;
unsigned char NodeId;

BYTE Led_Extend = 0;

int main( void )
{
    BYTE AliveActiveCounter = 0;
    RGBColor_t MyColors[5];
    NodeId = *EE_NODE_ID;
    
    MyColors[0] = BLACK;
    MyColors[1] = WHITE;
    MyColors[2].R = 7;MyColors[2].G = 10;MyColors[2].B = 5;
    MyColors[3] = GREEN;
    MyColors[4].R = 1;MyColors[4].G = 2;MyColors[4].B = 4;

    RGBColor_t ColorSend = BLACK;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();
    Test_Led_Off();

    rgb_PIO_Init();
    
    rgb_SwitchOff_Range(0,NB_LEDS);//(From led id 0, NB_LEDS leds)
    
    uart_init();

    printf_eol();
    printf_ln("__________________________________________________");
    printf_ln("rgb_leds\\rgb_leds_rf\\");

    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();

    while (1)
    {
		if(AliveActiveCounter == 5)
		{
			AliveActiveCounter = 0;
		}
        ColorSend = MyColors[AliveActiveCounter];
        rgb_FlashColors(0,ColorSend);
		BYTE TargetNodeID = 3;//3 - 18
		rf_rgb_set(TargetNodeID,ColorSend);

		Test_Led_On();
		delay_ms(100);
		Test_Led_Off();
		delay_ms(4900);
		AliveActiveCounter++;
    }
}
