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

#include "nRF_P2P.h"

#define EEPROM_Offset 0x4000
#define EE_NODE_ID       (char *) EEPROM_Offset;
BYTE NodeId;

BYTE Led_Extend = 0;

BYTE test_ping_targetNodeId = 0;

BYTE test_ping_start = 0;
BYTE test_ping_nb = 0;

BYTE test_chan_start = 0;
BYTE test_chan_select = 0;

BYTE send_pong = 0;
int16_t rx_ping = 0;
BYTE rx_pong = 0;
BYTE rx_chan_ack = 0;


void rf_Message_CallBack(BYTE *rxData,BYTE rx_DataSize)
{
	Led_Extend = 2;//signal retransmission
	switch(rxData[0])
	{
		case rf_pid_0x79_rgb:
			{
				//set the RGB
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
		test_ping_targetNodeId = get_hex(buffer,5);
		test_ping_nb = get_hex(buffer,10);
		test_ping_start = 1;
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

void rf_safe_pings(BYTE test_ping_targetNodeId)
{
	rf_message_t msg;
	msg.dest = test_ping_targetNodeId;
	msg.nb_retries = 20;
	BYTE nb_retries = p2p_send_ping(&msg);

	printf("sent ping retries: ");printf_uint(nb_retries);printf_eol();

}

int main( void )
{
	
    BYTE AliveActiveCounter = 0;
    NodeId = *EE_NODE_ID;

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
			rf_safe_pings(test_ping_targetNodeId);
			test_ping_start = 0;
		}
		if(rx_ping)
		{
			int total_rx = 0;
			//started reception, wait till it's over
			do
			{
				total_rx += rx_ping;
				rx_ping = 0;	//clear reception
				delay_ms(70);	//did another reception happen in the meanwhile ?
			}while(rx_ping != 0);//if yes, then keep looping
			printf("Rx Pings : ");printf_uint(total_rx);printf_eol();
		}
    }
}
