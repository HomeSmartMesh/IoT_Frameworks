/** @file max44009.h
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 29.10.2016 - creation out of refactoring
 * $Revision: 1 
 *
*/

#include "commonTypes.h"


//still possible to develop

//void max44009_inetrrupt_status();
//void max44009_interrupt_enable();
//void max44009_threshold_set();

BYTE max44009_read_reg(BYTE address);

uint16_t max44009_read_light();
