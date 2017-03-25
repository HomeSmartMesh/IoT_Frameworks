/** @file nRF_P2P.c
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 12.03.2017
          25.03.2017 - renamed from nRF_P2P.c to rf_messages.c
 * $Revision:
 *
 */

#include "rf_messages.h"
#include "nRF_Tx.h"

#include "rf_protocol.h"
#include "WS2812B.h"

#include "clock_led.h"
#include "uart.h"

#include "nRF_Configuration.h"

#include "cmdutils.h"

BYTE p2p_message[31];//size not included

BYTE * p2p_payload = p2p_message + rfi_header_size;

#define p2p_msg_pl (p2p_message+3)

BYTE p2p_ack = 0;
BYTE p2p_expected_Pid = 0;
BYTE p2p_nb_retries = 3;


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
void rf_Message_CallBack(BYTE* rxHeader,BYTE *rxPayload,BYTE rx_PayloadSize);
#endif

void send_ack(BYTE *rxData)
{
	p2p_message[rfi_pid] = rxData[rfi_pid] & ~P2P_HEADER_BITS_MASK;//clear all of the Header bits
    p2p_message[rfi_pid] |=  P2P_BIT7_DIRECTED | P2P_BIT6_MSGACK | P2P_BIT5_ACK;
	p2p_message[rfi_src] = NodeId;//Source
	p2p_message[rfi_dst] = rxData[rfi_src];//Dest is the Rx Source
    p2p_message[rfi_size] = rfi_header_size;//Empty payload
    crc_set(p2p_message);
    BYTE rf_msg_size = rfi_header_size + crc_size;
	nRF_Transmit_Wait_Rx(p2p_message,rf_msg_size);
    //printf("sent Ack : ");printf_tab(p2p_message,rf_msg_size);printf_eol();

}

//User Rx CallBack
void userRxCallBack(BYTE *rxData,BYTE rx_DataSize)
{
    BYTE isReturned = 0;

    //------------------------ Check the CRC --------------------------------
    if(!crc_check(rxData))
    {
        printf("rx crc Fail : ");printf_tab(rxData,rx_DataSize+2);printf_eol();
        return;
    }
    //------------------------ Check the destination ------------------------
    //printf("msg_received: ");printf_tab(rxData,rx_DataSize);printf_eol();
    if(rxData[rfi_dst] != NodeId)
    {
        return;//as this packet is not directed to us
    }
    //check if it's a Broadcast or a P2P
    if((rxData[rfi_pid] & P2P_BROADCAST_MASK) == P2P_BROADCAST_MASK)
    {
        #if P2P_BROADCAST_CALLBACK == 1
        rf_Broadcast_CallBack(rxData,rx_DataSize);
        #endif
    }
    else//it's a P2P protocol
    {
        if((rxData[rfi_pid] & P2P_PKT_TYPE_MASK) == P2P_PKT_TYPE_MSQ_ACK)
        {
            if((rxData[rfi_pid] & P2P_MESSAGE_MASK) == P2P_MESSAGE_MASK)
            {
                send_ack(rxData);
                //printf("rx message:");printf_tab(rxData,rx_DataSize+crc_size);printf_eol();
                #if P2P_MESSAGE_CALLBACK == 1
                rxData[rfi_pid]&= 0x01F;// clear bit7, bit6, bit5 and keep id
                rf_Message_CallBack(rxData,rxData+rfi_header_size,rx_DataSize-rfi_header_size);
                #endif
            }
            else//it's an acknowledge
            {
                isReturned = 1;
            }
        }
        else//it's Request / Response
        {
            if((rxData[rfi_pid] & P2P_REQUEST_MASK) == P2P_REQUEST_MASK)
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
            if((p2p_expected_Pid & 0x1F) == (rxData[rfi_pid] & 0x1F) && (rxData[rfi_dst] == NodeId)  )
            {
                p2p_ack = 1;//notify the re-transmitter
                //printf_ln("received_ack");
            }
        }
    }
}

BYTE p2p_send_check_ack(BYTE rf_msg_size)
{
    p2p_ack = 0;
    p2p_expected_Pid = p2p_message[rfi_pid];//Pid
	nRF_Transmit_Wait_Rx(p2p_message,rf_msg_size);
    //printf("sent ");printf_tab(p2p_message,rf_msg_size);printf_eol();
	delay_ms(190);// >>> Timeout important, might depend on Nb briges, and on the ReqResp or just MsgAck
    return p2p_ack;
}

BYTE p2p_send(BYTE payload_size)
{
	BYTE success = 0;
	BYTE i = 0;
    BYTE size = payload_size + rfi_header_size;//this size does not include the crc
    p2p_message[rfi_size] = size;//complete the message with the correct size first
    //printf("tx message:");printf_tab(p2p_message,6);printf_eol();
    //the crc will be set after the header and payload
    crc_set(p2p_message);
    BYTE rf_msg_size = size + crc_size ;
	do
	{
		success = p2p_send_check_ack(rf_msg_size);//1 on success, 0 on fail
		i++;
	}while((i<p2p_nb_retries) && (success == 0) );
	return i;//nb_retries
}


//Message, requires acknowledge, not a response
BYTE rf_ping(BYTE Dest)
{
    p2p_message[rfi_pid] = P2P_BIT7_DIRECTED | P2P_BIT6_MSGACK | P2P_BIT5_MESSAGE | rf_pid_ping;     //Pid
    p2p_message[rfi_src] = NodeId;                   //source
    p2p_message[rfi_dst] = Dest;                     //dest
    return p2p_send(0);
}

//Message, requires acknowledge, no response
BYTE rf_rgb_set(BYTE Dest,RGBColor_t Color)
{
    p2p_message[rfi_pid] = P2P_BIT7_DIRECTED | P2P_BIT6_MSGACK | P2P_BIT5_MESSAGE | rf_pid_rgb;     //Pid
    p2p_message[rfi_src] = NodeId;
    p2p_message[rfi_dst] = Dest;
    BYTE PayloadSize;
    rgb_encode_rf(Color,p2p_message+rfi_payload_offset,&PayloadSize);
    
    return p2p_send(PayloadSize);
}

void rf_set_retries(BYTE nb_retries)
{
    p2p_nb_retries = nb_retries;
}
