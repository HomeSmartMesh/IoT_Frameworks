/** @file light_dimmer_krida.h
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8S
 *
 *
 * $Date: 05.11.2015
 * $Revision:
 *
 */

//---------- Light Dimmer with servo Board Layout ----------

//------------------------------ PIO ------------------------------
//		DIMMER_SYNC 		-> Pin C3
//		CURRENT_SENSE 		-> Pin D2
//		DIMMER_CH1			-> Pin C4
//		DIMMER_CH2			-> Pin C5
//		DIMMER_CH3			-> Pin C6
//		DIMMER_CH4			-> Pin C7
//------------------------------ Interrupts ------------------------------
//		Sync Interrupt 		-> PortC IRQ
//		Channel Triggers 	-> Timer1 CAPCOM IRQ
//		Channel Triggers 	-> Timer1 OVERFLOW IRQ
//----------------------------------------------------------------------------

#define DIMMER_SYNC_IN	PC_IDR_IDR3

#define DIMMER_CH1_OUT	PC_ODR_ODR4
#define DIMMER_CH2_OUT	PC_ODR_ODR5
#define DIMMER_CH3_OUT	PC_ODR_ODR6
#define DIMMER_CH4_OUT	PC_ODR_ODR7

#define CURRENT_SENSE	PD_IDR_IDR2

void dimmer_init();
int get_int_count();

//the level is suggested, a 0 starting level, the returned delay is calculated with the Timer calibration period
uint16_t dimmer_set_level(BYTE channel, uint16_t level);

