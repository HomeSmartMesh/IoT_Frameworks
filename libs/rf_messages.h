/** @file rf_messages.h
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 12.03.2017
          25.03.2017 - renamed from nRF_P2P.h to rf_messages.h
 * $Revision:
 *
 */
//When using the Higher level p2p RF Functions,
//userRxCallBack() must not be instantiated, as it is handled here
//if the user needs to receive from RF, he has to chose one of
// - requests
// - broadcasts

#include "commonTypes.h"

#define P2P_HEADER_BITS_MASK    0xE0

//bit 7 Broadcast or Directed Message
#define P2P_BROADCAST_MASK      0x80

#define P2P_BIT7_BROADCAST      0x80
#define P2P_BIT7_DIRECTED       0x00
//bit 6 msgAck or ReqResp
#define P2P_PKT_TYPE_MASK       0x40
#define P2P_PKT_TYPE_MSQ_ACK    0x40
#define P2P_MSGACK_NEG         ~0x40

//if bit 7 is Directed :
#define P2P_BIT6_MSGACK         0x40
#define P2P_BIT6_REQRESP        0x00

//bit 5 msg or ack
#define P2P_REQUEST_MASK        0x20
#define P2P_MESSAGE_MASK        0x20
#define P2P_ACK_NEG            ~0x20

 //if bit 6 is MSGACK:
#define P2P_MESSAGE             0x20
#define P2P_ACK                 0x00
 //if bit 6 is REQRESP:
#define P2P_BIT5_MESSAGE        0x20
#define P2P_BIT5_ACK            0x00

//acknowledge transmission
BYTE p2p_send(BYTE payload_size);

BYTE rf_ping(BYTE Dest);

BYTE rf_rgb_set(BYTE Dest,RGBColor_t Color);


//------------ configuration functions ------------ 
void rf_set_retries(BYTE nb_retries);
void rf_set_ack_delay(uint16_t delay);
