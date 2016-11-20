/*
	main.c for 
		IoT_Frameworks
			\rfuart
				\uarttester

	started	20.11.2016
	
*/

//Test Led is Port B Pin 5


#include "uart.h"

#include "clock_led.h"

#include "eeprom.h"

#include "light_dimmer_krida.h"

BYTE NodeId;


//UART Rx Callback
void uart_rx_user_callback(BYTE *buffer,BYTE size)
{
		//convert UART Text Terminal Format to String commands
		buffer[size-1] = '\0';//replace UART_EOL_C with string EoL
}

BYTE Dimmer_logon;

int main( void )
{
	BYTE AliveActiveCounter = 0;
	NodeId = *NODE_ID;
	dimmer_init();//fast config - enables the PB4 interrupt

	//command interface parameters
	Dimmer_logon = 0;

	InitialiseSystemClock();


	Initialise_TestLed_GPIO_B5();
	Test_Led_Off();

	uart_init();

	/*NodeId = *NODE_ID;

	InitialiseSystemClock();

	Initialise_TestLed_GPIO_B5();

	Test_Led_Off();

	uart_init();*/

	delay_ms(1000);
	printf_ln("____________________________");
	printf_ln("IoT_Frameworks/light_dimmer/");
	printf_ln("light_server/");
	delay_ms(1000);

	delay_ms(1000);
	printf_ln("_____________________");
	printf_ln("IoTFrameworks/rfuart/");
	printf_ln("uarttester");
	
        delay_ms(200);
        //the overflow is completely discarded from RF connection 
        //as not enough time to handle the Overflow section, no delay between characters
	printf_ln("012345678901234567890123456789_Overflow");
	printf_ln("012345678901234567890123456789_OverflowMore");
        delay_ms(200);

        
	while (1)
	{

		AliveActiveCounter++;//Why are you counting ?
		printf("Counter ");
		printf_uint(AliveActiveCounter);
		printf_eol();
		Test_Led_Off();
		delay_ms(4900);

		Test_Led_On();
		delay_ms(100);
	}
}
