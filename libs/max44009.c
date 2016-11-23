/** @file max44009.c
 *
 * @author Wassim FILALI
 *
 * @compiler IAR STM8
 *
 *
 * $Date: 29.10.2016 - creation out of refactoring
 * $Revision: 1 
 *
*/

#include "i2c_stm8x.h"
#include "commonTypes.h"

#include "clock_led.h"
#include "uart.h"

BYTE max44009_read_reg(BYTE address)
{
    BYTE result;

    I2C_Write(0x4A, &address,1);
    delay_100us();
    I2C_Read(0x4A, &result,1); 
    delay_100us();//wait to complete before writing into unallocated variable
    
    return result;
}

uint16_t max44009_read_light()
{
    BYTE sensorData[2];
    sensorData[0] = max44009_read_reg(0x03);
    sensorData[1] = max44009_read_reg(0x04);
    uint16_t Val = sensorData[0];
    Val <<= 4;//shift to make place for the 4 LSB
    Val = Val + (0x0F & sensorData[1]);
	return Val;
}

