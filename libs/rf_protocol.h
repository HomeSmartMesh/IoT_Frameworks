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
#define rf_pid_0x49_reset		0x49
#define rf_pid_0x59_rgb 		0x59
#define rf_pid_0x3B_light		0x3B
#define rf_pid_0xC5_magnet		0xC5
#define rf_pid_0xE2_bme280		0xE2

#define rf_pid_0x5F_retransmit  0x5F

#define rf_pid_0x84_test_ping   0x84
#define rf_pid_0x89_test_pong   0x89

#define rf_pid_0x87_test_switchChan     0x87
#define rf_pid_0x81_test_chanAck        0x81

// Alive RF ping
void rf_get_tx_alive_3B(BYTE NodeId, BYTE* tx_data);
void rx_alive(BYTE *rxData,BYTE rx_DataSize);//Rx 3 Bytes

// Reset
void rf_get_tx_reset_3B(BYTE NodeId, BYTE* tx_data);
void rx_reset(BYTE *rxData,BYTE rx_DataSize);//Rx 3 Bytes

//should move to max44009 lib
void rx_light(BYTE *rxData,BYTE rx_DataSize);//Rx 5 Bytes

//should move to magnet lib
void rx_magnet(BYTE *rxData,BYTE rx_DataSize);//Rx 4 Bytes


//id, GetSet, node, r, g, b, crc
void rgb_rf_get_tx_Color_7B(BYTE Target_NodeId,BYTE isSet,BYTE *txData,RGBColor_t Color);
//id, node, r, g, b, crc
void rgb_rf_get_tx_Color_6B(BYTE Target_NodeId,BYTE *txData,RGBColor_t Color);
