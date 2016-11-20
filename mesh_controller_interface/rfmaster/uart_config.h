/** @file uart_config.h
 *
 * @author Wassim FILALI
 * 
 *
 *
 * $Date: 12.11.2016
 * $Revision:
 *
 */


//----------------------------------- Interruptions ---------------------------------------
//Flag: UART_USE_RX_INETRRUPT
// 1 : Enabled
// declares the IRQ and requires teh user to declare a uart_callback() function

#define UART_USE_RX_INETRRUPT 1

#define UART_FRAME_SIZE 31

//the terminal sends 13
#define UART_EOL_C	13

//Flag: UART_Through_RF_Delay
//1 : Enabled this delay is added after every end of line to allow proper transmission of messages through radio
//0 : Disabled, normal behaviour
#define UART_Through_RF_Delay 0
