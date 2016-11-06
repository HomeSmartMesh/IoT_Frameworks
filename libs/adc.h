/** @file adc.h

 @author Wassim FILALI  STM8 S

 @compiler IAR STM8


 $Date: 06.11.2016
 $Revision:

*/

#ifndef __ADC__
#define __ADC__

#include "commonTypes.h"
#include "deviceType.h"

#if DEVICE_STM8S != 1
  #error this adc with 'deviceType.h'currently only supports STM8S
#endif

typedef BYTE ADC_Channel_t;
extern const ADC_Channel_t AIN3_PD2;
extern const ADC_Channel_t AIN4_PD3;
extern const ADC_Channel_t AIN5_PD5;
extern const ADC_Channel_t AIN6_PD6;

typedef BYTE ADC_Mode_t;
extern const ADC_Mode_t ADC_SINGLE_SHOT;
extern const ADC_Mode_t ADC_TIMER2;

typedef struct
{
	uint16_t	c1s_min;
	uint16_t	c1s_max;
	uint16_t	c1s_avg;
}adv_user_vals_t;

//channel is the Analog input AIN3
void adc_init(ADC_Channel_t channel, ADC_Mode_t mode);
void adc_start();
adv_user_vals_t adc_get_vals();
void adc_print_vals();
uint16_t adc_read();
void adc_acs712_print_current();

#endif /*__ADC__*/
