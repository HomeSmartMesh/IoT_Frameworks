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

//for transmit
#include "nRF_Tx.h"

//to parse the RF response with rx_temperature_ds18b20()
#include "temp_ds18b20.h"

//for rx_pids and callbacks
#include "rf_protocol.h"

//for parsing rf bme280 data
#include "bme280.h"

BYTE Led_Extend = 0;

BYTE tx_data[32];

void retransmit(BYTE timeToLive, BYTE *rxData,BYTE rx_DataSize)
{
	if(rx_DataSize < 30)//max was 31, now 2 more so <=29
	{
		tx_data[0] = rf_pid_0x5F_retransmit;
		tx_data[1] = timeToLive;
		BYTE* pData = tx_data+2;
		for(BYTE i=0;i<rx_DataSize;i++)
		{
			(*pData++) = (*rxData++);
		}
		nRF_Transmit(tx_data,rx_DataSize+2);
		nRF_Wait_Transmit();
		nRF_SetMode_RX();	//back to listening
		Led_Extend = 2;//signal retransmission
	}
	//else not subject to retransmission as size protocol does not allow
}

//User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
	Test_Led_On();
	if(rxData[0] == rf_pid_0x5F_retransmit)
	{
		BYTE ttl = rxData[1];
		if(ttl>0)
		{
			retransmit(ttl-1,rxData,rx_DataSize);
		}
	}
	else//new transmission
	{
		BYTE ttl = 2;
		retransmit(ttl,rxData,rx_DataSize);
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
    printf("sensors_logger\\firmware_rf_dongle\\rf_bridge\\\n\r");

    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();

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
