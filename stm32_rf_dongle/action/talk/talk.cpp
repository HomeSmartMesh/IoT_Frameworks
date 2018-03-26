
#include "talk.h"
#include "protocol.h"
#include "utils.h"

#define MAX_NB_NODES 10

partner_t speakers[MAX_NB_NODES];

void print_state(Serial *ps,uint8_t st)
{
    switch(st)
    {
        case talk::online           : ps->printf("online"); break;
        case talk::idle             : ps->printf("idle"); break;
        case talk::speak            : ps->printf("speak"); break;
        case talk::listen           : ps->printf("listen"); break;
        case talk::requesting       : ps->printf("req"); break;
        case talk::requested_from   : ps->printf("reqfrm"); break;
    }
}

void print_speakers(Serial *ps,uint8_t *data)
{
    ps->printf("Speakers: ");
    for(int i=0;i<MAX_NB_NODES;i++)
    {
        if(data[2*i+1] != talk::offline)
        {
            ps->printf("(%u:",data[2*i]);
            print_state(ps,data[2*i+1]);
            ps->printf(") ");
        }
    }
    ps->printf("\r\n");
}

talk_node::talk_node(RfMesh* p_mesh,Serial* p_ser,ws2812B *p_led,uint8_t l_id):
                        mesh(p_mesh),
                        ser(p_ser),
                        led(p_led)
{
    node_id = l_id;
    status = talk::offline;
    master = 0;

    speakers[0].id = node_id;
    speakers[0].status = talk::online;

    for(int i=1;i<MAX_NB_NODES;i++)
    {
        speakers[i].id = 0;
        speakers[i].status = talk::offline;
    }
}

void talk_node::self_acted()
{
	switch(status)
	{
		case talk::idle : 
			status = talk::speak;
			break;
		case talk::speak : 
			status = talk::idle;
			break;
		case talk::listen : 
			status = talk::requesting;
			break;
		case talk::requesting : 
			status = talk::listen;
			break;
		case talk::requested_from : 
			status = talk::listen;
			break;
	}
	apply_color_status();
	ser->printf("Self acted State:");print_state(ser,status);ser->printf("\n");
	wait(2);
}

void talk_node::other_acted()
{
	switch(status)
	{
		case talk::idle : 
			status = talk::listen;
			break;
		case talk::speak :
			status = talk::requested_from;
			break;
		case talk::listen :
			status = talk::listen;
			break;
		case talk::requesting : 
			status = talk::speak;
			break;
		case talk::requested_from :
			status = talk::requested_from;
			break;
	}
	apply_color_status();
	ser->printf("Other acted State:%u\r\n",status);
	wait(2);
}

void talk_node::add_node(uint8_t wake_id)
{
    bool registred = false;
    for(int i=0;(i<MAX_NB_NODES)&&(!registred);i++)
    {
        if(speakers[i].id == wake_id)
        {
            speakers[i].status = talk::online;
            registred = true;
        }
    }
    if(!registred)//then take the first offline
    {
        for(int i=0;(i<MAX_NB_NODES)&&(!registred);i++)
        {
            if(speakers[i].status == talk::offline)
            {
                speakers[i].id = wake_id;
                speakers[i].status = talk::online;
                registred = true;
            }
        }
    }
}

void talk_node::get_nodes(uint8_t *data)
{
    for(uint8_t i=0;i<MAX_NB_NODES;i++)
    {
        data[i*2] = speakers[i].id;
        data[i*2+1] = speakers[i].status;
    }
}

void talk_node::broadcast(uint8_t *data,uint8_t size)
{
    if(data[rf::ind::bcst_payload] == rf::talk::wakeup)
    {
        add_node(data[rf::ind::source]);
        uint8_t buffer[32];
        buffer[0] = 5 + 1 + 20;//size
        buffer[1] = 0x71;//Message with ack, ttl = 1
        buffer[2] = rf::pid::talk;
        buffer[3] = node_id;                //src
        buffer[4] = data[rf::ind::source];  //dest
        buffer[5] = rf::talk::list;  //sub-id
        get_nodes(buffer+rf::ind::p2p_payload+1);
        mesh->send_msg(buffer);
        ser->printf("Rx bcast : wake> sent:");
        print_speakers(ser,buffer+rf::ind::p2p_payload+1);
    }
    else
    {
        ser->printf("Unhandled Talk Broadcast\n");
    }
}

void talk_node::message(uint8_t *data,uint8_t size)
{
    if(data[rf::ind::p2p_payload] == rf::talk::list)//sub-id check
    {
        ser->printf("Rx Talk Msg list>");
        print_speakers(ser,data+rf::ind::p2p_payload+1);
        status = talk::online;
    }
    else
    {
        ser->printf("Rx Talk Msg Unknown pid 0x%02X\n",data[rf::ind::p2p_payload]);
    }
}

void talk_node::set_color(uint8_t r,uint8_t g,uint8_t b)
{
	uint8_t lr,lg,lb;
	led->get(lr,lg,lb);
	if( (lr != r) || (lg != g) || (lb != b) )
	{
		led->set(r,g,b);//Red
	}
}

void talk_node::set_red()		{	set_color(0x0F,0,0);}
void talk_node::set_green()	    {	set_color(0,0x0F,0);}
void talk_node::set_blue()		{	set_color(0,0,0x0F);}
void talk_node::set_yellow()	{	set_color(0x0F,0x0F,0);}
void talk_node::set_orange()	{	set_color(0x0F,0x05,0);}


void talk_node::apply_color_status()
{
	switch(status)
	{
		case talk::idle : 
			set_blue();
			break;
		case talk::speak : 
			set_green();
			break;
		case talk::listen : 
			set_red();
			break;
		case talk::requesting : 
			set_orange();
			break;
		case talk::requested_from : 
			set_yellow();
			break;
	}
}

