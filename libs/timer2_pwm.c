/** @file timer2_pwm.c
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8S
 *
 *
 * $Date: 25.02.2017
 * $Revision:
 *
 */

//---------------------------- CONFIG ----------------------------
#include "deviceType.h"


//---------------------------- rest of includes ----------------------------
#include "timer2_pwm.h"

//for BYTE
#include "commonTypes.h"

//for IRQ
#include <intrinsics.h>

#if DEVICE_STM8S == 1
	#include <iostm8s103f3.h>
#elif
	#error only DEVICE_STM8S supported with deviceType.h
#endif

#define TIMER_PERIOD 255

BYTE ChanTime[8],TimerCycle;
BYTE ChanDuty[8];

#pragma vector = TIM2_OVR_UIF_vector
__interrupt void tim2_overflow(void)
{
	if(TIM2_SR1_UIF)//overflow
	{
        TimerCycle--;
        if(TimerCycle == 0)
        {
            TimerCycle = TIMER_PERIOD;
            ChanTime[0] = ChanDuty[0];
            TIMER2_PWM_CH0 = 1;
            TIMER2_PWM_CH1 = 1;
        }
        ChanTime[0]--;
        ChanTime[1]--;
        if(ChanTime[0] == 0)
        {
            TIMER2_PWM_CH0 = 0;
        }
        if(ChanTime[1] == 0)
        {
            TIMER2_PWM_CH1 = 0;
        }
		TIM2_SR1_UIF = 0;
	}
}

void timer2_init()
{
  // 256 * 62500 * 1/16MHz = 1
 //CLK_PCKENR1_PCKEN15 = 1;//Timer 2 Clock enable - all already enabled
 
 TIM2_CR1_ARPE = 0;//use preload register
 
 TIM2_PSCR_PSC = 0x04;//0x00 to 0x0F : fCK_CNT = fCK_PSC/2^(PSC[3:0]); 1 => 2, 4 => 16 : 1MHz,  0x0F => 32768 : 
 
 //100 us
 TIM2_ARRH = 0;
 TIM2_ARRL = 100;

 
 TIM2_IER_UIE = 1;//interrupt not used
 TIM2_CR1_CEN = 1;//Counter Enable
}

void pwm_all_off()
{
	TIMER2_PWM_CH0 = 0;
	TIMER2_PWM_CH1 = 0;
	TIMER2_PWM_CH2 = 0;
	TIMER2_PWM_CH3 = 0;
	TIMER2_PWM_CH4 = 0;
	TIMER2_PWM_CH5 = 0;
	TIMER2_PWM_CH6 = 0;
	TIMER2_PWM_CH7 = 0;
}

void pwm_all_on()
{
	TIMER2_PWM_CH0 = 1;
	TIMER2_PWM_CH1 = 1;
	TIMER2_PWM_CH2 = 1;
	TIMER2_PWM_CH3 = 1;
	TIMER2_PWM_CH4 = 1;
	TIMER2_PWM_CH5 = 1;
	TIMER2_PWM_CH6 = 1;
	TIMER2_PWM_CH7 = 1;
}



void timer2_pwm_init()
{
	//start with all pio off
	pwm_all_off();
	BYTE i;
    for(i = 0;i<8;i++)
    {
        ChanDuty[i] = 0;
    }

	timer2_init();
	
}

//channel is 1,2,3,4
//level is from 0 to 1.000
void timer2_pwm_set_level(BYTE channel, uint8_t level)
{
    ChanDuty[channel] = level;
}