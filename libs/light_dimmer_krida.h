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

#define DIMMER_SYNC_IN	PC_IDR_IDR3

#define DIMMER_CH1_OUT	PC_ODR_ODR4
#define DIMMER_CH2_OUT	PC_ODR_ODR5
#define DIMMER_CH3_OUT	PC_ODR_ODR6
#define DIMMER_CH4_OUT	PC_ODR_ODR7


void dimmer_init();
int get_int_count();
void dimmer_set_level(BYTE channel, U16_t level);

