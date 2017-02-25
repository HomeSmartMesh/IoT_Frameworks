/** @file timer2_pwm.h
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

//------------------------------ pwm ------------------------------
#include "commonTypes.h"
//----------------------- SMARTIO Board Layout --------------------
//		-> Pin D3 => Timer2 Chan 0
//		-> Pin D2 => Timer2 Chan 1
//		-> Pin D1 => Timer2 Chan 2
//		-> Pin C7 => Timer2 Chan 3
//		-> Pin C6 => Timer2 Chan 4
//		-> Pin C5 => Timer2 Chan 5
//		-> Pin C4 => Timer2 Chan 6
//		-> Pin C3 => Timer2 Chan 7
//------------------------------ Interrupts ------------------------------
//		-> Timer1 OVERFLOW IRQ
//----------------------------------------------------------------------------

#define TIMER2_PWM_CH0	PD_ODR_ODR3
#define TIMER2_PWM_CH1	PD_ODR_ODR2
#define TIMER2_PWM_CH2	PD_ODR_ODR1
#define TIMER2_PWM_CH3	PC_ODR_ODR7
#define TIMER2_PWM_CH4	PC_ODR_ODR6
#define TIMER2_PWM_CH5	PC_ODR_ODR5
#define TIMER2_PWM_CH6	PC_ODR_ODR4
#define TIMER2_PWM_CH7	PC_ODR_ODR3

void timer2_pwm_init();

//output function
void timer2_pwm_set_level(BYTE channel, uint16_t level);

