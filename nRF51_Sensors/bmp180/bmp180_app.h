#ifndef BMP180_APP_H
#define BMP180_APP_H

#include <stdint.h>

void bmp_dump_regs();

void bmp_init();

void bmp_measure();

void bmp_get_temperature(uint8_t *data);

void bmp_get_pressure(uint8_t *data);

#endif /*BMP180_APP_H*/