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

#define TIMER_PERIOD 10000

BYTE ServoPos_H[3];
BYTE ServoPos_L[3];

#pragma vector = TIM2_OVR_UIF_vector
__interrupt void tim2_overflow(void)
{
  TIM2_SR1_UIF = 0;
  TIM2_SR1 = 0;//clear all other pending flags

  TIM2_CCR3H = ServoPos_H[0];
  TIM2_CCR3L = ServoPos_L[0];
  
  TIM2_CCR2H = ServoPos_H[1];
  TIM2_CCR2L = ServoPos_L[1];
  
  TIM2_CCR1H = ServoPos_H[2];
  TIM2_CCR1L = ServoPos_L[2];
}

void timer2_init()
{
  // 256 * 62500 * 1/16MHz = 1
  
 //CLK_PCKENR1_PCKEN15 = 1;//Timer 2 Clock enable - all already enabled
 
 TIM2_CR1_ARPE = 0;//use preload register
 
 TIM2_PSCR_PSC = 0x04;//0x00 to 0x0F : fCK_CNT = fCK_PSC/2^(PSC[3:0]); 1 => 2, 4 => 16 : 1MHz,  0x0F => 32768 : 
 
 //10 000 us = 39*256 + 16
 TIM2_ARRH = 39;
 TIM2_ARRL = 16;

 //1520 us = 5*256 + 240
 //1720 us = 6*256 + 184

 //------------TMR2 - Channel 1------------
 TIM2_CCR1H = 0;
 TIM2_CCR1L = 0;
 
 //use Timer2 PWM channel 3 on pin A3/D2
 TIM2_CCMR1_CC1S = 0x00;//CC1 channel configured as output
 TIM2_CCMR1_OC1M = 0x06;//PWM1 mode 1 till match then 0
 TIM2_CCMR1_OC1PE = 0x01;//Preload register enabled for TIM2_CCR1
 
 TIM2_CCER1_CC1P = 0;//Default active high
 TIM2_CCER1_CC1E = 1;//Enable output on pin

 
 //------------TMR2 - Channel 3------------
 TIM2_CCR3H = 0;
 TIM2_CCR3L = 0;
 
 //use Timer2 PWM channel 3 on pin A3/D2
 TIM2_CCMR3_CC3S = 0x00;//CC1 channel configured as output
 TIM2_CCMR3_OC3M = 0x06;//PWM1 mode 1 till match then 0
 TIM2_CCMR3_OC3PE = 0x01;//Preload register enabled for TIM2_CCR1
 
 TIM2_CCER2_CC3P = 0;//Default active high
 TIM2_CCER2_CC3E = 1;//Enable output on pin

 //------------TMR2 - Channel 2------------
 TIM2_CCR2H = 0;
 TIM2_CCR2L = 0;
 
 //use Timer2 PWM channel 3 on pin A3/D2
 TIM2_CCMR2_CC2S = 0x00;//CC1 channel configured as output
 TIM2_CCMR2_OC2M = 0x06;//PWM1 mode 1 till match then 0
 TIM2_CCMR2_OC2PE = 0x01;//Preload register enabled for TIM2_CCR1
 
 TIM2_CCER1_CC2P = 0;//Default active high
 TIM2_CCER1_CC2E = 1;//Enable output on pin

 
 TIM2_IER_UIE = 1;//interrupt used for smoothing
 TIM2_CR1_CEN = 1;//Counter Enable
}

void pwm_all_off()
{
    ServoPos_H[0] = 0;
    ServoPos_L[0] = 0;
    ServoPos_H[1] = 0;
    ServoPos_L[1] = 0;
    ServoPos_H[2] = 0;
    ServoPos_L[2] = 0;
}

void pwm_all_on()
{
    ServoPos_H[0] = 39;
    ServoPos_L[0] = 16;
    ServoPos_H[1] = 39;
    ServoPos_L[1] = 16;
    ServoPos_H[2] = 39;
    ServoPos_L[2] = 16;
}



void timer2_pwm_init()
{
	//start with all pio off
	pwm_all_off();

	timer2_init();
	
}

//channel is 1,2,3
//level is from 0 to 10.000
void timer2_pwm_set_level(BYTE channel, uint16_t level)
{
	if(level > TIMER_PERIOD)//should not shift to next cycle
	{
		level = TIMER_PERIOD;
	}
	BYTE level_L = level & 0x00FF;
	BYTE level_H = level >> 8;
    if(channel < 3)
    {
        ServoPos_H[channel] = level_H;
        ServoPos_L[channel] = level_L;
    }

}