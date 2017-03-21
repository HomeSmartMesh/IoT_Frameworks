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

#include "rf_protocol.h"

#include "clock_led.h"
#include "uart.h"

#include "nRF_Configuration.h"

BYTE p2p_message[31];//size not included

#define p2p_msg_pl (p2p_message+3)

BYTE p2p_ack = 0;
BYTE p2p_expected_Pid = 0;
BYTE p2p_nb_retries = 20;


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

void send_ack(BYTE *rxData)
{
	p2p_message[0] = rxData[0];
    p2p_message[0] &= ~P2P_BROADCAST_MASK;//not a broadcast
    p2p_message[0] |= P2P_PKT_TYPE_MSQ_ACK;//it's a Msg Ack type not Req Resp
    p2p_message[0] &= ~P2P_MESSAGE_MASK;//it is rather an Ack
	p2p_message[1] = NodeId;//Source
	p2p_message[2] = rxData[1];//Dest is the Rx Source
	nRF_Transmit_Wait_Rx(p2p_message,3);
    //printf_ln("sent Ack");

}

//User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
    BYTE isReturned = 0;
    //printf("msg_received: ");printf_tab(rxData,rx_DataSize);printf_eol();
    if(rxData[2] != NodeId)
    {
        return;//as this packet is not directed to us
    }
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
                //here should check a generic CRC
                send_ack(rxData);
                #if P2P_MESSAGE_CALLBACK == 1
                rxData[0]&= 0x01F;// clear bit7, bit6, bit5 and keep id
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
            if((p2p_expected_Pid & 0x1F) == (rxData[0] & 0x1F) && (rxData[2] == NodeId)  )
            {
                p2p_ack = 1;//notify the re-transmitter
                //printf_ln("received_ack");
            }
        }
    }
}

BYTE p2p_send_check_ack(BYTE *payload,BYTE payload_size)
{
    p2p_ack = 0;
    p2p_expected_Pid = payload[0];//Pid
	nRF_Transmit_Wait_Rx(payload,payload_size);
    //printf("sent ");printf_tab(payload,payload_size);printf_eol();
	delay_ms(190);// >>> Timeout important, might depend on Nb briges, and on the ReqResp or just MsgAck
    return p2p_ack;
}

BYTE p2p_send(BYTE *payload,BYTE payload_size,BYTE max_retries)
{
	BYTE success = 0;
	BYTE i = 0;
	do
	{
		success = p2p_send_check_ack(payload,payload_size);//1 on success, 0 on fail
		i++;
	}while((i<max_retries) && (success == 0) );
	return i;//nb_retries
}


BYTE p2p_send_ping(rf_message_t *p_msg)
{
    //This is a message
    p2p_message[0] = P2P_BIT7_DIRECTED | P2P_BIT6_MSGACK | P2P_BIT5_MESSAGE | rf_pid_ping;     //Pid
    p2p_message[1] = NodeId;                    //source
    p2p_message[2] = p_msg->dest;               //dest
    return p2p_send(p2p_message,3,p_msg->nb_retries);
}

//Message, requires acknowledge, no response
BYTE rf_ping(BYTE Dest)
{
    p2p_message[0] = P2P_BIT7_DIRECTED | P2P_BIT6_MSGACK | P2P_BIT5_MESSAGE | rf_pid_ping;     //Pid
    p2p_message[1] = NodeId;                   //source
    p2p_message[2] = Dest;                     //dest
    return p2p_send(p2p_message,3,p2p_nb_retries);
}

//Message, requires acknowledge, no response
BYTE rf_rgb_set(BYTE Dest,RGBColor_t Color)
{
    p2p_message[0] = P2P_BIT7_DIRECTED | P2P_BIT6_MSGACK | P2P_BIT5_MESSAGE | rf_pid_rgb;     //Pid
    p2p_message[1] = NodeId;
    p2p_message[2] = Dest;
    p2p_message[3] = Color.R;
    p2p_message[4] = Color.G;
    p2p_message[5] = Color.B;
    p2p_message[6] = p2p_message[1] ^ p2p_message[2] ^ p2p_message[3] ^ p2p_message[4]^ p2p_message[5];
    
    return p2p_send(p2p_message,7,p2p_nb_retries);
}

void rf_set_retries(BYTE nb_retries)
{
    p2p_nb_retries = nb_retries;
}
