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

//for rx_pids and callbacks
#include "rf_protocol.h"

//for parsing rf bme280 data
#include "bme280.h"

//User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
	switch(rxData[0])
	{
		case rf_pid_0x35_temperature:
			{
				rx_temperature_ds18b20(rxData,rx_DataSize);
			}
			break;
		case rf_pid_0x75_alive:
			{
				rx_alive(rxData,rx_DataSize);
			}
			break;
		case rf_pid_0x3B_light:
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
		default :
			{
				printf("Unknown Protocol Id: ");
				UARTPrintfHex(rxData[0]);
				UARTPrintfLn("");
			}
			break;
	}
}

int main( void )
{
	
    BYTE AliveActiveCounter = 0;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();

    Test_Led_Off();

    uart_init();
	
    printf("\r\n__________________________________________________\n\r");
    printf("sensors_logger\\firmware_rf_dongle\\rf_receiver_logger\\\n\r");

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
