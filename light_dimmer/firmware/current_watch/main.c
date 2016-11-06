/*

	main.c for 
		IoT_Frameworks
			\light_dimmer
				\simple_monitor

	started	05.11.2016
	
*/

//---------- Light Dimmer with servo Board Layout ----------

//------------------------------ PIO ------------------------------
//		Test Led 				-> Pin B5
//		DIMMER_SYNC 			-> Pin C3
//		CURRENT_SENSE 			-> Pin D2
//		DIMMER_CH1				-> Pin C4
//		DIMMER_CH2				-> Pin C5
//		DIMMER_CH3				-> Pin C6
//		DIMMER_CH4				-> Pin C7
//------------------------------ Interrupts ------------------------------
//		Sync Interrupt 			-> PortC IRQ
//		Channel Triggers On 	-> Timer1 CAPCOM IRQ
//		Channel Triggers Reset	-> Timer1 OVERFLOW IRQ
//		ADC Measures			-> Timer2 OVERFLOW IRQ
//----------------------------------------------------------------------------



#include "uart.h"
#include "clock_led.h"

#include "light_dimmer_krida.h"
#include <intrinsics.h>

#include "adc.h"

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

	dimmer_set_level(0,1800);//60us is the Sync shift + 10 to be in the positive section
	dimmer_set_level(1,2200);//60us is the Sync shift
	dimmer_set_level(2,2600);//60us is the Sync shift
	dimmer_set_level(3,3000);//60us is the Sync shift

        
	__enable_interrupt();
	
	//adc
	adc_init(AIN3_PD2,ADC_TIMER2);
	
	adc_start();
	
	
	U16_t last_count = get_int_count();
	while (1)
	{

		printf("int count: ");
		U16_t count = get_int_count();
		printf_uint(count - last_count);
		last_count = count;
		printf_ln();
		
		printf("adc_chin3_pd2 = ");
		printf_uint(adc_read());
		printf_ln();

		AliveActiveCounter++;//Why are you counting ?
		
		Test_Led_Off();
		delay_1ms_Count(4900);
		
		Test_Led_On();
		delay_1ms_Count(100);
	}
}
