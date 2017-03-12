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
BYTE NodeId;

BYTE Led_Extend = 0;

BYTE test_targetNodeId = 0;

BYTE test_ping_start = 0;
BYTE test_ping_nb = 0;

BYTE test_chan_start = 0;
BYTE test_chan_select = 0;

BYTE send_pong = 0;
int16_t rx_ping = 0;
BYTE rx_pong = 0;
BYTE rx_chan_ack = 0;

void rf_send_ping(BYTE target_id)
{
	BYTE txData[2];
	txData[0] = rf_pid_0x84_test_ping;
	txData[1] = target_id;
	//printf_ln("sending ping");
	nRF_Transmit_Wait_Rx(txData,2);
}

void rf_send_pong()
{
	BYTE txByte;
	txByte = rf_pid_0x89_test_pong;
	nRF_Transmit_Wait_Rx(&txByte,1);
}

//Triple redunduncy for a safe channel switching
void rf_send_switchChan(BYTE target_id,BYTE channel)
{
	BYTE txData[5];
	txData[0] = rf_pid_0x87_test_switchChan;
	txData[1] = target_id;
	txData[2] = channel;
	txData[3] = channel;
	txData[4] = channel;
	nRF_Transmit_Wait_Rx(txData,5);
}

void rf_send_chanAck()
{
	BYTE txByte;
	txByte = rf_pid_0x81_test_chanAck;
	//printf("Channel");printf_uint()
	nRF_Transmit_Wait_Rx(&txByte,1);
}

void rf_request_setChan(BYTE *rxData)
{
	//test that all three samples of channel are identical (error verification)
	if( (rxData[2] == rxData[3]) && (rxData[3] == rxData[4]) )
	{
		BYTE Requested_Channel = rxData[2];
		nRF_SelectChannel(Requested_Channel);
		printf("Switched Channel:");
		nRF_PrintChannel();
		//send a channel achknowledge on the newly selected channel
		rf_send_chanAck();
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
				//what do we do when we receive a ping request, 
				//check if we're the target, if do => We send a pong back
				if(rxData[1] == NodeId)
				{
					rf_send_pong();
				}
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
				if(rxData[1] == NodeId)
				{
					rf_request_setChan(rxData);
				}
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

BYTE rf_ping(BYTE target_id)
{
	rx_pong = 0;
	rf_send_ping(target_id);
	//maximum delay after which there is no statistical difference
	//in waiting a longer time
	delay_ms(30);
	return rx_pong;
}

//This multi ping completes all requests before returning
BYTE rf_many_pings(BYTE target_id,BYTE nb_requests)
{
	BYTE success = 0;
	BYTE i;
	for(i=0;i<nb_requests;i++)
	{
		success += rf_ping(target_id);//1 on success, 0 on fail
	}
	return success;
}

//This multi ping returns since the first success
BYTE rf_ping_retries(BYTE target_id,BYTE nb_retries)
{
	BYTE success = 0;
	BYTE i = 0;
	do
	{
		success = rf_ping(target_id);//1 on success, 0 on fail
		i++;
	}while((i<nb_retries) && (success == 0) );
	return success;
}

//From main() context
BYTE rf_test_many_pings(BYTE target_id,BYTE nb_requests)
{
	BYTE success = rf_many_pings(target_id,nb_requests);
	printf("Many Pings : ");
	printf_uint(success);printf("/");printf_uint(nb_requests);printf_eol();
	delay_ms(100);
	return success;
}

void rf_test_Switch_Channel(BYTE targetNodeId,BYTE channel)
{
	BYTE prev_Channel = nRF_GetChannel();
	//clear the acknowledge for a later check
	rx_chan_ack = 0;
	//send request to the remote
	rf_send_switchChan(targetNodeId,channel);
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
		printf_ln("Channel Switch attempt failed, have to find the Target now :");
		if(rf_ping_retries(targetNodeId,100))
		{
			printf("Channel Switch confirmed: ");
			nRF_PrintChannel();
		}
		else
		{
			nRF_SelectChannel(prev_Channel);
			if(rf_ping_retries(targetNodeId,100))
			{
				printf("Channel Switch canceled: ");
				nRF_PrintChannel();
			}
			else
			{
				printf_ln("We're in trouble, target unreachable");
			}
		}
	}
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
	//rfch 0x12 0x02 => rfch TargetNodeId ChannelToSet
	else if(strbegins(buffer,"rfch") == 0)
	{
		test_targetNodeId = get_hex(buffer,5);
		test_chan_select = get_hex(buffer,10);
		if( (test_chan_select > 1 ) && (test_chan_select <= 125 ) )
		{
			test_chan_start = 1;
		}
	}
	//rfch 0x02 => stch ChannelToSetOnHost
	else if(strbegins(buffer,"stch") == 0)
	{
		BYTE NewHostChannel = get_hex(buffer,5);
		if( (NewHostChannel > 1 ) && (NewHostChannel <= 125 ) )
		{
			nRF_SelectChannel(NewHostChannel);
			nRF_PrintChannel();
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
			rf_test_many_pings(test_targetNodeId,test_ping_nb);
			test_ping_start = 0;
		}
		if(test_chan_start)
		{
			rf_test_Switch_Channel(test_targetNodeId,test_chan_select);
			test_chan_start = 0;
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
