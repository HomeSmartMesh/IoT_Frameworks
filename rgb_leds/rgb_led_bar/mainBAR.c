/*
	mainRGB.c for 
		IoT_Frameworks
			\rg_leds
				\hello_rgb

	started	    27.02.2017
	
*/

// ----------------------- Target Board ---------------------
// Fixed Node v2
// Port B Pin 5 => Test Led is
// SPI          => nRF
// UART         
// Port A Pin 3 => RGB WS2812B
// ---------------- Test with 60 RGB LED Bar ---------------- 

#include "clock_led.h"

#include "uart.h"

#include "WS2812B.h"
//for NB_LEDS
#include "rgb_config.h"

int main( void )
{
    BYTE AliveActiveCounter = 0;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();
    Test_Led_Off();

    rgb_PIO_Init();
    rgb_SwitchOff_Range(0,NB_LEDS);//(From led id 0, NB_LEDS leds)
    
    uart_init();

    printf_eol();
    printf_ln("__________________________________________________");
    printf_ln("rgb_leds\\hello_rgb\\");

    printf_ln("Color Flashed R:40, G:200, B:160");
    while (1)
    {
      Test_Led_On();
      delay_ms(100);
      Test_Led_Off();

      //cycle a shading color from Blue to Red and Back to Blue over NB_LEDS RGB LED Bar
      printf("Color Loop. ");
      printf_uint(NB_LEDS);
      printf_ln(" Leds GREEN to BLUE then to RED"); 
      rgb_Loop_BlueRedBlue(NB_LEDS);

      printf("Cycle:");
      printf_uint(AliveActiveCounter);
      printf_eol();
      
    }
}
