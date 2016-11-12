/** @file eeprom.h
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 12.11.2016
 * $Revision:
 *
*/

#include "deviceType.h"

#if DEVICE_STM8L == 1
	#define EEPROM_Offset 0x1000
#elif DEVICE_STM8S == 1
	#define EEPROM_Offset 0x4000
#else
  #error needs a file deviceType.h with DEVICE_STM8L 1  or S 1 defined
#endif

#define NODE_ID       (char *) EEPROM_Offset;
