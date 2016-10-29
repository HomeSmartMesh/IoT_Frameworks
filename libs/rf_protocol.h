/** @file rf_protocol.h
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 29.10.2016 - creation
 * $Revision: 1 
 *
*/

#include "commonTypes.h"

//protocol ids for the first identification byte

#define rf_pid_0x35_temperature	0x35
#define rf_pid_0x75_alive		0x75
#define rf_pid_0x3B_light		0x3B
#define rf_pid_0xC5_magnet		0xC5
#define rf_pid_0xE2_bme280		0xE2


// Alive RF ping
void rf_get_tx_alive_3B(BYTE NodeId, BYTE* tx_data);
void rx_alive(BYTE *rxData,BYTE rx_DataSize);

//should move to max44009 lib
void rx_light(BYTE *rxData,BYTE rx_DataSize);

//should move to magnet lib
void rx_magnet(BYTE *rxData,BYTE rx_DataSize);

