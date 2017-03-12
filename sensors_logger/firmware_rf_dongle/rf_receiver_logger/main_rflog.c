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

//for parsing rf bme280 data
#include "bme280.h"

#include "nRF_Tx.h"
#include "nRF_RegText.h"

#include "cmdutils.h"

#define EEPROM_Offset 0x4000
#define EE_NODE_ID       (char *) EEPROM_Offset;
unsigned char NodeId;

BYTE Led_Extend = 0;

//forward declaration as recurisve call from each other
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize);

void handle_retransmission(BYTE *rxData,BYTE rx_DataSize)
{
	printf("RTX:");
	UARTPrintf_uint(rxData[1]);
	putc(';');
	userRxCallBack(rxData+2,rx_DataSize-2);
}

//User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
	Led_Extend = 2;//signal retransmission
	switch(rxData[0])
	{
		case rf_pid_0xB5_temperature:
			{
				rx_temperature_ds18b20(rxData,rx_DataSize);
			}
			break;
		case rf_pid_0xF5_alive:
			{
				rx_alive(rxData,rx_DataSize);
			}
			break;
		case rf_pid_0xC9_reset:
			{
				rx_reset(rxData,rx_DataSize);
			}
			break;
		case rf_pid_0xBB_light:
			{
				rx_light(rxData,rx_DataSize);
			}
			break;
		case rf_pid_0xC5_magnet:
			{
				rx_magnet(rxData,rx_DataSize);
			}
			break;
		case rf_pid_0xE2_bme280:
			{
				bme280_rx_measures(rxData,rx_DataSize);
			}
			break;
		case rf_pid_0xDF_retransmit:
		{
			handle_retransmission(rxData,rx_DataSize);
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

void handle_command(BYTE *buffer,BYTE size)
{
	
	if(strbegins(buffer,"rgb") == 0)
	{
		BYTE txData[6];
		RGBColor_t Color;
		//rgb NodeId R G B
		//rgb 0x00 0x00 0x00 0x00
		BYTE TargetNodeId = get_hex(buffer,4);
		Color.R = get_hex(buffer,9);
		Color.G = get_hex(buffer,14);
		Color.B = get_hex(buffer,19);
		rgb_rf_get_tx_Color_6B(TargetNodeId,txData,Color);
		nRF_Transmit_Wait_Rx(txData,6);
		printf("Node (");
		printf_uint(TargetNodeId);
		printf(") R ");
		printf_uint(Color.R);
		printf_eol();
		printf("  G ");
		printf_uint(Color.G);
		printf_eol();
		printf("  B ");
		printf_uint(Color.B);
		printf_eol();
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
    printf("sensors_logger\\firmware_rf_dongle\\rf_receiver_logger\\\n\r");

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
    }
}
