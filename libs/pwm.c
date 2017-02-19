/** @file pwm.c
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8S
 *
 *
 * $Date: 19.02.2017
 * $Revision:
 *
 */

//---------------------------- CONFIG ----------------------------
#include "deviceType.h"


//---------------------------- rest of includes ----------------------------
#include "pwm.h"

//for BYTE
#include "commonTypes.h"

//for IRQ
#include <intrinsics.h>

#if DEVICE_STM8S == 1
	#include <iostm8s103f3.h>
#elif
	#error only DEVICE_STM8S supported with deviceType.h
#endif

#define TIMER_PERIOD 65535

void timer1_init()
{
    TIM1_CR1_CEN = 0;//Counter Disable during configuration
	// 256 * 62500 * 1/16MHz = 1
	TIM1_CR1_OPM = 0;// One Pulse Mode : No
	TIM1_CR1_ARPE = 1;//ARR buffered
	//0x00 to 0xFFFF : fCK_CNT = fCK_PSC/2^(PSCR[15:0]+1);
	//0x0F : 16MHz/(2^(15+1)) = 1MHz => 1 us - step timer
	TIM1_PSCRL = 0x0F;

	//Auto Reload Register 1 ms => 1000 = TIMER_PERIOD
	TIM1_ARRL = 232;
	TIM1_ARRH = 20;

	TIM1_IER_UIE = 0;//Update Inetrrupt Disabled
	
	//------------------------------------------------
	//------------TIM1 - Channel 1 - pin C6
	//------------CCR : Capture Compare Register
    TIM1_CCR1H = 0;
    TIM1_CCR1L = 0;
	//------------CCMR : Capture Compare Mode Register
	TIM1_CCMR1_CC1S = 0x00;//00:CC1 channel configured as output
    TIM1_CCMR1_OC1M = 0x06;//PWM1 mode 1 till match then 0
    TIM1_CCMR1_OC1PE = 0x01;//Preload register enabled
	
	//------------TIM1 - Channel 2 - pin C7
	//------------CCR : Capture Compare Register
    TIM1_CCR2H = 0;
    TIM1_CCR2L = 0;
	//------------CCMR : Capture Compare Mode Register
	TIM1_CCMR2_CC2S = 0x00;//00:CC1 channel configured as output
    TIM1_CCMR2_OC2M = 0x06;//PWM1 mode 1 till match then 0
    TIM1_CCMR2_OC2PE = 0x01;//Preload register enabled
	//------------TIM1 - Channel 3 - pin C3:unused
	//------------CCR : Capture Compare Register
    TIM1_CCR3H = 0;
    TIM1_CCR3L = 0;
	//------------CCMR : Capture Compare Mode Register
	TIM1_CCMR3_CC3S = 0x00;//00:CC1 channel configured as output
    TIM1_CCMR3_OC3M = 0x06;//PWM1 mode 1 till match then 0
    TIM1_CCMR3_OC3PE = 0x01;//Preload register enabled
	//------------TIM1 - Channel 4 - pin C4
	//------------CCR : Capture Compare Register
    TIM1_CCR4H = 0;
    TIM1_CCR4L = 0;
	//------------CCMR : Capture Compare Mode Register
	TIM1_CCMR4_CC4S = 0x00;//00:CC1 channel configured as output
    TIM1_CCMR4_OC4M = 0x06;//PWM1 mode 1 till match then 0
    TIM1_CCMR4_OC4PE = 0x01;//Preload register enabled
	

    TIM1_CCER1_CC1P = 0;//Default active high
    TIM1_CCER1_CC1E = 1;//Enable output on pin
    TIM1_CCER1_CC2P = 0;//Default active high
    TIM1_CCER1_CC2E = 1;//Enable output on pin
    TIM1_CCER2_CC3P = 0;//Default active high
    TIM1_CCER2_CC3E = 1;//Enable output on pin
    TIM1_CCER2_CC4P = 0;//Default active high
    TIM1_CCER2_CC4E = 1;//Enable output on pin

    TIM1_IER_UIE = 0;//no interrupt
    TIM1_CR1_CEN = 1;//Counter Enable
}

void timer2_init()
{
  // 256 * 62500 * 1/16MHz = 1
  
 //CLK_PCKENR1_PCKEN15 = 1;//Timer 2 Clock enable - all already enabled
 
 TIM2_CR1_ARPE = 0;//use preload register
 
 TIM2_PSCR_PSC = 0x04;//0x00 to 0x0F : fCK_CNT = fCK_PSC/2^(PSC[3:0]); 1 => 2, 4 => 16 : 1MHz,  0x0F => 32768 : 
 
 //10 000 us
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

 
 TIM2_IER_UIE = 0;//interrupt not used
 TIM2_CR1_CEN = 1;//Counter Enable
}

void pwm_off_all()
{
	PWM_CH1_OUT = 0;
	PWM_CH2_OUT = 0;
	PWM_CH3_OUT = 0;
	PWM_CH4_OUT = 0;
}



void pwm1_init()
{
	//start with all pio off
	//pwm_off_all();
	
	// Pin C3 - TMR1 CH3
    PC_DDR_bit.DDR3 = 0;//input - alternate function

	// Pin C4 -  TMR1 CH4
    PC_DDR_bit.DDR4 = 0;//input - alternate function

	// Pin C6 -  TMR1 CH1
    PC_DDR_bit.DDR6 = 0;//input - alternate function

	// Pin C7 - TMR1 CH2
    PC_DDR_bit.DDR7 = 0;//input - alternate function

	timer1_init();
	
}

void pwm2_init()
{
	//start with all pio off
	//pwm_off_all();
	
    //D2 - TIM2 CH3
    PD_DDR_DDR2 = 0;//input - Alternate function

    //D3 - TIM2 CH2
    PD_DDR_DDR3 = 0;//input - Alternate function

    //C5 - TIM2 CH1
    PC_DDR_DDR5 = 0;//input - Alternate function

	timer2_init();
	
}

void pwm_init()
{
    pwm1_init();
    pwm2_init();
}


//channel is 1,2,3,4
//level is from 0 to 1.000
void pwm_set_level(BYTE channel, uint16_t level)
{
	/*if(level > TIMER_PERIOD)//should not shift to next cycle
	{
		level = TIMER_PERIOD;
	}*/
	BYTE level_L = level & 0x00FF;
	BYTE level_H = level >> 8;
    switch(channel)
    {
        case 1:{
            TIM1_CCR1L = level_L;
            TIM1_CCR1H = level_H;
        }break;
        case 2:{
            TIM1_CCR2L = level_L;
            TIM1_CCR2H = level_H;
        }break;
        case 3:{
            TIM1_CCR3L = level_L;
            TIM1_CCR3H = level_H;
        }break;
        case 4:{
            TIM1_CCR4L = level_L;
            TIM1_CCR4H = level_H;
        }break;
        case 5:{
            TIM2_CCR1L = level_L;
            TIM2_CCR1H = level_H;
        }break;
        case 6:{
            TIM2_CCR2L = level_L;
            TIM2_CCR2H = level_H;
        }break;
        case 7:{
            TIM2_CCR3L = level_L;
            TIM2_CCR3H = level_H;
        }break;
        default:{
            //do nothing
        }break;
    }

}