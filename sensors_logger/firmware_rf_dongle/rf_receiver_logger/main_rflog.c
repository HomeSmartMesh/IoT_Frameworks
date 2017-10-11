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

//for nRF_Config() nRF_SetMode_RX() 
#include "nRF.h"

//to parse the RF response with rx_temperature_ds18b20()
#include "temp_ds18b20.h"

//for rx_pids and callbacks
#include "rf_protocol.h"
#include "rf_messages.h"

//for parsing rf bme280 data
#include "bme280.h"

#include "nRF_Tx.h"
#include "nRF_RegText.h"

#include "cmdutils.h"

#define EEPROM_Offset 0x4000
#define EE_NODE_ID       (char *) EEPROM_Offset;
unsigned char NodeId;

BYTE Led_Extend = 0;

//User Rx CallBack
void rf_Broadcast_CallBack(BYTE* rxHeader,BYTE *rxPayload,BYTE rx_PayloadSize)
{
	Led_Extend = 2;//signal retransmission
	switch(rxHeader[rfi_pid])
	{
		case rf_pid_0xB5_temperature:
			{
				rx_temperature_ds18b20(rxHeader[rfi_src],rxPayload,rx_PayloadSize);
			}
			break;
		case rf_pid_0xF5_alive:
			{
				rx_alive(rxHeader[rfi_src]);
			}
			break;
		case rf_pid_0xC9_reset:
			{
				rx_reset(rxHeader[rfi_src]);
			}
			break;
		case rf_pid_0xBB_light:
			{
				rx_light(rxHeader[rfi_src],rxPayload,rx_PayloadSize);
			}
			break;
		case rf_pid_0xC5_magnet:
			{
				rx_magnet(rxHeader[rfi_src],rxPayload,rx_PayloadSize);
			}
			break;
		case rf_pid_0xE2_bme280:
			{
				bme280_rx_measures(rxHeader[rfi_src],rxPayload,rx_PayloadSize);
			}
			break;
		default :
			{
				printf("Unknown Pid:");
				printf_hex(rxHeader[rfi_pid]);
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

BYTE TargetNodeId;
RGBColor_t Color;
BYTE send_rgb = 0;

void handle_command(BYTE *buffer,BYTE size)
{
	
	if(strbegins(buffer,"rgb") == 0)
	{
		//rgb NodeId R G B
		//rgb 0x00 0x00 0x00 0x00
		TargetNodeId = get_hex(buffer,4);
		Color.R = get_hex(buffer,9);
		Color.G = get_hex(buffer,14);
		Color.B = get_hex(buffer,19);
		send_rgb = 1;
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
    printf_ln("sensors_logger\\firmware_rf_dongle\\rf_receiver_logger\\");

	printf("NodeId:");printf_uint(NodeId);printf_ln(";is:RFLogging");

    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();
    nRF_SelectChannel(10);

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
		//uart_rx_user_poll();
		delay_ms(100);
		if(send_rgb)
		{
			rf_set_retries(20);
			rf_set_ack_delay(400);
			BYTE success = rf_rgb_set(TargetNodeId,Color);
			printf("NodeId:");
			printf_uint(NodeId);
			printf(";SetRGB_to:");
			printf_uint(TargetNodeId);
			printf(";R:");
			printf_uint(Color.R);
			printf(";G:");
			printf_uint(Color.G);
			printf(";B:");
			printf_uint(Color.B);
			printf(";Success:");
			printf_uint(success);
			printf_eol();
			send_rgb = 0;
		}
    }
}
