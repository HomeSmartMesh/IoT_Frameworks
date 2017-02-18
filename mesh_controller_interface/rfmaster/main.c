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
	printf_ln("available commands:\n");
	printf_ln("rfcmdon rfcmdoff\tEnables disable handling RF as commands");
	printf_ln("uartechoon\tuartechooff\tEcho Typed Characters");
	printf_ln("logon\tTurn the log on");
	printf_ln("logoff\tTurn the log off");
	printf_ln("logtext\tLog in Text hex mode");
	printf_ln("logdata\tLog in binary data mode");
	printf_ln("readreg hADD\t'readreg 0x35' reads register adress: one space then 0x");
	printf_ln("writereg hADD hVAL\t'writereg 0x00 0x33' writes vale 0x33 at address 0x00");
	printf_ln("connectrf\t'Connects all UART receptions to RF transmissions");
	printf_ln("disconnectrf\t'Disconnects UART receptions from RF transmissions");
	printf_ln("standby\tSets the nRF into Standby Mode I");
	printf_ln("listen\tSets the nRF into Reception Mode");
	printf_ln("status\tPrints bit fields of the Status and Config registers");
	printf_ln("channel RF_CH\t'channel 0x02' = 2400 + RF_CH[x1MHz] 2.400 GHz - 2.525 GHz");
	printf_ln("rxaddress ADD\t'rxaddress 0xE7' sets the LSB of the Rx Address");
	printf_ln("txaddress ADD\t'txaddress 0xE7' sets the LSB of the Tx Address");
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
			printf_ln("[not id]");
		}
	}
	else if(strcmp(buffer,"rfcmdon") == 0)
	{
		rfmaster_HandleRFasCommands = 1;
		printf_ln("RF is treated as commands");
	}
	else if(strcmp(buffer,"rfcmdoff") == 0)
	{
		rfmaster_HandleRFasCommands = 0;
		printf_ln("RF not treated as commands");
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
	else if(strcmp(buffer,"uartcmdon") == 0)
	{
		rfmaster_HandleUARTasCommands = 1;
		printf_ln("UART is treated as commands");
	}
	else if(strcmp(buffer,"uartcmdoff") == 0)
	{
		rfmaster_HandleUARTasCommands = 0;
		printf_ln("UART not treated as commands");
	}
	else if(strcmp(buffer,"logon") == 0)
	{
		rfmaster_LOG = 1;
		printf_ln("log is on");
	}
	else if(strcmp(buffer,"logoff") == 0)
	{
		rfmaster_LOG = 0;
		printf_ln("log is off");
	}
	else if(strcmp(buffer,"logtext") == 0)
	{
		rfmaster_DATA = 0;
		printf_ln("Log is in Text Mode");
	}
	else if(strcmp(buffer,"logdata") == 0)
	{
		rfmaster_DATA = 1;
		printf_ln("log is in Data Mode");
	}
	else if(strbegins(buffer,"status") == 0)
	{
		nRF_PrintStatus(SPI_Read_Register(STATUS));
		nRF_PrintConfig(SPI_Read_Register(CONFIG));
		printf_ln((CE_Pin_getstate() == 1)?"CE High":"CE Low");
		nRF_PrintChannel();
		printf_ln(uart_echo?"UART Echo On":"UART Echo Off");
		printf_ln(rfmaster_DATA?"UART Tx Data":"UART Tx Text");
		printf_ln(rfmaster_HandleUARTasCommands?"UART Rx commands":"UART Rx not commands");
		printf_ln(rfmaster_Connected?"UART Rx to RF":"UART Rx not to RF");
		printf_ln(rfmaster_LOG?"RF to UART Tx":"RF not to UART Tx");
		printf_ln(rfmaster_HandleRFasCommands?"RF to commands":"RF not commands");

	}
	else if(strbegins(buffer,"standby") == 0)
	{
		nRF_SetMode_Standby_I();
		printf_ln("Standby Mode Set");
	}
	else if(strbegins(buffer,"listen") == 0)
	{
		nRF_SetMode_RX();
		printf_ln("Listening");
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
		printf_ln("TX_ADDR LSB set to ");
		printf_hex(address);
		printf_eol();
	}
	else if(strbegins(buffer,"rxaddress") == 0)
	{
		BYTE address = get_hex(buffer,10);
		nRF_SetTxAddress(address);
		printf("RX_ADDR_P0 LSB set to ");
		printf_hex(address);
		printf_eol();
	}
	else if(strbegins(buffer,"readreg") == 0)
	{
		BYTE regAddress = get_hex(buffer,8);
		printf("nRF reg ");
		printf_hex(regAddress);
		BYTE reg_val = SPI_Read_Register(regAddress);
		printf(" = ");
		printf_hex(reg_val);
		printf_eol();
	}
	else if(strbegins(buffer,"writereg") == 0)
	{
		BYTE regAddress = get_hex(buffer,9);
		BYTE regValue = get_hex(buffer,14);
		printf("nRF reg ");
		printf_hex(regAddress);
		printf(" <= ");
		printf_hex(regValue);
		printf_eol();
		SPI_Write_Register(regAddress,regValue);
		printf("nRF reg ");
		printf_hex(regAddress);
		BYTE reg_val = SPI_Read_Register(regAddress);
		printf(" = ");
		printf_hex(reg_val);
		printf_eol();
	}
	else if(strbegins(buffer,"connectrf") == 0)
	{
		rfmaster_Connected = 1;
		printf_ln("connected");
	}
	else if(strbegins(buffer,"disconnectrf") == 0)
	{
		rfmaster_Connected = 0;
		printf_ln("disconnected");
	}
	else if(strcmp(buffer,"help") == 0)
	{
		printf_ln("https://github.com/wassfila/IoT_Frameworks");
		help();
	}
	else if((!rfmaster_Connected)&&(size > 1))
	{
		printf_ln("Unknown Command,type 'help'");
	}
}

//UART Rx Callback
void uart_rx_user_callback(BYTE *buffer,BYTE size)
{

	//Node0x00>
	if(rfmaster_Connected)
	{
		//convert UART Text to RF Fixed size format done inside nRF_Transmit()
		//if size >= RF_MAX_DATASIZE, will return without doing anything
		nRF_Transmit(buffer,size);
		nRF_Wait_Transmit();
		nRF_SetMode_RX();
	}
	
	if(rfmaster_HandleUARTasCommands)
	{
		//convert UART Text Terminal Format to String commands
		buffer[size-1] = '\0';//replace UART_EOL_C with string EoL
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
			printf_eol();
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
	
    printf_ln("_______________________");
    printf_ln("IoTFrameworks/rfuart/");
	printf_ln("rfmaster/");
    //no need for help every start up
	//help();

    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();

    //The RX Mode is independently set from nRF_Config() and can be changed on run time
    nRF_SetMode_RX();

	//initialize the nRF before running its script
	run_eeprom_script();
	
	printf_ln("________");
	printf_ln("Running 'status' :");
	handle_command("status",7);
	
	if(rfmaster_HandleUARTasCommands)
	{
		//DEBUG prompt();
	}
	
    while (1)
    {

		AliveActiveCounter++;//Why are you counting ?
		
		Test_Led_Off();
		delay_ms(4900);
		
		Test_Led_On();
		delay_ms(100);
    }
}
