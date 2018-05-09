#ifndef __MESH_H__
#define __MESH_H__


#include <stdint.h>


uint32_t mesh_init();

uint32_t mesh_tx_button(uint8_t state);

uint32_t mesh_tx_light_on();

uint32_t mesh_tx_light_off();

void mesh_wait_tx();

#endif /*__MESH_H__*/
