/** @file rx_protocol.h
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

//protocol ids for the first identification byte

#define rx_pid_0x35_temperature	0x35
#define rx_pid_0x75_alive		0x75
#define rx_pid_0x3B_light		0x3B
#define rx_pid_0xC5_magnet		0xC5



void rx_alive(BYTE *rxData,BYTE rx_DataSize);

void rx_light(BYTE *rxData,BYTE rx_DataSize);

void rx_magnet(BYTE *rxData,BYTE rx_DataSize);

