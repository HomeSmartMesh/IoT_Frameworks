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


#define P2P_BROADCAST_MASK      0x80
#define P2P_PKT_TYPE_MASK       0x60
#define P2P_PKT_TYPE_MSQ_ACK    0x60
#define P2P_REQUEST_MASK        0x40
#define P2P_MESSAGE_MASK        0x40



//acknowledge transmission
BYTE p2p_send_info(p2p_info_t *p_info);

//acknowledge transmission
BYTE p2p_send(BYTE *payload,BYTE payload_size,BYTE nb_retries);

BYTE p2p_send_message(rf_message_t *p_info);
