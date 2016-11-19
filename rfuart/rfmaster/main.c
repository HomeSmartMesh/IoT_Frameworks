/*
	main.c for 
		IoT_Frameworks
			\sensors_logger
				\firmware_rf_dongle
					\rf_bridge

	started	03.11.2016
	
*/

//Test Led is Port B Pin 5


#include "uart.h"

#include "clock_led.h"

//for nRF_Config() nRF_SetMode_RX() 
#include "nRF.h"

//direct usage of registers
#include "nRF_SPI.h"
#include "nRF_Tx.h"
#include "nRF_RegText.h"
//for CE pin get state
#include "spi_stm8x.h"

//to parse the RF response with rx_temperature_ds18b20()
#include "temp_ds18b20.h"

#include "eeprom.h"

#include "cmdutils.h"

BYTE NodeId;

BYTE rfmaster_LOG = 0;
BYTE rfmaster_DATA = 0;
BYTE rfmaster_Connected = 0;
BYTE rfmaster_HandleRFasCommands = 0;
BYTE rfmaster_HandleUARTasCommands = 1;

void prompt()
{
	printf("Node");
	printf_hex(NodeId);
	printf(">");
}

void help()
{
	printf("available commands:\n");
	printf("rfcmdon rfcmdoff\n\tEnables disable handling RF as commands\n");
	printf("logon\n\tTurn the log on\n");
	printf("logoff\n\tTurn the log off\n");
	printf("logtext\n\tLog in Text hex mode\n");
	printf("logdata\n\tLog in binary data mode\n");
	printf("readreg hADD\n\t'readreg 0x35' reads register adress: one space then 0x\n");
	printf("writereg hADD hVAL\n\t'writereg 0x00 0x33' writes vale 0x33 at address 0x00\n");
	printf("connectrf\n\t'Connects all UART receptions to RF transmissions\n");
	printf("disconnectrf\n\t'Disconnects UART receptions from RF transmissions\n");
	printf("standby\n\tSets the nRF into Standby Mode I\n");
	printf("listen\n\tSets the nRF into Reception Mode\n");
	printf("status\n\tPrints bit fields of the Status and Config registers\n");
	printf("channel RF_CH\n\t'channel 0x02' = 2400 + RF_CH[x1MHz] 2.400 GHz - 2.525 GHz\n");
	printf("rxaddress ADD\n\t'rxaddress 0xE7' sets the LSB of the Rx Address\n");
	printf("txaddress ADD\n\t'txaddress 0xE7' sets the LSB of the Tx Address\n");
}

void handle_command(BYTE *buffer,BYTE size)
{
	if(strbegins(buffer,"node") == 0)
	{
		BYTE node_id = get_hex(buffer,5);
		if(NodeId == node_id)
		{
			buffer+=10;
			printf("[node ");
			printf_hex(node_id);
			printf("]");
			handle_command(buffer,size-10);
		}
		else
		{
			printf("[not id]\n");
		}
	}
	else if(strcmp(buffer,"rfcmdon") == 0)
	{
		rfmaster_HandleRFasCommands = 1;
		printf("RF is treated as commands\n");
	}
	else if(strcmp(buffer,"rfcmdoff") == 0)
	{
		rfmaster_HandleRFasCommands = 0;
		printf("RF not treated as commands\n");
	}
	else if(strcmp(buffer,"uartcmdon") == 0)
	{
		rfmaster_HandleUARTasCommands = 1;
		printf("UART is treated as commands\n");
	}
	else if(strcmp(buffer,"uartcmdoff") == 0)
	{
		rfmaster_HandleUARTasCommands = 0;
		printf("UART not treated as commands\n");
	}
	else if(strcmp(buffer,"logon") == 0)
	{
		rfmaster_LOG = 1;
		printf("log is on\n");
	}
	else if(strcmp(buffer,"logoff") == 0)
	{
		rfmaster_LOG = 0;
		printf("log is off\n");
	}
	else if(strcmp(buffer,"logtext") == 0)
	{
		rfmaster_DATA = 0;
		printf("Log is in Text Mode\n");
	}
	else if(strcmp(buffer,"logdata") == 0)
	{
		rfmaster_DATA = 1;
		printf("log is in Data Mode\n");
	}
	else if(strbegins(buffer,"status") == 0)
	{
		nRF_PrintStatus(SPI_Read_Register(STATUS));
		nRF_PrintConfig(SPI_Read_Register(CONFIG));
		printf((CE_Pin_getstate() == 1)?"CE High\n":"CE Low\n");
		nRF_PrintChannel();
		printf(rfmaster_HandleUARTasCommands?"UART to commands\n":"UART not commands\n");
		printf(rfmaster_LOG?"RF to UART Log Enabled\n":"RF to UART Log Disabled\n");
		printf(rfmaster_DATA?"Log Data\n":"Log Text\n");
		printf(rfmaster_Connected?"UART send RF\n":"UART do not send RF\n");
		printf(rfmaster_HandleRFasCommands?"RF to commands\n":"RF not commands\n");

	}
	else if(strbegins(buffer,"standby") == 0)
	{
		nRF_SetMode_Standby_I();
		printf("Standby Mode Set\n");
	}
	else if(strbegins(buffer,"listen") == 0)
	{
		nRF_SetMode_RX();
		printf("Listening\n");
	}
	else if(strbegins(buffer,"channel") == 0)
	{
		BYTE channel = get_hex(buffer,8);
		if(channel > 125 )
		{
			channel = 125;//Max Freq is 2.525GHz
		}
		nRF_SelectChannel(channel);
		nRF_PrintChannel();
	}
	else if(strbegins(buffer,"txaddress") == 0)
	{
		BYTE address = get_hex(buffer,10);
		nRF_SetTxAddress(address);
		printf("TX_ADDR LSB set to ");
		printf_hex(address);
		printf_ln();
	}
	else if(strbegins(buffer,"rxaddress") == 0)
	{
		BYTE address = get_hex(buffer,10);
		nRF_SetTxAddress(address);
		printf("RX_ADDR_P0 LSB set to ");
		printf_hex(address);
		printf_ln();
	}
	else if(strbegins(buffer,"readreg") == 0)
	{
		BYTE regAddress = get_hex(buffer,8);
		printf("nRF reg ");
		printf_hex(regAddress);
		BYTE reg_val = SPI_Read_Register(regAddress);
		printf(" = ");
		printf_hex(reg_val);
		printf_ln();
	}
	else if(strbegins(buffer,"writereg") == 0)
	{
		BYTE regAddress = get_hex(buffer,9);
		BYTE regValue = get_hex(buffer,14);
		printf("nRF reg ");
		printf_hex(regAddress);
		printf(" <= ");
		printf_hex(regValue);
		printf_ln();
		SPI_Write_Register(regAddress,regValue);
		printf("nRF reg ");
		printf_hex(regAddress);
		BYTE reg_val = SPI_Read_Register(regAddress);
		printf(" = ");
		printf_hex(reg_val);
		printf_ln();
	}
	else if(strbegins(buffer,"connectrf") == 0)
	{
		rfmaster_Connected = 1;
		printf("connected\n");
	}
	else if(strbegins(buffer,"disconnectrf") == 0)
	{
		rfmaster_Connected = 0;
		printf("disconnected\n");
	}
	else if(strcmp(buffer,"help") == 0)
	{
		printf("https://github.com/wassfila/IoT_Frameworks\n");
		help();
	}
	else if((!rfmaster_Connected)&&(size > 1))
	{
		printf("Unknown Command, type 'help' for info\n");
	}
}

//UART Rx Callback
void uart_rx_user_callback(BYTE *buffer,BYTE size)
{
	//for commands parsing
	if(size < UART_FRAME_SIZE)
	{
		buffer[size] = '\0';
		size++;
	}
	//Node0x00>
	if(rfmaster_Connected)
	{
		//use UART_EOL_C for RF transmission
		if(size < UART_FRAME_SIZE)
		{
			buffer[size] = UART_EOL_C;
			size++;
		}
		nRF_Transmit(buffer,size);
		nRF_Wait_Transmit();
		nRF_SetMode_RX();
	}
	
	if(rfmaster_HandleUARTasCommands)
	{
		handle_command(buffer,size);
		prompt();
	}
	//printf_tab(buffer,size);
}

//RF User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
	if((rfmaster_LOG && rfmaster_DATA)||rfmaster_HandleRFasCommands)
	{
		rx_DataSize = line_length(rxData,rx_DataSize);
	}
	if(rfmaster_LOG)
	{
		if(rfmaster_DATA)
		{
			//is special case of converting bin to text with EOL
			//so use UART end Of Line
			rxData[rx_DataSize-1] = UART_EOL_C;
			print_data_tab(rxData,rx_DataSize);
		}
		else
		{
			printf("Rx: ");
			printf_tab(rxData,rx_DataSize);
			printf_ln();
		}
	}
	if(rfmaster_HandleRFasCommands)
	{
		//to handle it as a command the line should end with '\0' not '\n'
		rxData[rx_DataSize-1] = '\0';
		handle_command(rxData,rx_DataSize);
	}
}

int main( void )
{
    BYTE AliveActiveCounter = 0;
	NodeId = *NODE_ID;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();

    Test_Led_Off();

    uart_init();
	
    printf("\n__________________________________________________\n");
    printf("IoTFrameworks\\rfuart\\rfmaster\\\n");
    //no need for help every start up
	//help();

    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();

    //The RX Mode is independently set from nRF_Config() and can be changed on run time
    nRF_SetMode_RX();

	//initialize the nRF before running its script
	run_eeprom_script();
	
	printf("________\nRunning 'status' :\n");
	handle_command("status",7);
	
	if(rfmaster_HandleUARTasCommands)
	{
		prompt();
	}
	
    while (1)
    {

		AliveActiveCounter++;//Why are you counting ?
		
		Test_Led_Off();
		delay_1ms_Count(4900);
		
		Test_Led_On();
		delay_1ms_Count(100);
    }
}
