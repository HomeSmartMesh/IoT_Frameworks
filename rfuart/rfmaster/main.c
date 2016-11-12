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

//to parse the RF response with rx_temperature_ds18b20()
#include "temp_ds18b20.h"

#include "eeprom.h"

BYTE NodeId;

BYTE rfmaster_LOG = 0;
BYTE rfmaster_DATA = 0;

char help_logon[] = "logon\r\tTurn the log on\r";
char help_logoff[] = "logoff\r\tTurn the log off\r";
char help_logtext[] = "logtext\r\tLog in Text hex mode\r";
char help_logdata[] = "logdata\r\tLog in binary data mode\r";
char help_rfreadreg[] = "rfreadreg\r\t'rfreadreg 0x35' reads register adress: one space then 0x\r";
char help_rfwritereg[] = "rfwritereg\r\t'rfwritereg 0x00 0x33' writes vale 0x33 at address 0x00\r";


void prompt()
{
	printf("Node");
	printf_hex(NodeId);
	printf(">");
}

void help()
{
      printf("available commands:\r");
      printf(help_logon);
      printf(help_logoff);
      printf(help_logtext);
      printf(help_logdata);
      printf(help_rfreadreg);
      printf(help_rfwritereg);
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
		res = c - 'A';
	}
	else if(c <= 'f')
	{
		res = c - 'a';
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

//UART Rx Callback
void uart_rx_user_callback(BYTE *buffer,BYTE size)
{
	//Node0x00>
	if(size < UART_FRAME_SIZE)
	{
          buffer[size] = '\0';
	}
	
	if(strcmp(buffer,"logon") == 0)
	{
		rfmaster_LOG = 1;
		printf(help_logon);
	}
	else if(strcmp(buffer,"logoff") == 0)
	{
		rfmaster_LOG = 0;
		printf(help_logoff);
	}
	else if(strcmp(buffer,"logtext") == 0)
	{
		rfmaster_DATA = 0;
		printf(help_logtext);
	}
	else if(strcmp(buffer,"logdata") == 0)
	{
		rfmaster_DATA = 1;
		printf(help_logdata);
	}
	else if(strbegins(buffer,"rfreadreg") == 0)
	{
		BYTE regAddress = get_hex(buffer,10);
		printf("nRF reg ");
		printf_hex(regAddress);
		BYTE reg_val = SPI_Read_Register(regAddress);
		printf(" = ");
		printf_hex(reg_val);
		printf_ln();
	}
	else if(strbegins(buffer,"rfwritereg") == 0)
	{
		BYTE regAddress = get_hex(buffer,11);
		BYTE regValue = get_hex(buffer,16);
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
	else if(strcmp(buffer,"help") == 0)
	{
          help();
	}
	else
	{
		printf("Unknown Command, type 'help' for info\r");
	}
	prompt();
	//printf_tab(buffer,size);
}

//RF User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
	if(rfmaster_LOG)
	{
		if(rfmaster_DATA)
		{
			print_data_tab(rxData,rx_DataSize);
		}
		else
		{
			printf("Rx: ");
			printf_tab(rxData,rx_DataSize);
			printf_ln();
		}
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
