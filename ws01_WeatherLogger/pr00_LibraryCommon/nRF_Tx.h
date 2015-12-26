/** @file nRF_Tx.h
 *
 * @author Wassim FILALI
 * Inferred from the Datasheet
 * 
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 12.12.2015
 * $Revision:
 *
 */
 
//for BYTE
#include "commonTypes.h"

BYTE nRF_Transmit(BYTE* payload, BYTE size);