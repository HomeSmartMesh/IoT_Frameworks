/** @file irq_debug.c
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 23.11.2016
 * $Revision:
 *
*/
#include <intrinsics.h>

#include "deviceType.h"

#if DEVICE_STM8L == 1
	#include <iostm8l151f3.h>
#elif DEVICE_STM8S == 1
	#include <iostm8s103f3.h>
#else
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif

#include "uart.h"


#if DEVICE_STM8L == 1

//#pragma vector = RTC_ALARM_vector
//__interrupt void irq_RTC_ALARM_vector(void){printf_ln("RTC_ALARM_vector");}

#pragma vector = EXTIE_vector
__interrupt void irq_EXTIE_vector(void){printf_ln(">>>EXTIE_vector");}

#pragma vector = EXTIB_vector
__interrupt void irq_EXTIB_vector(void){printf_ln(">>>EXTIB_vector");}

#pragma vector = EXTID_vector
__interrupt void irq_EXTID_vector(void){printf_ln(">>>EXTID_vector");}

#pragma vector = CLK_CSS_vector
__interrupt void irq_CLK_CSS_vector(void){printf_ln(">>>CLK_CSS_vector");}

#pragma vector = COMP_EF2_vector
__interrupt void irq_COMP_EF2_vector(void){printf_ln(">>>COMP_EF2_vector");}

#elif DEVICE_STM8S == 1

#pragma vector = EXTI3_vector
__interrupt void irq_EXTI3_vector(void)
{
	printf_ln("EXTI3_vector");
}

#else
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif

