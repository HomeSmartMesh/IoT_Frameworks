/** @file WS2812B.h
 *
 * @author Wassim FILALI
 * Inferred from the Datasheet
 * 
 *
 * @compiler gcc for STM32
 *
 *
 * started on local project                 on 10.10.2015
 * this file was created for refactoring    on 03.01.2016
 * takeover for STM32                       on 02.12.2017
 * $Date:
 * $Revision:
 *
 */

#ifndef __talk__
#define __talk__

#include "mbed.h"
#include "rfmesh.h"
#include "ws2812B.h"

typedef struct
{
  uint8_t id;
  uint8_t status;
}partner_t;


namespace talk
{
	uint8_t const offline   		= 0x0;
	uint8_t const idle 				= 0x1;
	uint8_t const speak 			= 0x2;
	uint8_t const listen 			= 0x3;
	uint8_t const requesting		= 0x4;
	uint8_t const requested_from 	= 0x5;

	uint8_t const online   		    = 0xF;
}


class talk_node
{
public:
    talk_node(RfMesh* p_mesh,Serial* p_ser,ws2812B *p_led,uint8_t l_id);
    void self_acted();
    void other_acted();
    void broadcast(uint8_t *data,uint8_t size);
    void message(uint8_t *data,uint8_t size);

    void add_node(uint8_t wake_id);
    void get_nodes(uint8_t *data);

	uint8_t status;
	uint8_t master;

private:
    RfMesh* mesh;
    Serial* ser;
    ws2812B* led;
    uint8_t node_id;
    
    void set_color(uint8_t r,uint8_t g,uint8_t b);
    void set_red();
    void set_green();
    void set_blue();
    void set_yellow();
    void set_orange();
    void apply_color_status();

};

#endif /*__talk__*/
