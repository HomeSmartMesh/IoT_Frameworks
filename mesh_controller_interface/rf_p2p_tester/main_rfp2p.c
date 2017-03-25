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

#include "WS2812B.h"

#include "rf_messages.h"

#define EEPROM_Offset 0x4000
#define EE_NODE_ID       (char *) EEPROM_Offset;
BYTE NodeId;

BYTE Led_Extend = 0;

BYTE test_targetNodeId = 0;

BYTE test_ping_start = 0;
BYTE test_ping_nb = 0;
BYTE test_rgb_start = 0;

BYTE test_chan_start = 0;
BYTE test_chan_select = 0;

BYTE send_pong = 0;
BYTE rx_pong = 0;
BYTE rx_chan_ack = 0;


void rf_Message_CallBack(BYTE* rxHeader,BYTE *rxPayload,BYTE rx_PayloadSize)
{
	Led_Extend = 2;//signal retransmission
	switch(rxHeader[rfi_pid])
	{
		case rf_pid_ping:
			{
				printf_ln("received ping");
			}
			break;
		case rf_pid_rgb:
			{
				rgb_decode_rf(rxPayload,rx_PayloadSize);
			}
			break;
		default :
			{
				//do nothing, not concerned, all other RF signals are just noise
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

void handle_command(BYTE *buffer,BYTE size)
{
	//ping 0x09 0x64 => ping TargetNodeId nbRequests
	if(strbegins(buffer,"ping") == 0)
	{
		test_targetNodeId = get_hex(buffer,5);
		test_ping_nb = get_hex(buffer,10);
		test_ping_start = 1;
	}
	//rgb 0x03 => rgb Dest
	if(strbegins(buffer,"rgb") == 0)
	{
		test_targetNodeId = get_hex(buffer,4);
		test_rgb_start = 1;
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
    RGBColor_t MyColors[5];
	BYTE colorId = 0;
    
    MyColors[0] = BLACK;
    MyColors[1] = WHITE;
    MyColors[2].R = 7;MyColors[2].G = 10;MyColors[2].B = 5;
    MyColors[3] = GREEN;
    MyColors[4].R = 1;MyColors[4].G = 2;MyColors[4].B = 4;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();

    Test_Led_Off();

    uart_init();
	
	printf_eol();
    printf_ln("__________________________________________________");
    printf_ln("sensors_logger\\firmware_rf_dongle\\rf_tester\\");
	printf("Node Id: ");printf_uint(NodeId);printf_eol();


    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();

    printf("__________________________________________________\n\r");
	nRF_PrintInfo();
    printf("__________________________________________________\n\r");

    //The RX Mode is independently set from nRF_Config() and can be changed on run time
    nRF_SetMode_RX();

    rgb_PIO_Init();
	rgb_FlashColors(1,GREEN);

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
		if(test_ping_start)
		{
			rf_ping(test_targetNodeId);
			test_ping_start = 0;
		}
		if(test_rgb_start)
		{
			if(colorId == 5)colorId = 0;
			rf_rgb_set(test_targetNodeId,MyColors[colorId++]);
			printf("sent RGB to");printf_uint(test_targetNodeId);
			printf_eol();
			test_rgb_start = 0;
		}
    }
}
