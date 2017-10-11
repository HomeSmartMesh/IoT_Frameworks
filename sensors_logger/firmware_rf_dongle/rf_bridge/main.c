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

#include "nRF_Configuration.h"

#include "nRF_RegText.h"

//for parsing rf bme280 data
#include "bme280.h"

#include "WS2812B.h"
//for NB_LEDS
#include "rgb_config.h"

#include "cmdutils.h"

BYTE Led_Extend = 0;

BYTE tx_data[RF_MAX_DATASIZE];

#define EEPROM_Offset 0x4000
#define EE_NODE_ID       (char *) EEPROM_Offset;
unsigned char NodeId;

void rf_Message_CallBack(BYTE* rxHeader,BYTE *rxPayload,BYTE rx_PayloadSize)
{
	Led_Extend = 2;//signal retransmission
	switch(rxHeader[rfi_pid])
	{
		case rf_pid_rgb:
			{
				rgb_decode_rf(rxPayload,rx_PayloadSize);
			}
			break;
		default :
			{
				//do nothing, not concerned, all other RF signals are just noise
				Led_Extend = 1;//shorten the signal
			}
			break;
	}
}

void rf_reset_bcast()
{
    tx_data[rfi_size] = rfi_broadcast_header_size;
    tx_data[rfi_pid] = rf_pid_0xC9_reset;
    tx_data[rfi_src] = NodeId;
    crc_set(tx_data);
   
	nRF_Transmit_Wait_Rx(tx_data,rfi_broadcast_header_size+crc_size);
}

int main( void )
{
    NodeId = *EE_NODE_ID;
    BYTE AliveActiveCounter = 0;

    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();
    Test_Led_Off();

    rgb_PIO_Init();
    //rgb_SwitchOff_Range(0,NB_LEDS);//(From led id 0, NB_LEDS leds)
	rgb_SendColor(0,0,1);

    uart_init();
	
    printf_eol();
    printf_ln("__________________________________________________");
    printf_ln("sensors_logger\\firmware_rf_dongle\\rf_bridge\\");
    
    printf("NodeId:");printf_uint(NodeId);printf_ln(";is:RFBridging");


    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();
    nRF_SelectChannel(10);

    printf("__________________________________________________\n\r");
	nRF_PrintInfo();
    printf("__________________________________________________\n\r");
	//notify that a reset happened
	rf_reset_bcast();

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
