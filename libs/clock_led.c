/** @file clock_led.h
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
#elif DEVICE_STM8S == 1
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
