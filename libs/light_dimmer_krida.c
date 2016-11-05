/** @file light_dimmer_krida.c
 *
 * @author Wassim FILALI  STM8S
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 05.11.2016
 * $Revision:
 *
 */

#include "deviceType.h"

//for BYTE
#include "commonTypes.h"

//for IRQ
#include <intrinsics.h>

#include "light_dimmer_krida.h"

#if DEVICE_STM8S == 1
	#include <iostm8s103f3.h>
#elif
	#error only DEVICE_STM8S supported with deviceType.h
#endif

BYTE delay_H[4];
BYTE delay_L[4];
BYTE autoreload_H[4];
BYTE autoreload_L[4];

const U16_t PULSE_WIDTH_US = 10;

/*
#define DIMMER_SYNC_IN	PC_IDR_IDR3

#define DIMMER_CH1_OUT	PC_ODR_ODR4
#define DIMMER_CH2_OUT	PC_ODR_ODR5
#define DIMMER_CH3_OUT	PC_ODR_ODR6
#define DIMMER_CH4_OUT	PC_ODR_ODR7
*/

void timer2_ch1_pc5_OnePulseMode()
{
	// 256 * 62500 * 1/16MHz = 1
	//CLK_PCKENR1_PCKEN15 = 1;//Timer 2 Clock enable - all already enabled
	TIM2_CR1_OPM = 1;// One Pulse Mode : Counter stops counting at the next update event (clearing CEN bit)
	//TIM2_CR1_ARPE = 0;//use preload register : Optional when alternatively used in stand of OnePulseMode
	//0x00 to 0x0F : fCK_CNT = fCK_PSC/2^(PSC[3:0]); 1 => 2, 4 => 16 : 1MHz,  0x0F => 32768 : 
	//0x04 : 16MHz/(2^4) = 1MHz => 1 us - step timer
	TIM2_PSCR_PSC = 0x04;

	TIM2_IER_UIE = 0;//interrupt not used

	//------------TMR2 - Channel 1
	//------------CCMR : Capture Compare Mode Register
	//use Timer2 channel 1 on pin C5
	//			_OC1CE
	TIM2_CCMR1_OC1M = 0x07;//PWM mode 2  - RM0016 P177 : upcounting, active when > CCR1 same when down counting
	//			_OC1PE //Preload Anable : Needed but Not mandatory to be set in One Pulse Mode
	//			_OC1FE //Fast Enable
	TIM2_CCMR1_CC1S = 0x00;//00:CC1 channel configured as output
	//------------CCER : Capture Compare Enable Register
	TIM2_CCER1_CC1P = 0;//Default active high
	TIM2_CCER1_CC1E = 1;//Enable output on pin
	//------------IER : Interrupt Enable Register
	
}

void timer1_ch4_pc4_OnePulseMode()
{
	// 256 * 62500 * 1/16MHz = 1
	TIM1_CR1_OPM = 1;// One Pulse Mode : Counter stops counting at the next update event (clearing CEN bit)
	//TIM1_CR1_ARPE = 0;//use preload register : Optional when alternatively used in stand of OnePulseMode
	//0x00 to 0x0F : fCK_CNT = fCK_PSC/2^(PSC[3:0]); 1 => 2, 4 => 16 : 1MHz,  0x0F => 32768 : 
	//0x04 : 16MHz/(2^4) = 1MHz => 1 us - step timer
	TIM1_PSCRL = 0x04;

	//------------IER : Interrupt Enable Register
	TIM1_IER_UIE = 0;//interrupt not used

	//------------------------------------------------
	//------------TIM1 - Channel 4 - pin C4
	//------------CCMR : Capture Compare Mode Register
	//			_OC1CE
	TIM1_CCMR4_OC4M = 0x07;//PWM mode 2  - RM0016 P177 : upcounting, active when > CCR1 same when down counting
	//			_OC1PE //Preload Anable : Needed but Not mandatory to be set in One Pulse Mode
	//			_OC1FE //Fast Enable
	TIM1_CCMR4_CC4S = 0x00;//00:CC1 channel configured as output
	//------------CCER : Capture Compare Enable Register
	TIM1_CCER2_CC4P = 0;//Default active high
	TIM1_CCER2_CC4E = 1;//Enable output on pin
	
}

void dimmer_off_all()
{
	DIMMER_CH1_OUT = 0;
	DIMMER_CH2_OUT = 0;
	DIMMER_CH3_OUT = 0;
	DIMMER_CH4_OUT = 0;
}

void dimmer_init()
{
	//start with all pio off
	dimmer_off_all();
	
	__disable_interrupt();//in case it was enabled
	
	// Pin C3 - SYNC - IN
    PC_DDR_bit.DDR3 = 0;// Data Direction Register - 0: Input
    PC_CR1_bit.C13 = 0;// Control Register 1 - 0: Floating input
    PC_CR2_bit.C23 = 1;// Control Register 2 - 1: External interrupt "enabled"

	EXTI_CR1_PCIS = 1;//PCIS Port C Inetrrupt Sensitivity. 1 : Rising edge only
	//Alternate function for C3 not set to TLI so is this needed ? 
	EXTI_CR2_TLIS = 1;//  Falling edge only.
	
	
	// Pin C4 -  CH2 - OUT
    PC_DDR_bit.DDR4 = 1;// Data Direction Register - 1: Output
    PC_CR1_bit.C14 = 1;// Control Register 1 - 1: Push-pull
    PC_CR2_bit.C24 = 0;// Control Register 2 - 0: Output speed up to  2 MHz

	// Pin C5 -  CH3 - OUT
    PC_DDR_bit.DDR5 = 1;// Data Direction Register - 1: Output
    PC_CR1_bit.C15 = 1;// Control Register 1 - 1: Push-pull
    PC_CR2_bit.C25 = 0;// Control Register 2 - 0: Output speed up to  2 MHz

	// Pin C6 -  CH4 - OUT
    PC_DDR_bit.DDR6 = 1;// Data Direction Register - 1: Output
    PC_CR1_bit.C16 = 1;// Control Register 1 - 1: Push-pull
    PC_CR2_bit.C26 = 0;// Control Register 2 - 0: Output speed up to  2 MHz

	// Pin C7 - CH1 - OUT
    PC_DDR_bit.DDR7 = 1;// Data Direction Register - 1: Output
    PC_CR1_bit.C17 = 1;// Control Register 1 - 1: Push-pull
    PC_CR2_bit.C27 = 0;// Control Register 2 - 0: Output speed up to  2 MHz

	timer2_ch1_pc5_OnePulseMode();
	timer1_ch4_pc4_OnePulseMode();
	
}

int int_count = 0;

//interrupt on port C are on EXTI2 vector address : 0x00 801C
#pragma vector = EXTI2_vector
__interrupt void irq_sync(void)
{
	//This interrupt is bringing ~ 3 us of delay, with the minimal default 1us delay
	int_count++;

	TIM1_CCR4L = delay_L[0];
	TIM1_CCR4H = delay_H[0];
	TIM1_ARRL = autoreload_L[0];
	TIM1_ARRH = autoreload_H[0];
	TIM1_CR1_CEN = 1;//Timer 1 Counter Enable

	TIM2_CCR1L = delay_L[1];
	TIM2_CCR1H = delay_H[1];
	TIM2_ARRL = autoreload_L[1];
	TIM2_ARRH = autoreload_H[1];
	TIM2_CR1_CEN = 1;//Timer 2 Counter Enable
}

int get_int_count()
{
	return int_count;
}

//level is from 1 to 10.000

void dimmer_set_level(BYTE channel, U16_t level)
{
	//note minimal is 1 as 0 sets a pulse for all the time
	if(level == 0)
	{
		level++;
	}
	if((channel == 0) || (channel == 1))
	{
			delay_L[channel] = level & 0x00FF;
			delay_H[channel] = level >> 8;
			U16_t autoreload = level + PULSE_WIDTH_US;
			autoreload_L[channel] = autoreload & 0x00FF;
			autoreload_H[channel] = autoreload >> 8;
	}
		
}