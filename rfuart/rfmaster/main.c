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

//to parse the RF response with rx_temperature_ds18b20()
#include "temp_ds18b20.h"

#include "eeprom.h"

BYTE NodeId;

BYTE RFMASTER_RX_Enabled = 0;
BYTE G;
BYTE B;



void prompt()
{
	printf("Node");
	printf_hex(NodeId);
	printf(">");
}

void help()
{
      printf("available commands:\r");
      printf("rxon\r");
      printf("Enable Reception\r");
      printf("rxoff\r");
      printf("Disable Reception\r");
}

BYTE strcmp (BYTE * s1, const char * s2)
{
    for(; *s1 == *s2; ++s1, ++s2)
        if(*s1 == 0)
            return 0;
    return *(unsigned char *)s1 < *(unsigned char *)s2 ? -1 : 1;
}

//UART Rx Callback
void uart_rx_user_callback(BYTE *buffer,BYTE size)
{
	//Node0x00>
	if(size < UART_FRAME_SIZE)
	{
          buffer[size] = '\0';
	}
	if(strcmp(buffer,"rxon") == 0)
	{
		RFMASTER_RX_Enabled = 1;
		printf("RFMASTER_RX_Enabled = 1;\r");
	}
	else if(strcmp(buffer,"rxoff") == 0)
	{
		RFMASTER_RX_Enabled = 0;
		printf("RFMASTER_RX_Enabled = 0;\r");
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
	if(RFMASTER_RX_Enabled)
	{
		printf("Rx: ");
		printf_tab(rxData,rx_DataSize);
		printf_ln();
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
