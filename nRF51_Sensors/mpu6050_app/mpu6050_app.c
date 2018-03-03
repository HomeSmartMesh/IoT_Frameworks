

#include "mpu6050_app.h"
#include "mpu6050.h"
#include "mpu60x0_register_map.h"
#include "twi_master.h"

#ifdef NRF_LOG_USES_RTT
#include "SEGGER_RTT.h"
#define DEBUG_PRINTF(...)           SEGGER_RTT_printf(0, __VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif

void mpu_dump_regs()
{
    bool res;
    uint8_t data_reg;
    res = mpu6050_register_read(MPU_REG_PWR_MGMT_1,&data_reg,1);
    DEBUG_PRINTF("MPU_REG_PWR_MGMT_1(%d) = 0x%02X\r\n",res,data_reg);
    res = mpu6050_register_read(MPU_REG_PWR_MGMT_2,&data_reg,1);
    DEBUG_PRINTF("MPU_REG_PWR_MGMT_2(%d) = 0x%02X\r\n",res,data_reg);
}

void mpu_start()
{
    bool res_twi = twi_master_init();
    DEBUG_PRINTF("twi_master_init(%d)\r\n",res_twi);

    bool res = mpu6050_init(0x69);//0x69
    DEBUG_PRINTF("mpu @ 0x69 => (%d)\r\n",res);

    mpu_dump_regs();

    //config = 6 ; Bandwidth : Accel 5 Hz, Gyro 5 Hz
    mpu6050_register_write(MPU_REG_CONFIG,6);
    //FS_SEL = 0 => 250°/s (lowest)
    mpu6050_register_write(MPU_REG_GYRO_CONFIG,0); // same as reset value
    //AFS_SEL = 0 => +- 2g (lowest)
    mpu6050_register_write(MPU_REG_ACCEL_CONFIG,0); // same as reset value
    
    //wakeup
    //8 : TEMP_DIS => disable temperature
    mpu6050_register_write(MPU_REG_PWR_MGMT_1,0x08);//8 : TEMP_DIS - awake
    //channels standby modes 0x38 Accel  ; 0x07 Gyro
    mpu6050_register_write(MPU_REG_PWR_MGMT_2,0x07);//0x07 Gyro in standby

}

void mpu_sleep()
{
    mpu6050_register_write(MPU_REG_PWR_MGMT_1,0x48);//0x48 => sleep
}

void mpu_get_accell(int8_t *x,int8_t *y,int8_t *z)
{
    uint8_t accell[5];
    //XH,XL ; YH,YL ; ZH,ZL
    mpu6050_register_read(MPU_REG_ACCEL_XOUT_H,accell,5);//need only high
    //DEBUG_PRINTF("0x%02X 0x%02X ; 0x%02X 0x%02X ; 0x%02X 0x%02X\r\n",
    //                accell[0],accell[1],accell[2],
    //                accell[3],accell[4],accell[5]);
    *x = accell[0];
    *y = accell[2];
    *z = accell[4];
}