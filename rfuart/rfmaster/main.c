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

//to parse the RF response with rx_temperature_ds18b20()
#include "temp_ds18b20.h"

#define EEPROM_Offset 0x1000
#define NODE_ID       (char *) EEPROM_Offset;

//UART Rx Callback
void uart_rx_user_callback(BYTE *buffer,BYTE size)
{
	BYTE NodeId = *NODE_ID;
	//simple echo
	printf("Node");
	printf_hex(NodeId);
	printf(">");
	//printf_tab(buffer,size);
}

//RF User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
	
	/*printf("Rx: ");
	printf_tab(rxData,rx_DataSize);
	printf_ln();*/
}

int main( void )
{
	
    BYTE AliveActiveCounter = 0;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();

    Test_Led_Off();

    uart_init();
	
    printf("\n__________________________________________________\n");
    printf("IoTFrameworks\\rfuart\\rfmaster\\\n");

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
