/*
****************************************************************************
* Copyright (C) 2014 - 2015 Bosch Sensortec GmbH
*
* bmp180_support.c
* Date: 2015/07/16
* Revision: 1.0.5 $
*
* Usage: Sensor Driver support file for BMP180
*
****************************************************************************
* License:
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
*   Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the distribution.
*
*   Neither the name of the copyright holder nor the names of the
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
* OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*
* The information provided is believed to be accurate and reliable.
* The copyright holder assumes no responsibility
* for the consequences of use
* of such information nor for any infringement of patents or
* other rights of third parties which may result from its use.
* No license is granted by implication or otherwise under any patent or
* patent rights of the copyright holder.
**************************************************************************/
/*---------------------------------------------------------------------------*/
/* Includes*/
/*---------------------------------------------------------------------------*/
#include "bmp180.h"

#include "twi_master.h"
#include "nrf_delay.h"


/*----------------------------------------------------------------------------
  * The following functions are used for reading and writing of
  * sensor data using I2C or SPI communication
----------------------------------------------------------------------------*/
/*	\Brief: The function is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
s8 BMP180_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*	\Brief: The function is used as SPI bus write
 *	\Return : Status of the SPI write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
s8 BMP180_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*
 * \Brief: I2C init routine
*/
s8 I2C_routine(void);

/********************End of I2C function declarations***********************/
/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BMP180_delay_msek(u32 msek);
/* This function is an example for reading sensor data
 *	\param: None
 *	\return: communication result
 */
s32 bmp180_data_readout_template(void);
/*----------------------------------------------------------------------------
 struct bmp180_t parameters can be accessed by using bmp180
 *	bmp180_t having the following parameters
 *	Bus write function pointer: BMP180_WR_FUNC_PTR
 *	Bus read function pointer: BMP180_RD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *	I2C address: dev_addr
 *	Chip id of the sensor: chip_id
 *	Calibration parameters
 ---------------------------------------------------------------------------*/
struct bmp180_t bmp180;
/*----------------------------------------------------------------------------*/
/* This function is an example for reading sensor data
 *	\param: None
 *	\return: communication result
 */
s32 bmp180_data_readout_template(void)
{
 /* result of communication results*/
	s32 com_rslt = E_BMP_COMM_RES;
	u16 v_uncomp_temp_u16 = BMP180_INIT_VALUE;
	u32 v_uncomp_press_u32 = BMP180_INIT_VALUE;
/*********************** START INITIALIZATION ************************/
    /**************Call the I2C init routine ***************/
	I2C_routine();
/*--------------------------------------------------------------------------
 *  This function used to assign the value/reference of
 *	the following parameters
 *	I2C address
 *	Bus Write
 *	Bus read
 *	Chip id
 *	Calibration values
-------------------------------------------------------------------------*/
	com_rslt = bmp180_init(&bmp180);

/************************* END INITIALIZATION *************************/
/*------------------------------------------------------------------*
************************* START CALIPRATION ********
*---------------------------------------------------------------------*/
	/*  This function used to read the calibration values of following
	 *	these values are used to calculate the true pressure and temperature
	 *	Parameter		MSB		LSB		bit
	 *		AC1			0xAA	0xAB	0 to 7
	 *		AC2			0xAC	0xAD	0 to 7
	 *		AC3			0xAE	0xAF	0 to 7
	 *		AC4			0xB0	0xB1	0 to 7
	 *		AC5			0xB2	0xB3	0 to 7
	 *		AC6			0xB4	0xB5	0 to 7
	 *		B1			0xB6	0xB7	0 to 7
	 *		B2			0xB8	0xB9	0 to 7
	 *		MB			0xBA	0xBB	0 to 7
	 *		MC			0xBC	0xBD	0 to 7
	 *		MD			0xBE	0xBF	0 to 7*/
	com_rslt += bmp180_get_calib_param();
/*------------------------------------------------------------------*
************************* END CALIPRATION ********
*---------------------------------------------------------------------*/
/************************* START READ UNCOMPENSATED TEMPERATURE AND PRESSURE********/


	/*	This API is used to read the
	*	uncompensated temperature(ut) value*/
	v_uncomp_temp_u16 = bmp180_get_uncomp_temperature();

	/*	This API is used to read the
	*	uncompensated pressure(ut) value*/
	v_uncomp_press_u32 = bmp180_get_uncomp_pressure();

/************************* END READ UNCOMPENSATED TEMPERATURE AND PRESSURE********/


/************************* START READ TRUE TEMPERATURE AND PRESSURE********/
/****************************************************************************
 *	This API is used to read the
 *	true temperature(t) value input
 *	parameter as uncompensated temperature(ut)
 *
 ***************************************************************************/
	com_rslt += bmp180_get_temperature(v_uncomp_temp_u16);

/****************************************************************************
 *	This API is used to read the
 *	true pressure(p) value
 *	input parameter as uncompensated pressure(up)
 *
 ***************************************************************************/
	com_rslt += bmp180_get_pressure(v_uncomp_press_u32);

/************************* END READ TRUE TEMPERATURE AND PRESSURE********/
return com_rslt;
}

/*--------------------------------------------------------------------------*
*	The following function is used to map the I2C bus read, write, delay and
*	device address with global structure bmp180_t
*-------------------------------------------------------------------------*/
s8 I2C_routine(void) {
/*--------------------------------------------------------------------------*
 *  By using bmp180 the following structure parameter can be accessed
 *	Bus write function pointer: BMP180_WR_FUNC_PTR
 *	Bus read function pointer: BMP180_RD_FUNC_PTR
 *	Delay function pointer: delay_msec
 *	I2C address: dev_addr
 *--------------------------------------------------------------------------*/
	bmp180.bus_write = BMP180_I2C_bus_write;
	bmp180.bus_read = BMP180_I2C_bus_read;
	bmp180.dev_addr = BMP180_I2C_ADDR;
	bmp180.delay_msec = BMP180_delay_msek;

	return BMP180_INIT_VALUE;
}

/************** I2C buffer length ******/

#define	I2C_BUFFER_LEN 8
#define I2C0 5
/*-------------------------------------------------------------------*
*	This is a sample code for read and write the data by using I2C
*	Configure the below code to your I2C driver
*	The device address is defined in the bmp180.c file
*-----------------------------------------------------------------------*/
/*	\Brief: The function is used as I2C bus write
 *	\Return : Status of the I2C write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
s8 BMP180_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BMP180_INIT_VALUE;

    bool res = twi_master_transfer(dev_addr, &reg_addr, 1, TWI_DONT_ISSUE_STOP);
	if(res)
	{
		res = twi_master_transfer(dev_addr, reg_data, cnt, TWI_ISSUE_STOP);
	}
	if(!res)
	{
		iError = -C_BMP180_ONE_U8X;
	}
	return (s8)iError;
}

 /*	\Brief: The function is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
s8 BMP180_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
	s32 iError = BMP180_INIT_VALUE;

    bool res = twi_master_transfer(dev_addr, &reg_addr, 1, TWI_DONT_ISSUE_STOP);
	if(res)
	{
		res = twi_master_transfer(dev_addr|TWI_READ_BIT, reg_data, cnt, TWI_ISSUE_STOP);
	}
	if(!res)
	{
		iError = -C_BMP180_ONE_U8X;
	}

	return (s8)iError;
}
/*	Brief : The delay routine
 *	\param : delay in ms
*/
void BMP180_delay_msek(u32 msek)
{
	nrf_delay_ms(msek);
}


void bmp_dump_regs()
{

}

//calib params are maintained in RAM as long as no power off RAM loss sleep is called
void bmp_init()
{
	I2C_routine();
	s32 com_rslt = bmp180_init(&bmp180);
	s32 com_rslt += bmp180_get_calib_param();
}

//should here trigger the measures from BMP180_CTRL_MEAS_REG
//ideally both temperature and pressure together
void bmp_measure()
{

}

//this function is currently triggering an independent measure for the temperature
//Return the temperature in steps of 0.1 deg Celsius
void bmp_get_temperature(uint8_t *data)
{
	u16 v_uncomp_temp_u16 = bmp180_get_uncomp_temperature();
	sint16_t temperature = bmp180_get_temperature(v_uncomp_temp_u16);
}

//this function is currently triggering an independent measure for the pressure
//Return the value of pressure in steps of 1.0 Pa
void bmp_get_pressure(uint8_t *data)
{
	u32 v_uncomp_press_u32 = bmp180_get_uncomp_pressure();
	sint32_t pressure = bmp180_get_pressure(v_uncomp_press_u32);
}
