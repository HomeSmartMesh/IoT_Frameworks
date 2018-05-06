#ifndef __MAX_44009_H__
#define __MAX_44009_H__
/** @file max44009.h
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8 -> gcc
 *
 *
 * $Date: 29.10.2016 - creation out of refactoring
 * $Data: 06.05.2018 - port to nRF52 with nRF SDK 15 and gcc
 * $Revision: 2
 *
*/

#include <stdint.h>

#include "nrf_drv_twi.h"

uint16_t max44009_read_light();

#endif /*__MAX_44009_H__*/
