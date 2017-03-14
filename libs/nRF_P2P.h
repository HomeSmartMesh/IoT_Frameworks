/** @file nRF_P2P.h
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 12.03.2017
 * $Revision:
 *
 */
//When using the Higher level p2p RF Functions,
//userRxCallBack() must not be instantiated, as it is handled here
//if the user needs to receive from RF, he has to chose one of
// - requests
// - broadcasts

#include "commonTypes.h"

typedef struct 
{
    BYTE source;
    BYTE dest;
    BYTE nb_retries;
    BYTE Pid;
    BYTE *payload;
    BYTE payload_size;
}p2p_info_t;

typedef struct 
{
    BYTE *payload;
    BYTE payload_size;
    BYTE source;
    BYTE dest;
    BYTE nb_retries;
    BYTE Pid;
}rf_message_t;

//bit 7 Broadcast or Directed Message
#define P2P_BROADCAST_MASK      0x80
//bit 6 msgAck or ReqResp
#define P2P_PKT_TYPE_MASK       0x40
#define P2P_PKT_TYPE_MSQ_ACK    0x40
#define P2P_MSGACK_NEG         ~0x40
//bit 5 msg or ack
#define P2P_REQUEST_MASK        0x20
#define P2P_MESSAGE_MASK        0x20
#define P2P_ACK_NEG            ~0x20


//acknowledge transmission
BYTE p2p_send(BYTE *payload,BYTE payload_size,BYTE nb_retries);

BYTE p2p_send_ping(rf_message_t *p_msg);
