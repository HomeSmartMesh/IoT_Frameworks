/** @file nRF_P2P.c
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

#include "nRF_P2P.h"
#include "nRF_Tx.h"

BYTE p2p_ack = 0;
BYTE p2p_expected_Pid = 0;

extern BYTE NodeId;

#if P2P_BROADCAST_CALLBACK == 1
void rf_Broadcast_CallBack(BYTE *rxData,BYTE rx_DataSize);
#endif

#if P2P_REQUEST_CALLBACK == 1
void rf_Request_CallBack(BYTE *rxData,BYTE rx_DataSize);
#endif

#if P2P_RESPONSE_CALLBACK == 1
void rf_Response_CallBack(BYTE *rxData,BYTE rx_DataSize);
#endif

#if P2P_MESSAGE_CALLBACK == 1
void rf_Message_CallBack(BYTE *rxData,BYTE rx_DataSize);
#endif

//User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
    BYTE isReturned = 0;
    //check if it's a Broadcast or a P2P
    if((rxData[0] & P2P_BROADCAST_MASK) == P2P_BROADCAST_MASK)
    {
        #if P2P_BROADCAST_CALLBACK == 1
        rf_Broadcast_CallBack(rxData,rx_DataSize);
        #endif
    }
    else//it's a P2P protocol
    {
        if((rxData[0] & P2P_PKT_TYPE_MASK) == P2P_PKT_TYPE_MSQ_ACK)
        {
            if((rxData[0] & P2P_MESSAGE_MASK) == P2P_MESSAGE_MASK)
            {
                #if P2P_MESSAGE_CALLBACK == 1
                rf_Message_CallBack(rxData,rx_DataSize);
                #endif
            }
            else//it's an acknowledge
            {
                isReturned = 1;
            }
        }
        else//it's Request / Response
        {
            if((rxData[0] & P2P_REQUEST_MASK) == P2P_REQUEST_MASK)
            {
                #if P2P_REQUEST_CALLBACK == 1
                rf_Request_CallBack(rxData,rx_DataSize);
                #endif
            }
            else
            {
                // => Response
                isReturned = 1;
                #if P2P_RESPONSE_CALLBACK == 1
                rf_Response_CallBack(rxData,rx_DataSize);
                #endif
            }
        }
        if(isReturned)
        {
            if(  (p2p_expected_Pid == rxData[0]) && (rxData[2] == NodeId)  )
            {
                p2p_ack = 1;//notify the re-transmitter
            }
        }
    }
}

BYTE p2p_send_info(p2p_info_t *p_info)
{
    p2p_ack = 0;
    p2p_expected_Pid = p_info->Pid;

}

BYTE p2p_send_check_ack(BYTE *payload,BYTE payload_size)
{
    p2p_ack = 0;
    p2p_expected_Pid = payload[0];//Pid
	nRF_Transmit_Wait_Rx(txData,payload_size);
	delay_ms(30);// >>> Timeout important, might depend on Nb briges, and on the ReqResp or just MsgAck
    return p2p_ack;
}

BYTE p2p_send(BYTE *payload,BYTE payload_size,BYTE nb_retries)
{
	BYTE success = 0;
	BYTE i = 0;
	do
	{
		success = p2p_send_check_ack(payload,payload_size);//1 on success, 0 on fail
		i++;
	}while((i<nb_retries) && (success == 0) );
	return success;
}
