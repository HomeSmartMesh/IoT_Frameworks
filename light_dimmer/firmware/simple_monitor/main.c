/*

	main.c for 
		IoT_Frameworks
			\light_dimmer
				\simple_monitor

	started	05.11.2016
	
*/

//Test Led is Port B Pin 5


#include "uart.h"
#include "clock_led.h"

#include "light_dimmer_krida.h"

#include <intrinsics.h>

int main( void )
{
	
	dimmer_init();//fast config - enables the PB4 interrupt

    BYTE AliveActiveCounter = 0;

    InitialiseSystemClock();


    Initialise_TestLed_GPIO_B5();
    Test_Led_Off();

    uart_init();
	
    printf("\r\n__________________________________________________\n\r");
    printf("IoT_Frameworks\\light_dimmer\\simple_monitor\\\n\r");

    __enable_interrupt();

    while (1)
    {

		printf("int count: ");
		printf_uint(get_int_count());
		printf_ln();

		AliveActiveCounter++;//Why are you counting ?
		
		Test_Led_Off();
		delay_1ms_Count(4900);
		
		Test_Led_On();
		delay_1ms_Count(100);
    }
}
