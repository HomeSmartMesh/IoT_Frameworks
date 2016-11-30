/** @file clock_led.h
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 20.09.2015
 * $Revision:
 *
 */

#include "commonTypes.h"

#define delay_1us(); delay(0);

#define delay_10us(); delay(18);

#define delay_50us(); delay(98);

#define delay_100us(); delay(198);

#define delay_1ms(); delay(1998);

#define delay_10ms(); delay(19998);

void delay(unsigned int n);

void delay_ms(unsigned int n);

void InitialiseSystemClock();

// L E D 
void Initialise_TestLed_GPIO_B5();
void Test_Led_On();
void Test_Led_Off();


#if DEVICE_STM8L == 1
void Initialise_STM8L_Clock();
BYTE Initialise_STM8L_RTC_LowPower();
#endif

