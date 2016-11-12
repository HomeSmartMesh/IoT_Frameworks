/** @file clock_led.c
 *
 * @author Wassim FILALI  STM8 X
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 26.10.2016
 * $Revision:
 *
 */

#include "clock_led.h"

#include "deviceType.h"

#if DEVICE_STM8L == 1
	#include <iostm8l151f3.h>
#elif DEVICE_STM8S == 1
	#include <iostm8s103f3.h>
#else
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif


#define Test_Led_PB5 PB_ODR_ODR5

//
// Delay loop
//
// Actual delay depends on clock settings
// and compiler optimization settings.
//
void delay(unsigned int n)
{
    while (n-- > 0);
}

void delay_1ms_Count(unsigned int n)
{
    while (n-- > 0)
	{
		delay_1ms();
	}
}

#if DEVICE_STM8L == 1
	#define Test_Led_PB5 PB_ODR_ODR5
#elif DEVICE_STM8S == 1
	#define Test_Led_PB5 PB_ODR_ODR5
#else
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif

#if DEVICE_STM8L == 1
void Initialise_TestLed_GPIO_B5()
{
    PB_DDR_bit.DDR5 = 1;
    PB_CR1_bit.C15 = 0;
}

void InitialiseSystemClock()
{
}

#elif DEVICE_STM8S == 1

void InitialiseSystemClock()
{
	CLK_ICKR = 0;                       //  Reset the Internal Clock Register.
	CLK_ICKR_HSIEN = 1;                 //  Enable the HSI.
	CLK_ECKR = 0;                       //  Disable the external clock.
	while (CLK_ICKR_HSIRDY == 0);       //  Wait for the HSI to be ready for use.
	CLK_CKDIVR = 0;                     //  Ensure the clocks are running at full speed.
	//CLK_PCKENR1 = 0x00;               //  Do not change 
	//CLK_PCKENR2 = 0x00;               //  BootROM only enabled
	CLK_CCOR = 0;                       //  Turn off CCO.
	CLK_HSITRIMR = 0;                   //  Turn off any HSIU trimming.
	CLK_SWIMCCR = 0;                    //  Set SWIM to run at clock / 2.
	CLK_SWR = 0xe1;                     //  Use HSI as the clock source.
	CLK_SWCR = 0;                       //  Reset the clock switch control register.
	CLK_SWCR_SWEN = 1;                  //  Enable switching.
	while (CLK_SWCR_SWBSY != 0);        //  Pause while the clock switch is busy.
}

void Initialise_TestLed_GPIO_B5()
{
    PB_DDR_bit.DDR5 = 1;
    PB_CR1_bit.C15 = 0;
}
#else
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif

void Test_Led_On()
{
  Test_Led_PB5 = 0;
}
void Test_Led_Off()
{
  Test_Led_PB5 = 1;
}
