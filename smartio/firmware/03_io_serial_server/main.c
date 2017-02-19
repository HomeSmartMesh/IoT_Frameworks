/*

	main.c for 
		IoT_Frameworks
			\smartio
				\03_io_serial_server

	started	19.02.2017
	
*/

//---------- using the smart io Board Layout ----------

//------------------------------ PIO ------------------------------
//		Test Led 				-> Pin B5
//		PWM_CH1				-> Pin C6
//		PWM_CH2				-> Pin C7
//		PWM_CH3				-> Pin C3
//		PWM_CH4				-> Pin C4
//----------------------------------------------------------------------------

//------------------------- include all config files before all others -------------------------
#include "timers_config.h"


#include "uart.h"
#include "clock_led.h"

#include <iostm8s103f3.h>
#include <intrinsics.h>

#include "eeprom.h"
#include "cmdutils.h"

#include "pwm.h"

BYTE NodeId;
BYTE Dimmer_logon;

void Initialise_ULN_Outputs()
{
  PD_DDR_bit.DDR1 = 1;// 1: Output : Rx Pin
  PD_CR1_bit.C11 = 1;//   0: Push Pull
      
  PD_DDR_bit.DDR2 = 1;// 1: Output : Rx Pin
  PD_CR1_bit.C12 = 1;//   0: Push Pull

  PD_DDR_bit.DDR3 = 1;// 1: Output : Rx Pin
  PD_CR1_bit.C13 = 1;//   0: Push Pull

  PC_DDR_bit.DDR3 = 1;// 1: Output : Rx Pin
  PC_CR1_bit.C13 = 1;//   0: Push Pull

  PC_DDR_bit.DDR4 = 1;// 1: Output : Rx Pin
  PC_CR1_bit.C14 = 1;//   0: Push Pull

  PC_DDR_bit.DDR5 = 1;// 1: Output : Rx Pin
  PC_CR1_bit.C15 = 1;//   0: Push Pull

  PC_DDR_bit.DDR6 = 1;// 1: Output : Rx Pin
  PC_CR1_bit.C16 = 1;//   0: Push Pull

  PC_DDR_bit.DDR7 = 1;// 1: Output : Rx Pin
  PC_CR1_bit.C17 = 1;//   0: Push Pull

}


void Reset_ULN_Output()
{
  PD_ODR_bit.ODR1 = 0;
  PD_ODR_bit.ODR2 = 0;
  PD_ODR_bit.ODR3 = 0;

  PC_ODR_bit.ODR3 = 0;
  PC_ODR_bit.ODR4 = 0;
  PC_ODR_bit.ODR5 = 0;
  PC_ODR_bit.ODR6 = 0;
  PC_ODR_bit.ODR7 = 0;
}


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
	else if(strbegins(buffer,"pwm") == 0)
	{
		//pwm 0x05 0x08 0x98
		BYTE channel = get_hex(buffer,4);
		BYTE level_msb = get_hex(buffer,9);
		BYTE level_lsb = get_hex(buffer,14);
		uint16_t level = (uint16_t) (level_msb<<8) | level_lsb;
		pwm_set_level(channel,level);
		printf("pwm chan [");
		printf_uint(channel);
		printf("] to (");
		printf_uint(level);
		printf_ln(")");
	}
	else if(strbegins(buffer,"pwmall") == 0)
	{
		//pwmall 0x08 0x98
		BYTE level_msb = get_hex(buffer,7);
		BYTE level_lsb = get_hex(buffer,12);
		uint16_t level = (uint16_t) (level_msb<<8) | level_lsb;
		pwm_set_level(1,level);
		pwm_set_level(2,level);
		pwm_set_level(3,level);
		pwm_set_level(4,level);
		printf("pwmall [1,2,3,4] to level (");
		printf_uint(level);
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


int main( void )
{

	NodeId = *NODE_ID;

	//command interface parameters
	Dimmer_logon = 0;

	Initialise_ULN_Outputs();
	Reset_ULN_Output();

	InitialiseSystemClock();

	Initialise_TestLed_GPIO_B5();
	Test_Led_Off();

	pwm_init();

	uart_init();
	//No echo by default, can be enabled by terminal commad
	uart_echo = 1;

	//start after the rf node
	delay_ms(1000);
	printf_ln("____________________________");
	printf_ln("IoT_Frameworks/smartio/");
	printf_ln("io_serial_server/");


	pwm_set_level(1,0);
	pwm_set_level(2,10000);
	pwm_set_level(3,30000);
	pwm_set_level(4,65000);
	pwm_set_level(5,0);
	pwm_set_level(6,100);
	pwm_set_level(7,1000);
	delay_ms(3000);
	pwm_set_level(5,999);
	pwm_set_level(6,500);
	pwm_set_level(7,100);

	__enable_interrupt();
	
	
	prompt();
	
	uint8_t cycle_count = 0;
	while (1)
	{
		cycle_count++;
		//timer2_user_callback();
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
