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
BYTE start_chan = 0;
BYTE start_chan_target = 0;
BYTE send_pong = 0;
int16_t rx_ping = 0;
BYTE rx_pong = 0;
BYTE rx_chan_ack = 0;

void rf_send_ping()
{
	BYTE txByte;
	txByte = rf_pid_0x84_test_ping;
	//printf_ln("sending ping");
	nRF_Transmit_Wait_Rx(&txByte,1);
}

void rf_send_pong()
{
	BYTE txByte;
	txByte = rf_pid_0x89_test_pong;
	nRF_Transmit_Wait_Rx(&txByte,1);
}

void rf_send_switchChan(BYTE channel)
{
	BYTE txData[4];
	txData[0] = rf_pid_0x87_test_switchChan;
	txData[1] = channel;
	txData[2] = channel;
	txData[3] = channel;
	nRF_Transmit_Wait_Rx(txData,4);
}

void rf_chanAck()
{
	BYTE txByte;
	txByte = rf_pid_0x81_test_chanAck;
	//printf("Channel");printf_uint()
	nRF_Transmit_Wait_Rx(&txByte,1);
}

void rf_test_setChan(BYTE *rxData)
{
	//test that all three samples of channel are identical (error verification)
	if( (rxData[1] == rxData[2]) && (rxData[2] == rxData[3]) )
	{
		BYTE channel = rxData[1];
		nRF_SelectChannel(channel);
		printf("Switched to Channel:");printf_uint(channel);printf_eol();
		//send a channel achknowledge on the newly selected channel
		rf_chanAck();
	}
}

//User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
	Led_Extend = 2;//signal retransmission
	switch(rxData[0])
	{
		case rf_pid_0x84_test_ping:
			{
				rx_ping++;
				//what do we do when we receive a ping request, => We send a pong back
				rf_send_pong();
			}
			break;
		case rf_pid_0x89_test_pong:
			{
				//received a pong
				rx_pong = 1;
			}
			break;
		case rf_pid_0x87_test_switchChan:
			{
				rf_test_setChan(rxData);
			}
			break;
		case rf_pid_0x81_test_chanAck:
			{
				rx_chan_ack = 1;
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

BYTE rf_ping()
{
	rx_pong = 0;
	rf_send_ping();
	//maximum delay after which there is no statistical difference
	//in waiting a longer time
	delay_ms(30);
	return rx_pong;
}

BYTE rf_many_pings(BYTE nb_retries)
{
	BYTE success = 0;
	BYTE i;
	for(i=0;i<nb_retries;i++)
	{
		success += rf_ping();//1 on success, 0 on fail
	}
	return success;
}

//From main() context
BYTE rf_testSignal(BYTE nb_retries)
{
	BYTE success = 0;
	BYTE i;
	for(i=0;i<nb_retries;i++)
	{
		success += rf_ping();//1 on success, 0 on fail
	}
	printf("TxRx Chan ");	printf_uint(nRF_GetChannel());	printf(" : ");
	printf_uint(success);printf("/");	printf_uint(nb_retries);printf_eol();
	delay_ms(100);
	return success;
}

void rf_test_Switch_Channel(BYTE channel)
{
	BYTE oldChannel = nRF_GetChannel();
	//clear the acknowledge for a later check
	rx_chan_ack = 0;
	//send request to the remote
	rf_send_switchChan(channel);
	//switch immidiatly after sending the switch request to the remote
	//there's no point in waiting an ack as an ack can be lost as well
	//which would bring always one way or the other in an undefined switch state
	nRF_SelectChannel(channel);
	delay_ms(100);

	if(rx_chan_ack)
	{
		printf("Channel Switched to : ");printf_uint(channel);printf_eol();
	}
	else
	{
		printf_ln("Channel Switch fail, have to ping now");
		if(rf_many_pings(10))
		{
			nRF_PrintChannel();
		}
		else
		{
			nRF_SelectChannel(oldChannel);
			if(rf_many_pings(10))
			{
				nRF_PrintChannel();
			}
			else
			{
				printf_ln("We're in trouble, remote lost");
			}
		}
	}
}


void handle_command(BYTE *buffer,BYTE size)
{
	
	if(strbegins(buffer,"test") == 0)
	{
		start_test = 1;
	}
	//chan 0x02
	else if(strbegins(buffer,"chan") == 0)
	{
		start_chan_target = get_hex(buffer,5);
		if( (start_chan_target > 1 ) && (start_chan_target <= 125 ) )
		{
			start_chan = 1;
		}
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
			rf_testSignal(100);
			start_test = 0;
		}
		if(start_chan)
		{
			rf_test_Switch_Channel(start_chan_target);
			start_chan = 0;
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
			printf("Rx Chan ");	printf_uint(nRF_GetChannel());	printf(" : ");
			printf_uint(total_rx);printf_eol();
		}
    }
}
