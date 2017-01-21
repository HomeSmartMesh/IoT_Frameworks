/*
	main.c for 
		IoT_Frameworks
			\sensors_logger
				\firmware_rf_dongle
					\rf_receiver_logger

	started	
	refactored	29.10.2016
	
*/

//Test Led is Port B Pin 5


#include "uart.h"
#include "clock_led.h"

/*

NodeId:7;BME280: 0x53 EB 00 7C 77 00 70 08

NodeId:6;BME280: 0x56 33 00 7F 82 00 62 34
*/

void emulate_sensors_log(int count)
{
	count*=8;//0 -> 2040
	
	printf_ln("NodeId:7;BME280: 0x53 EB 00 7C 77 00 70 08");
	delay_ms(100);
	
	printf("NodeId:7,Light: ");							
	printf_uint(count);
	printf_eol();
	delay_ms(100);
  
	delay_ms(1000);
	printf_ln("NodeId:6;BME280: 0x56 33 00 7F 82 00 62 34");
	delay_ms(100);
	
	printf("NodeId:6,Light: ");							
	printf_uint(count);
	printf_eol();
	delay_ms(100);
}


int main( void )
{
	
    BYTE AliveActiveCounter = 0;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();

    Test_Led_Off();

    uart_init();
	
    printf("\r\n__________________________________________________\n\r");
    printf("sensors_logger\\firmware_rf_dongle\\rf_emulator\\\n\r");
    
    while (1)
    {
		AliveActiveCounter++;//Why are you counting ?
		
		Test_Led_Off();
		delay_ms(3900);
		
		Test_Led_On();
		delay_ms(100);
		emulate_sensors_log(AliveActiveCounter);
    }
}
