#include "ap3216c.h"

#include "twi_master.h"
#include "nrf_delay.h"

uint8_t dynamic_range = 1;

#define AP_DEVICE_WRITE_ADD 0x3C

bool ap_write_reg(uint8_t reg_addr, uint8_t reg_val)
{
    uint8_t w2_data[2];

    w2_data[0] = reg_addr;
    w2_data[1] = reg_val;

    bool res = twi_master_transfer(AP_DEVICE_WRITE_ADD, w2_data, 2, TWI_ISSUE_STOP);

	return res;
}

bool ap_read_data(uint8_t reg_addr, uint8_t *reg_data, uint8_t count)
{
    bool res  = twi_master_transfer(AP_DEVICE_WRITE_ADD, &reg_addr, 1, TWI_DONT_ISSUE_STOP);
		 res &= twi_master_transfer(AP_DEVICE_WRITE_ADD|TWI_READ_BIT, reg_data, count, TWI_ISSUE_STOP);

	return res;
}

void ap_dump_regs()
{

}

bool ap_test()
{
	bool res;
	uint8_t reg_data[16];
	reg_data[0] = 0;//write crash test
	res = ap_read_data( 0x00, reg_data, 16);

	res &= ap_read_data( 0x19, reg_data, 5);

	res &= ap_write_reg(0x10,0x30);//11 in bits 5:4 of ALS Configuration

	reg_data[0] = 0;
	res &= ap_read_data( 0x10, reg_data, 1);
	
	return res;
}

bool ap_reset()
{
	const uint8_t AP_REG_SYS_CONF 		= 0x00;
	bool res;
	res = ap_write_reg(AP_REG_SYS_CONF,0x04);//SW reset
	nrf_delay_ms(11);//datasheet require 10 ms

	return res;
}

uint32_t last_measure = 20661;

bool ap_measure_light()
{
	const uint8_t AP_ALS_CONF 		= 0x10;
	if(last_measure < 300)
	{
		dynamic_range = 4;
		ap_write_reg(AP_ALS_CONF,0x30);
	}
	else if(last_measure < 1200)
	{
		dynamic_range = 3;
		ap_write_reg(AP_ALS_CONF,0x20);
	}
	else if(last_measure < 5100)
	{
		dynamic_range = 2;
		ap_write_reg(AP_ALS_CONF,0x10);
	}
	else
	{
		dynamic_range = 1;
		ap_write_reg(AP_ALS_CONF,0x00);
	}
	const uint8_t AP_REG_SYS_CONF 		= 0x00;
	return ap_write_reg(AP_REG_SYS_CONF,0x05);//ALS function once - power down automatically after 2.5 periods (250 ms)
}

bool ap_get_light(uint8_t *data)
{
	const uint8_t AP_REG_ALS_DATA_LOW	= 0x0C;
	//data[0] <= ALS Data Low
	//data[1] <= ALS Data High
	bool res = ap_read_data(AP_REG_ALS_DATA_LOW,data,2);
	uint32_t klux = data[1];
	klux *=256;
	klux = klux + data[0];
	klux *=1000;
	float f_klux = klux;
	switch(dynamic_range)
	{
		case 1:{
			f_klux = f_klux * 0.35;
		}break;
		case 2:{
			f_klux = f_klux * 0.0788;
		}break;
		case 3:{
			f_klux = f_klux * 0.0197;
		}break;
		case 4:{
			f_klux = f_klux * 0.0049;
		}break;
	}
	last_measure = f_klux;


	uint8_t *pData = (uint8_t*)&last_measure;
	data[0] = pData[0];
	data[1] = pData[1];
	data[2] = pData[2];
	data[3] = pData[3];

	return res;
}
