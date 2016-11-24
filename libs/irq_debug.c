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

#pragma vector = FLASH_EOP_vector
__interrupt void irq_FLASH_EOP_vector(void){printf_ln(">>>FLASH_EOP_vector");}

#pragma vector = DMA1_CH0_HT_vector
__interrupt void irq_DMA1_CH0_HT_vector(void){printf_ln(">>>DMA1_CH0_HT_vector");}

#pragma vector = DMA1_CH2_HT_vector
__interrupt void irq_DMA1_CH2_HT_vector(void){printf_ln(">>>DMA1_CH2_HT_vector");}

#pragma vector = EXTIE_vector
__interrupt void irq_EXTIE_vector(void){printf_ln(">>>EXTIE_vector");}

#pragma vector = EXTIB_vector
__interrupt void irq_EXTIB_vector(void){printf_ln(">>>EXTIB_vector");}

#pragma vector = EXTID_vector
__interrupt void irq_EXTID_vector(void){printf_ln(">>>EXTID_vector");}

//#pragma vector = EXTI0_vector
//__interrupt void irq_EXTI0_vector(void){printf_ln(">>>EXTI0_vector");}

#pragma vector = EXTI1_vector
__interrupt void irq_EXTI1_vector(void){printf_ln(">>>EXTI1_vector");}

#pragma vector = EXTI2_vector
__interrupt void irq_EXTI2_vector(void){printf_ln(">>>EXTI2_vector");}

#pragma vector = EXTI3_vector
__interrupt void irq_EXTI3_vector(void){printf_ln(">>>EXTI3_vector");}

#pragma vector = EXTI4_vector
__interrupt void irq_EXTI4_vector(void){printf_ln(">>>EXTI4_vector");}

#pragma vector = EXTI5_vector
__interrupt void irq_EXTI5_vector(void){printf_ln(">>>EXTI5_vector");}

#pragma vector = EXTI6_vector
__interrupt void irq_EXTI6_vector(void){printf_ln(">>>EXTI6_vector");}

#pragma vector = EXTI7_vector
__interrupt void irq_EXTI7_vector(void){printf_ln(">>>EXTI7_vector");}

#pragma vector = CLK_CSS_vector
__interrupt void irq_CLK_CSS_vector(void){printf_ln(">>>CLK_CSS_vector");}

#pragma vector = COMP_EF2_vector
__interrupt void irq_COMP_EF2_vector(void){printf_ln(">>>COMP_EF2_vector");}

#pragma vector = TIM2_OVR_UIF_vector
__interrupt void irq_TIM2_OVR_UIF_vector(void){printf_ln(">>>TIM2_OVR_UIF_vector");}

#pragma vector = RI_vector
__interrupt void irq_COMP_RI_vector(void){printf_ln(">>>RI_vector");}

#pragma vector = SPI_TXE_vector
__interrupt void irq_SPI_TXE_vector(void){printf_ln(">>>SPI_TXE_vector");}

#pragma vector = USART_T_TXE_vector
__interrupt void irq_USART_T_TXE_vector(void){printf_ln(">>>USART_T_TXE_vector");}

#pragma vector = USART_R_RXNE_vector
__interrupt void irq_USART_R_RXNE_vector(void){printf_ln(">>>USART_R_RXNE_vector");}

//#pragma vector = I2C_SB_vector
//__interrupt void irq_I2C_SB_vector(void){printf_ln(">>>I2C_SB_vector");}



#elif DEVICE_STM8S == 1

#pragma vector = EXTI3_vector
__interrupt void irq_EXTI3_vector(void)
{
	printf_ln("EXTI3_vector");
}

#else
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif

