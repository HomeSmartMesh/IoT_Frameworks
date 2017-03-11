/*
	main.c for 
		IoT_Frameworks
			\sensors_logger
				\firmware_rf_dongle
					\rf_tester

	started		11.03.2017
	
*/

//Test Led is Port B Pin 5


#include "uart.h"
#include "clock_led.h"

//for nRF_Config() nRF_SetMode_RX() 
#include "nRF.h"

#include "nRF_Tx.h"
#include "nRF_RegText.h"

#include "cmdutils.h"

#include "rf_protocol.h"

#define EEPROM_Offset 0x4000
#define EE_NODE_ID       (char *) EEPROM_Offset;
unsigned char NodeId;

BYTE Led_Extend = 0;
BYTE start_test = 0;
BYTE send_pong = 0;

void rf_ping()
{
	BYTE txByte;
	txByte = rf_pid_0x84_test_ping;
	printf_ln("sending ping");
	nRF_Transmit_Wait_Rx(&txByte,1);
}

void rf_pong()
{
	BYTE txByte;
	txByte = rf_pid_0x89_test_pong;
	printf_ln("sending pong");
	nRF_Transmit_Wait_Rx(&txByte,1);
}

//User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
	Led_Extend = 2;//signal retransmission
	switch(rxData[0])
	{
		case rf_pid_0x84_test_ping:
			{
				send_pong = 1;
			}
			break;
		default :
			{
				printf("Unknown Pid:");
				printf_hex(rxData[0]);
				printf_eol();
				Led_Extend = 1;//shorten the signal
			}
			break;
	}
}

void prompt()
{
	printf("Node");
	printf_hex(NodeId);
	printf_ln(">");
}

//From main() context
void rf_testChannels()
{
	printf_ln("rf_testChannels()");
	rf_ping();
	delay_ms(1);
}


void handle_command(BYTE *buffer,BYTE size)
{
	
	if(strbegins(buffer,"test") == 0)
	{
		start_test = 1;
	}
	else if(strcmp(buffer,"help") == 0)
	{
		printf_ln("see help in https://github.com/wassfila/IoT_Frameworks");
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
	
    BYTE AliveActiveCounter = 0;
    NodeId = *EE_NODE_ID;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();

    Test_Led_Off();

    uart_init();
	
    printf("\r\n__________________________________________________\n\r");
    printf("sensors_logger\\firmware_rf_dongle\\rf_tester\\\n\r");

    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();

    printf("__________________________________________________\n\r");
	nRF_PrintInfo();
    printf("__________________________________________________\n\r");

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
		if(start_test)
		{
			rf_testChannels();
		}
		if(send_pong)
		{
			rf_pong();
		}
    }
}
