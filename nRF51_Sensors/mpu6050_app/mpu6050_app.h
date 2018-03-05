#ifndef MPU6050_APP_H
#define MPU6050_APP_H

#include <stdint.h>

void mpu_dump_regs();

void mpu_start();//also wakes up

void mpu_wakeup();

void mpu_sleep();

void mpu_get_accell_xyz(int8_t *x,int8_t *y,int8_t *z);
void mpu_get_accell_data(uint8_t *data);

#endif /*MPU6050_APP_H*/