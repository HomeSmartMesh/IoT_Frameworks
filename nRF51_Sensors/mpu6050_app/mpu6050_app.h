#ifndef MPU6050_APP_H
#define MPU6050_APP_H

#include <stdint.h>

void mpu_dump_regs();

void mpu_start();

void mpu_sleep();

void mpu_get_accell(int8_t *x,int8_t *y,int8_t *z);


#endif /*MPU6050_APP_H*/