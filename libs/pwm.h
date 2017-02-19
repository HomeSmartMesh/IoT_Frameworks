/** @file pwm.h
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

//------------------------------ pwm ------------------------------
#include "commonTypes.h"
//----------------------- SMARTIO Board Layout --------------------
//		-> Pin D3 => TMR2 CH2
//		-> Pin D2 => TMR2 CH3
//		-> Pin D1
//		-> Pin C7 => TMR1 CH2 Used
//		-> Pin C6 => TMR1 CH1 Used
//		-> Pin C5 => TMR2 CH1
//		-> Pin C4 => TMR1 CH4 Used
//		-> Pin C3 => TMR1 CH3 Used
//------------------------------ Interrupts ------------------------------
//		-> Timer1 OVERFLOW IRQ
//----------------------------------------------------------------------------

#define PWM_CH1_OUT	PC_ODR_ODR4
#define PWM_CH2_OUT	PC_ODR_ODR5
#define PWM_CH3_OUT	PC_ODR_ODR6
#define PWM_CH4_OUT	PC_ODR_ODR7

void pwm_init();

//output function
void pwm_set_level(BYTE channel, uint16_t level);

