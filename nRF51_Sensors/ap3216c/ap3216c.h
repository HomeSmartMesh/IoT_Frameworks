#ifndef AP3216C_H
#define AP3216C_H

#include <stdint.h>
#include <stdbool.h>

bool ap_reset();

bool ap_measure_light();
bool ap_get_light(uint8_t *data);

#endif /*AP3216C_H*/