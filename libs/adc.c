/** @file adc.c

 @author Wassim FILALI  STM8 S

 @compiler IAR STM8


 $Date: 06.11.2016
 $Revision:

*/

#include "adc.h"

#include <iostm8s103f3.h>

typedef struct
{
	uint16_t	c20ms_min;
	uint16_t	c20ms_max;
	uint16_t	c20ms_avg;
	uint16_t	c1s_min;
	uint16_t	c1s_max;
	uint16_t	c1s_avg;
	BYTE		c20ms_count;
	BYTE		c1s_count;
}adv_vals_t;


const ADC_Channel_t AIN3_PD2 = 3;
const ADC_Channel_t AIN4_PD3 = 4;
const ADC_Channel_t AIN5_PD5 = 5;
const ADC_Channel_t AIN6_PD6 = 6;

const ADC_Mode_t ADC_SINGLE_SHOT = 1;
const ADC_Mode_t ADC_TIMER2 = 2;

ADC_Mode_t g_ADC_MODE;

adv_vals_t g_ADC_VALS;
adv_vals_t g_ADC_USER_VALS;

void adc_1s_buffer()
{
	g_ADC_USER_VALS.c1s_avg = g_ADC_VALS.c1s_avg;
	g_ADC_USER_VALS.c1s_min = g_ADC_VALS.c1s_min;
	g_ADC_USER_VALS.c1s_max = g_ADC_VALS.c1s_max;
}

void adc_1s_process()
{
	g_ADC_VALS.c1s_avg + = adc_val;
	if(adc_val < g_ADC_VALS.c1s_min)
	{
		g_ADC_VALS.c1s_min = adc_val;
	}
	else if(adc_val > g_ADC_VALS.c1s_max)
	{
		g_ADC_VALS.c1s_max = adc_val;
	}

	if(g_ADC_VALS.c1s_count == 50)
	{
		g_ADC_VALS.c1s_avg /= 50;
		adc_1s_buffer();
		g_ADC_VALS.c1s_min = 1024;
		g_ADC_VALS.c1s_max = 0;
		g_ADC_VALS.c1s_avg = 0;
		g_ADC_VALS.c1s_count = 0;
	}

	g_ADC_VALS.c1s_count++;
}

void adc_20ms_process(uint16_t adc_val)
{

	g_ADC_VALS.c20ms_avg + = adc_val;
	if(adc_val < g_ADC_VALS.c20ms_min)
	{
		g_ADC_VALS.c20ms_min = adc_val;
	}
	else if(adc_val > g_ADC_VALS.c20ms_max)
	{
		g_ADC_VALS.c20ms_max = adc_val;
	}

	if(g_ADC_VALS.c20ms_count == 20)
	{
		g_ADC_VALS.c20ms_avg /= 20;
		adc_1s_process();
		g_ADC_VALS.c20ms_min = 1024;
		g_ADC_VALS.c20ms_max = 0;
		g_ADC_VALS.c20ms_avg = 0;
		g_ADC_VALS.c20ms_count = 0;
	}

	g_ADC_VALS.c20ms_count++;
}


void timer2_init()
{

	//Clock - Peripheral Clock Enable Register - Peripheral Clock Enable 15
	//CLK_PCKENR1_PCKEN15 = 1;//Timer 2 Clock enable

	//TIM2_PSCR - Timer 2 Prescaler ------------------------------
	//0x00 to 0x0F : fCK_CNT = fCK_PSC/2^(PSC[3:0]); 1 => 2, 4 => 16 : 1MHz,  0x0F => 32768 : 
	//0x04 : 16MHz/(2^4) = 1MHz => 1 us - step timer
	TIM2_PSCR = 0x04;
	//TIM2_ARR - Auto Reload Register ----------------------------
	//1 ms => 1000 us = 3 * 256 + 232
	TIM2_ARRH = 3;
	TIM2_ARRL = 232;
	//TIM2 - IER - Interrupt Enable Register -----------------------
	TIM2_IER_UIE = 1;
}

void timer2_start()
{
	//TIM2 - CR1 - Control Register 1 - Counter Enable -------------------------------
	TIM2_CR1_CEN = 1;
}

uint16_t adc_conversion_start()
{
	//ADC_CR1 : Control Register 1 - AD on 0 -> 1  :Power On ; 1 -> 1 :Start Conversion
	if(ADC_CR1_ADON == 0)
	{
		//then first power on
		ADC_CR1_ADON = 1;
	}

	//Now start conversion
	ADC_CR1_ADON = 1;
}

uint16_t adc_get()
{
	uint16_t result;
	BYTE adc_lsb,adc_msb;

	//Now wait for the end of conversion
	//ADC_CSR : Control / Status Register - End Of Conversion, cleared by SW
	//while(ADC_CSR_EOC == 0);
	
	//ADC_CR1 : Control Register 1 - AD on 0 -> 1  :Power On ; 1 -> 1 :Start Conversion
	//ADC_CR1_ADON = 0;
	
	//ADC_CSR : Control / Status Register - End Of Conversion, cleared by SW
	ADC_CSR_EOC = 0;

	//make sure LSB is read first
	adc_lsb = ADC_DRL;
	adc_msb = ADC_DRH;
	result = adc_msb;
	result = (result << 8) + adc_lsb;
	
	return result;
}

#pragma vector = ADC1_EOC_vector
__interrupt void adc_end_of_conversion_irq()
{
	//ADC_CR1 : Control Register 1 - AD on 0 -> 1  :Power On ; 1 -> 1 :Start Conversion
	ADC_CR1_ADON = 0;

	//ADC_CSR : Control / Status Register - End Of Conversion, cleared by SW
	ADC_CSR_EOC = 0;

	uint16_t adc_val = adc_get();
	adc_20ms_process(adc_val);

}

#pragma vector = TIM2_OVR_UIF_vector
__interrupt void irq_timer2_1ms(void)
{
	//TIM2_SR1 : Status Register 1 - Update Interrupt Flag
	TIM2_SR1_UIF = 0;

	//start a new conversion for next cycle
	adc_conversion_start();
}

void adc_init(ADC_Channel_t channel, ADC_Mode_t mode)
{

	//ADC_CR1 - Control Register 1 - AD on 0 -> 1  :Power On ; 1 -> 1 :Start Conversion
	ADC_CR1_ADON = 1;
	//ADC - CR2 - Control Register 2 - Right Allignment, read LSB first --------------------------
	ADC_CR2_ALIGN = 1;
	//ADC - CSR - Channel selection --------------------------
	ADC_CSR_CH = channel;

	g_ADC_MODE = mode;
	if(mode == ADC_TIMER2)
	{
		timer2_init();
		g_VALS.c20ms_min    = 1024;
		g_VALS.c20ms_max    = 0;
		g_VALS.c20ms_avg    = 0;
		g_VALS.c1s_min      = 0;
		g_VALS.c1s_max      = 0;
		g_VALS.c1s_avg      = 0;
		g_VALS.c20ms_count  = 0;
		g_VALS.c1s_count    = 0;
	}
}

void adc_start()
{
	if(g_ADC_MODE == ADC_TIMER2)
	{
		timer2_start();
	}
	else
	{
		adc_conversion_start();
	}
}

void adc_stop()
{
	
}

uint16_t adc_read()
{
	uint16_t result;
	BYTE adc_lsb,adc_msb;
	//ADC_CR1 : Control Register 1 - AD on 0 -> 1  :Power On ; 1 -> 1 :Start Conversion
	ADC_CR1_ADON = 1;

	//Now wait for the end of conversion
	//ADC_CSR : Control / Status Register - End Of Conversion, cleared by SW
	while(ADC_CSR_EOC == 0);
	
	//ADC_CR1 : Control Register 1 - AD on 0 -> 1  :Power On ; 1 -> 1 :Start Conversion
	//ADC_CR1_ADON = 0;
	
	//ADC_CSR : Control / Status Register - End Of Conversion, cleared by SW
	ADC_CSR_EOC = 0;

	//make sure LSB is read first
	adc_lsb = ADC_DRL;
	adc_msb = ADC_DRH;
	result = adc_msb;
	result = (result << 8) + adc_lsb;
	
	return result;
}

