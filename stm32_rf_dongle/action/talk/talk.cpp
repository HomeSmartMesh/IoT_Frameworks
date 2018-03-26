
#include "talk.h"

#include "protocol.h"

partner_t partners[10];

talk_node::talk_node(RfMesh* p_mesh,Serial* p_ser,ws2812B *p_led):
                        mesh(p_mesh),
                        ser(p_ser),
                        led(p_led)
{
    status = talk::idle;
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
	ser->printf("Self acted State:%u\r\n",status);
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

void talk_node::broadcast(uint8_t *data,uint8_t size)
{
    if(data[rf::ind::bcst_payload] == rf::talk::get_list)
    {
        //add_node(data[rf::ind::source])
        //TODO send partners[] to data[rf::ind::source]
        //mesh->send_msg();
    }
    else
    {
        ser->printf("Unhandled Talk Broadcast\n");
    }
}

void talk_node::message(uint8_t *data,uint8_t size)
{
    ser->printf("Rx Message\n");
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

