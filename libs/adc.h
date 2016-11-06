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


//channel is the Analog input AIN3
void adc_init(ADC_Channel_t channel, ADC_Mode_t mode);
uint16_t adc_read();

#endif /*__ADC__*/
