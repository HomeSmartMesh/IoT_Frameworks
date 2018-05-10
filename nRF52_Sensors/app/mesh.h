#ifndef __MESH_H__
#define __MESH_H__


#include <stdint.h>


uint32_t mesh_init();

void mesh_wait_tx();

void mesh_tx_reset();
void mesh_tx_alive();

void mesh_tx_data(uint8_t pid,uint8_t * data,uint8_t size);

uint32_t mesh_tx_light_on();
uint32_t mesh_tx_light_off();
uint32_t mesh_tx_button(uint8_t state);

#endif /*__MESH_H__*/
