
#include "BMP280_API.h"

// t_fine carries fine temperature as global value
S32_t t_fine;

S32_t dig_T1,dig_T2,dig_T3;

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
S32_t BME280_compensate_T_int32(S32_t adc_T)
{
	S32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((S32_t)dig_T1<<1))) * ((S32_t)dig_T2)) >> 11;
	var2 = (((((adc_T>>4) - ((S32_t)dig_T1)) * ((adc_T>>4) - ((S32_t)dig_T1))) >> 12) * ((S32_t)dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
/*U32_t BME280_compensate_P_int64(S32_t adc_P)
{
	S64_t var1, var2, p;
	var1 = ((S64_t)t_fine) – 128000;
	var2 = var1 * var1 * (S64_t)dig_P6;
	var2 = var2 + ((var1*(S64_t)dig_P5)<<17);
	var2 = var2 + (((S64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (S64_t)dig_P3)>>8) + ((var1 * (S64_t)dig_P2)<<12);
	var1 = (((((S64_t)1)<<47)+var1))*((S64_t)dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((S64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((S64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((S64_t)dig_P7)<<4);
	return (U32_t)p;
}*/


// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
/*
U32_t bme280_compensate_H_int32(S32_t adc_H)
{
	S32_t v_x1_u32r;
	v_x1_u32r = (t_fine – ((S32_t)76800));
	v_x1_u32r = (((((adc_H << 14) – (((S32_t)dig_H4) << 20) – (((S32_t)dig_H5) * v_x1_u32r)) +
	((S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((S32_t)dig_H6)) >> 10) * (((v_x1_u32r *
	((S32_t)dig_H3)) >> 11) + ((S32_t)32768))) >> 10) + ((S32_t)2097152)) *
	((S32_t)dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r – (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((S32_t)dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (U32_t)(v_x1_u32r>>12);
}
*/