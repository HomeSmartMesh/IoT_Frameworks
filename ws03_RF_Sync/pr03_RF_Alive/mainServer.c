/*
	main.c for 
		IoT_Frameworks

	started	
	refactored	03.11.2016
	
*/

//Test Led is Port B Pin 5


#include "uart.h"
#include "clock_led.h"

//for nRF_Config() nRF_SetMode_RX() 
#include "nRF.h"

#include "nRF_Tx.h"

//for rx_pids and callbacks
#include "rf_protocol.h"

#define EEPROM_Offset 0x4000
#define EE_NODE_ID       (char *) EEPROM_Offset;
BYTE NodeId;

//User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
}

int main( void )
{
	
    BYTE Tx_Data[3];
    BYTE AliveActiveCounter = 0;
    
    NodeId = *EE_NODE_ID;


    rf_get_tx_alive_3B(NodeId,Tx_Data);
    
    InitialiseSystemClock();

    Initialise_TestLed_GPIO_B5();

    Test_Led_Off();

    uart_init();
	
    printf("\r\n__________________________________________________\n\r");
    printf("IoT_Framworks\\ws03_RF_Sync\\pr03_RF_Alive\\\n\r");

    //Applies the compile time configured parameters from nRF_Configuration.h
    BYTE status = nRF_Config();

    while (1)
    {

		AliveActiveCounter++;//Why are you counting ?
		
		Test_Led_Off();
		delay_ms(4900);
		
                nRF_Transmit(Tx_Data,3);
		Test_Led_On();
		delay_ms(100);
    }
}
