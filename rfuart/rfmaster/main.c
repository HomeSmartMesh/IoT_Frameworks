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
#include "uart_config.h"
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

BYTE NodeId;

BYTE rfmaster_LOG = 0;
BYTE rfmaster_DATA = 0;
BYTE rfmaster_Connected = 0;
BYTE rfmaster_HandleRFasCommands = 0;

#define HELP_CMD_TEXT
#ifdef HELP_CMD_TEXT
const char help_cmd[] = "rfcmdon rfcmdoff\r\tEnables disable handling RF as commands\r";
const char help_logon[] = "logon\r\tTurn the log on\r";
const char help_logoff[] = "logoff\r\tTurn the log off\r";
const char help_logtext[] = "logtext\r\tLog in Text hex mode\r";
const char help_logdata[] = "logdata\r\tLog in binary data mode\r";
const char help_rfreadreg[] = "readreg hADD\r\t'readreg 0x35' reads register adress: one space then 0x\r";
const char help_rfwritereg[] = "writereg hADD hVAL\r\t'writereg 0x00 0x33' writes vale 0x33 at address 0x00\r";
const char help_connectrf[] = "connectrf\r\t'Connects all UART receptions to RF transmissions\r";
const char help_disconnectrf[] = "disconnectrf\r\t'Disconnects UART receptions from RF transmissions\r";
const char help_rfstandby[] = "standby\r\tSets the nRF into Standby Mode I\r";
const char help_rflisten[] = "listen\r\tSets the nRF into Reception Mode\r";
const char help_rfregs[] = "regs\r\tPrints bit fields of the Status and Config registers\r";
const char help_channel[] = "channel RF_CH\r\t'channel 0x02' = 2400 + RF_CH[x1MHz] 2.400 GHz - 2.525 GHz\r";
const char help_rxaddress[] = "rxaddress ADD\r\t'rxaddress 0xE7' sets the LSB of the Rx Address\r";
const char help_txaddress[] = "txaddress ADD\r\t'txaddress 0xE7' sets the LSB of the Tx Address\r";
#else
const char help_cmd[] = "";
const char help_logon[] = "";
const char help_logoff[] = "";
const char help_logtext[] = "";
const char help_logdata[] = "";
const char help_rfreadreg[] = "";
const char help_rfwritereg[] = "";
const char help_connectrf[] = "";
const char help_disconnectrf[] = "";
const char help_rfstandby[] = "";
const char help_rflisten[] = "";
const char help_rfregs[] = "";
const char help_channel[] = "";
const char help_rxaddress[] = "";
const char help_txaddress[] = "";
#endif

void prompt()
{
	printf("Node");
	printf_hex(NodeId);
	printf(">");
}

void help()
{
	printf("available commands:\r");
	printf(help_cmd);
	printf(help_logon);
	printf(help_logoff);
	printf(help_logtext);
	printf(help_logdata);
	printf(help_rfreadreg);
	printf(help_rfwritereg);
	printf(help_connectrf);
	printf(help_rfstandby);
	printf(help_rflisten);
	printf(help_rfregs);
	printf(help_channel);
	printf(help_rxaddress);
	printf(help_txaddress);
}

BYTE strcmp (BYTE * s1, const char * s2)
{
    for(; *s1 == *s2; ++s1, ++s2)
        if(*s1 == 0)
            return 0;
    return *(unsigned char *)s1 < *(unsigned char *)s2 ? -1 : 1;
}

BYTE strbegins (BYTE * s1, const char * s2)
{
    for(; *s1 == *s2; ++s1, ++s2)
        if(*s2 == 0)
            return 0;
    return (*s2 == 0)?0:1;
}

BYTE get_hex_char(BYTE c)
{
	BYTE res = 0;
	if(c <= '9')
	{
		res = c - '0';
	}
	else if(c <= 'F')
	{
		res = c - 'A' + 10;
	}
	else if(c <= 'f')
	{
		res = c - 'a' + 10;
	}
	return res;
}

BYTE get_hex(BYTE* buffer,BYTE pos)
{
	BYTE hex_val;
	pos+=2;//skip "0x"
	hex_val = get_hex_char(buffer[pos++]);
	hex_val <<= 4;
	hex_val |= get_hex_char(buffer[pos]);
	return hex_val;
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
	else if(strbegins(buffer,"regs") == 0)
	{
		nRF_PrintStatus(SPI_Read_Register(STATUS));
		nRF_PrintConfig(SPI_Read_Register(CONFIG));
		printf((CE_Pin_getstate() == 1)?"CE High\n":"CE Low\n");
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
		printf("Channel ");
		printf_uint(channel);
		printf(" selected : ");
		uint16_t freq = 2400 + channel;
		printf_uint(freq);
		printf(" MHz\n");
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
		printf("Unknown Command, type 'help' for info\r");
	}
}

//UART Rx Callback
void uart_rx_user_callback(BYTE *buffer,BYTE size)
{
	//Node0x00>
	if(rfmaster_Connected)
	{
		//use new line for RF transmission
		buffer[size] = '\n';
		nRF_Transmit(buffer,size+1);
		nRF_Wait_Transmit();
		nRF_SetMode_RX();
	}
	//for commands parsing
	if(size < UART_FRAME_SIZE)
	{
		  buffer[size] = '\0';
	}
	
	if(1)
	{
		handle_command(buffer,size);
	}
	prompt();
	//printf_tab(buffer,size);
}

BYTE line_length(BYTE*rxData,BYTE max_size)
{
	for(BYTE i=0;i<max_size;i++)
	{
		if(*rxData++ == '\n')
		{
			return i+1;//+1 is to keep the '\n'
		}
	}
	return max_size;
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
    help();
    prompt();

    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();

    //The RX Mode is independently set from nRF_Config() and can be changed on run time
    nRF_SetMode_RX();

    while (1)
    {

		AliveActiveCounter++;//Why are you counting ?
		
		Test_Led_Off();
		delay_1ms_Count(4900);
		
		Test_Led_On();
		delay_1ms_Count(100);
    }
}
