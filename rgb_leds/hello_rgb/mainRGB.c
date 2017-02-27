/*
	mainRGB.c for 
		IoT_Frameworks
			\rg_leds
				\hello_rgb

	started	    27.02.2017
	
*/

//------------------Target Board--------------
// Fixed Node v2
// Port B Pin 5 => Test Led is
// SPI          => nRF
// UART         
// Port A Pin 3 => RGB WS2812B

#include "clock_led.h"

#include "uart.h"

#include "WS2812B.h"

int main( void )
{
    RGBColor_t ColorRx;

    BYTE AliveActiveCounter = 0;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();
    Test_Led_Off();

    RGBLeds_PIO_Init();
    RGB_SwitchOff(0,1);//First led, one led
    
    uart_init();

    printf_eol();
    printf_ln("__________________________________________________");
    printf_ln("rgb_leds\\hello_rgb\\");

    ColorRx.R = 40;
    ColorRx.G = 200;
    ColorRx.B = 160;
    FlashColors(5,ColorRx);
    printf_ln("Color Flashed R:40, G:200, B:160");
    while (1)
    {
      Test_Led_Off();
      delay_ms(4900);
      
      Test_Led_On();
      delay_ms(100);

      AliveActiveCounter+=10;//Why are you counting ?
      //update the color with the live counter
      ColorRx.R = 40;
      ColorRx.G = AliveActiveCounter;
      ColorRx.B = 160 - AliveActiveCounter;
      //Apply the color and serialize it
      FlashColors(5,ColorRx);//delay is 5
      printf("Color Flashed R:");printf_uint(ColorRx.R);
      printf(", G:");printf_uint(ColorRx.G);
      printf(", B:");printf_uint(ColorRx.B);
      printf_eol();

      
    }
}
