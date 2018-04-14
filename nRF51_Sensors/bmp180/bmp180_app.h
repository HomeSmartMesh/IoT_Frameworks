#ifndef BMP180_APP_H
#define BMP180_APP_H

#include <stdint.h>

void bmp_dump_regs();//TODO

bool bmp_init();

void bmp_measure();//TODO

void bmp_get_temperature(uint8_t *data);

void bmp_get_pressure(uint8_t *data);

#endif /*BMP180_APP_H*/