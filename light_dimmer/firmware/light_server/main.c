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

//------------------------- include all config files before all others -------------------------
//#include "timers_config.h" to get the flag TIMER2_USER_CALLBACK
#include "timers_config.h"


#include "uart.h"
#include "clock_led.h"

#include "light_dimmer_krida.h"
#include <intrinsics.h>

#include "adc.h"
#include "eeprom.h"
#include "cmdutils.h"



BYTE NodeId;
BYTE Dimmer_logon;

void help()
{
	printf_ln("available commands:");
	printf_ln("dimm CHAN LEVEL_MSB LEVEL_LSB\n\t'dimm 0x00 0x08 0x98' sets the dimming level of channel 0 to 0x898");
	printf_ln("logon\tTurn the log on");
	printf_ln("logoff\tTurn the log off");
}

void prompt()
{
	printf("Node");
	printf_hex(NodeId);
	printf_ln(">");
}

void handle_command(BYTE *buffer,BYTE size)
{
	if(strcmp(buffer,"logon") == 0)
	{
		Dimmer_logon = 1;
		printf_ln("log is on");
	}
	else if(strcmp(buffer,"logoff") == 0)
	{
		Dimmer_logon = 0;
		printf_ln("log is off");
	}
	else if(strcmp(buffer,"uartechoon") == 0)
	{
		uart_echo = 1;
		printf_ln("UART Echo On");
	}
	else if(strcmp(buffer,"uartechooff") == 0)
	{
		uart_echo = 0;
		printf_ln("UART Echo Off");
	}
	else if(strbegins(buffer,"dimm") == 0)
	{
		//dimm 0x00 0x08 0x98
		BYTE channel = get_hex(buffer,5);
		BYTE level_msb = get_hex(buffer,10);
		BYTE level_lsb = get_hex(buffer,15);
		uint16_t level = (uint16_t) (level_msb<<8) | level_lsb;
		uint16_t delay = dimmer_set_level(channel,level);
		printf("dimm chan [");
		printf_uint(channel);
		printf("] to ");
		printf_uint(level);
		printf_eol();
		printf("  delay(");
		printf_uint(delay);
		printf_ln(")");
	}
	else if(strbegins(buffer,"alldimm") == 0)
	{
		//alldimm 0x08 0x98
		BYTE level_msb = get_hex(buffer,8);
		BYTE level_lsb = get_hex(buffer,13);
		uint16_t level = (uint16_t) (level_msb<<8) | level_lsb;
		uint16_t delay;
		delay = dimmer_set_level(0,level);
		delay = dimmer_set_level(1,level);
		delay = dimmer_set_level(2,level);
		delay = dimmer_set_level(3,level);
		printf_ln("alldimm [0,1,2,3] to level");
		printf_uint(level);
		printf(" => delay(");
		printf_uint(delay);
		printf_ln(")");
	}
	else if(strcmp(buffer,"help") == 0)
	{
		printf_ln("https://github.com/wassfila/IoT_Frameworks");
		help();
	}
	else if(size > 1)
	{
		printf_ln("Unknown Command, type 'help' for info");
	}
}

//UART Rx Callback
void uart_rx_user_callback(BYTE *buffer,BYTE size)
{
	//convert UART Text Terminal Format to String commands
	buffer[size-1] = '\0';//replace UART_EOL_C with string EoL

	handle_command(buffer,size);
	prompt();
}


#if TIMER2_USER_CALLBACK == 1
void timer2_user_callback()
{
	#if UART_CALLBACK_POLLING == 1
	if(uart_rx_user_callback_pending)
	{
		//use it by polling after the timer which is the most convenient time not to bother the dimming timings
		uart_rx_user_callback(uart_BUFFER,uart_index);
		uart_rx_user_callback_pending = 0;
		uart_rx_user_callback_performed = 1;
	}
	#endif
}
#endif


int main( void )
{

	NodeId = *NODE_ID;
	dimmer_init();//fast config - enables the PB4 interrupt

	//command interface parameters
	Dimmer_logon = 0;

	InitialiseSystemClock();


	Initialise_TestLed_GPIO_B5();
	Test_Led_Off();

	uart_init();
	//No echo by default, can be enabled by terminal commad
	uart_echo = 0;

	//start after the rf node
	delay_ms(1000);
	printf_ln("____________________________");
	printf_ln("IoT_Frameworks/light_dimmer/");
	printf_ln("light_server/");

	dimmer_set_level(0,1800);//60us is the Sync shift + 10 to be in the positive section
	dimmer_set_level(1,2200);//60us is the Sync shift
	dimmer_set_level(2,2600);//60us is the Sync shift
	dimmer_set_level(3,3000);//60us is the Sync shift

	//adc
#define DEF_ADC_TIMER2
#ifdef DEF_ADC_TIMER2
	adc_init(AIN3_PD2,ADC_TIMER2);
#else
	adc_init(AIN3_PD2,ADC_SINGLE_SHOT);
#endif

	__enable_interrupt();
	
	
#ifdef DEF_ADC_TIMER2
	adc_start();
#endif
	
	prompt();
	
	uint16_t last_count = get_int_count();
	uint16_t sync_count;
	uint8_t cycle_count = 0;
	while (1)
	{
		cycle_count++;
		timer2_user_callback();
		sync_count = get_int_count();
		if(Dimmer_logon)
		{
			printf("int sync_count: ");
			printf_uint(sync_count - last_count);
			last_count = sync_count;
			printf_eol();
			#ifdef DEF_ADC_TIMER2
					adc_print_vals();
					adc_acs712_print_current();
			#else
					printf("adc_read() = ");
							uint16_t val = adc_read();
					printf_uint(val);
					printf_eol();
			#endif
		}

		Test_Led_Off();
		delay_ms(10);

		if(cycle_count == 100)
		{
			Test_Led_On();
			delay_ms(20);
			cycle_count = 0;
		}
	}
}
