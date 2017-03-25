/*
	mainLedRf.c for 
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

#include "WS2812B.h"
//for NB_LEDS
#include "rgb_config.h"

//for rx_pids and callbacks
#include "rf_protocol.h"

#include "rf_messages.h"


#define EEPROM_Offset 0x4000
#define EE_NODE_ID       (char *) EEPROM_Offset;
unsigned char NodeId;

BYTE Led_Extend = 0;

void rf_Message_CallBack(BYTE* rxHeader,BYTE *rxPayload,BYTE rx_PayloadSize)
{
	Led_Extend = 2;//signal retransmission
	switch(rxHeader[rfi_pid])
	{
		case rf_pid_rgb:
			{
				rgb_decode_rf(rxPayload,rx_PayloadSize);
			}
			break;
		default :
			{
				//do nothing, not concerned, all other RF signals are just noise
				Led_Extend = 1;//shorten the signal
			}
			break;
	}
}


int main( void )
{
    BYTE AliveActiveCounter = 0;
    NodeId = *EE_NODE_ID;

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


    //The RX Mode is independently set from nRF_Config() and can be changed on run time
    nRF_SetMode_RX();

    while (1)
    {
		AliveActiveCounter++;//Why are you counting ?
		if(Led_Extend != 0)
		{
			Test_Led_On();
			Led_Extend--;
		}
		else
		{
			Test_Led_Off();
		}
		delay_ms(100);
    }
}
