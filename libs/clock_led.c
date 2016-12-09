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

void delay_ms(unsigned int n)
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

void Initialise_STM8L_Clock()
{
  //Set Clock to full speed
  CLK_CKDIVR_CKM = 0; // Set to 0x00 => /1 ; Reset is 0x03 => /8
  //unsigned char cal = CLK_HSICALR-45;//default is 0x66 lowered by 45
  //Unlock the trimming
  /*CLK_HSIUNLCKR = 0xAC;
  CLK_HSIUNLCKR = 0x35;
  CLK_HSITRIMR = cal;
  */
  
  
  //Enable RTC Peripheral Clock
  CLK_PCKENR2_PCKEN22 = 1;
  
  CLK_CRTCR_RTCDIV = 0x5;//reset value : RTC Clock source /32
  CLK_CRTCR_RTCSEL = 2;// 2:LSI; reset value 0
  while (CLK_CRTCR_RTCSWBSY != 0);        //  Pause while the RTC clock changes.
    
}

BYTE Initialise_STM8L_RTC_LowPower(uint16_t times_sec)
{
	BYTE result = 0;
    //unlock the write protection for RTC
    RTC_WPR = 0xCA;
    RTC_WPR = 0x53;
    
    RTC_ISR1_INIT = 1;//Initialisation mode
    
    //RTC_SPRERH_PREDIV_S = 0;// 7bits 0x00 Sychronous prescaler factor MSB
    //RTC_SPRERL_PREDIV_S = 0;// 8bits 0xFF Sychronous prescaler factor MSB
    //RTC_APRER_PREDIV_A = 0;// 7bits 0x7F Asynchronous prescaler factor

    RTC_CR1_FMT = 0;//24h format
    RTC_CR1_RATIO = 0;//fsysclk >= 2x fRTCclk
    // N.A RTC_CR1_BYPSHAD = 0;//shadow used no direct read from counters
    
    RTC_CR2_WUTE = 0;//Wakeup timer Disable to update the timer
	uint16_t count = 65535;
    while((RTC_ISR1_WUTWF==0)&&(count!=0) )
	{
		count--;
	}//
	if(count== 0)
	{
		//printf_ln(">>>Error : RTC_ISR1_WUTWF does not get to 1");
		result = 1;
	}
	
	//RTCCCLK = LSI/32 => 38KHz / 32 = 1187,5 Hz
    RTC_CR1_WUCKSEL = 0;//-b00 RTCCCLK/16 ;
    //(1/38KHz) / (32 * 16) => 13.474 ms

    //max time sec is 883 sec
	uint32_t time_us = times_sec * 1000000;
	uint16_t ticks = time_us / 13474;
	
    RTC_WUTRH_WUT = ticks / 256;
    RTC_WUTRL_WUT = ticks % 256;
    
    RTC_CR2_WUTE = 1;//Wakeup timer enable - starts downcounting
    RTC_CR2_WUTIE = 1;//Wakeup timer interrupt enable
    
    RTC_ISR1_INIT = 0;//Running mode

    //locking the write protection for RTC
    RTC_WPR = 0x00;
    RTC_WPR = 0x00;
    
    //wait that the internal VRef is stabilized before changing the WU options (Reference Manual)
	count = 65535;
    while((PWR_CSR2_VREFINTF==0)&&(count!=0) )
	{
		count--;
	}//
	if(count== 0)
	{
		//printf_ln(">>>Error : PWR_CSR2_VREFINTF does not get to 1");
		result = 2;
	}
    PWR_CSR2_ULP = 1;//Internal Voltage Reference Stopped in Halt Active Halt
    PWR_CSR2_FWU = 1;//Fast wakeup time
    
    return result;
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
