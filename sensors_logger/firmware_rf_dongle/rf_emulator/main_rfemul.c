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
NodeId:4,is:Alive
NodeId:7;BME280: 0x07 53 F5 00 7E F6 00 73 EB 
NodeId:7;BME280: 0x07 53 F7 00 7E F7 00 73 07 
NodeId:7;BME280: 0x07 53 F9 00 7E F5 00 73 BB 
NodeId:6,is:Alive
NodeId:6,Light: 1701
NodeId:4,is:Alive
NodeId:7;BME280: 0x07 53 F7 00 7E F7 00 73 6C 
NodeId:7;BME280: 0x07 53 F3 00 7E F1 00 74 3F 
NodeId:6,is:Alive
NodeId:6,Light: 1700
NodeId:7;BME280: 0x07 53 F6 00 7E ED 00 73 70 
NodeId:4,is:Alive
NodeId:7;BME280: 0x07 53 F5 00 7E EF 00 74 44 
*/

void emulate_sensors_log(int count)
{
	
	printf_ln("NodeId:7;BME280: 0x07 53 F5 00 7E F6 00 73 EB");		
	delay_ms(100);
	
	count*=8;//0 -> 2040
	printf("NodeId:7,Light: ");							
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
		delay_ms(4900);
		
		Test_Led_On();
		delay_ms(100);
		emulate_sensors_log(AliveActiveCounter);
    }
}
