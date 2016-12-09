/*
	File : bme280_server.cpp
	creation date : 2016.10.30
	Author: wassfila : https://github.com/wassfila
	
	License 1: 
	* No warranties provided, final user owns the code with its full responsibilities
	
	License 2: 
	the compensation code is taken from the datasheet
	https://cdn-shop.adafruit.com/datasheets/BST-BME280_DS001-10.pdf
	all associated rights reserved
*/

#include "utils.hpp"
#include "bme280_server.hpp"
#include <cstring>
//for file
#include <fstream>
#include <iostream>


void bme_measures_c::set_all_measures_8(uint8_t *data)
{
	//DEBUG std::cout << "Measures:" << utl::data2hextext(data,8) << std::endl;

	//   0    1    2    3    4    5    6    7
	//0xF7 0xF8 0xF9 0xFA 0xFB 0xFC 0xFD 0xFE
	adc_P = (int32_t) (  (uint32_t)data[0]<<12 | (uint32_t)data[1]<<4 | data[2]>>4  );	//0xF7<<12 | 0xF8<<4 | 0xF9>>4
	adc_T = (int32_t) (  (uint32_t)data[3]<<12 | (uint32_t)data[4]<<4 | data[5]>>4  );	//0xFA<<12 | 0xFB<<4 | 0xFC>>4
	adc_H = (int32_t) (  (uint32_t)data[6]<<8  | data[7]  );							//0xF7<<12 | 0xF8<<4 | 0xF9>>4
}

void bme_measures_c::set_all_measures_Text(std::string Measures)
{
	uint8_t data_vals[8];
	utl::hextext2data(Measures,data_vals);
	std::string check_str = utl::data2hextext(data_vals,8);
	//DEBUG std::cout << "BME280 here !!!! with value: " << check_str << std::endl;
	
	//update measures values
	set_all_measures_8(data_vals);
	
	//process the compensation with the calibration parameters !temperature first as t_fine used by others!
	//DEBUG std::cout << "compensate_T_int32()" << std::endl;
	comp_T = compensate_T_int32();
	//DEBUG std::cout << "compensate_P_int64()" << std::endl;
	comp_P = compensate_P_int64();
	//DEBUG std::cout << "compensate_H_int32()" << std::endl;
	comp_H = compensate_H_int32();
}


// adc_T provided with m.set_all_measures_8()
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
int32_t	bme_measures_c::compensate_T_int32()
{
	int32_t var1, var2, T;
	var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
	var2 = (	(		(	((adc_T>>4) - ((int32_t)dig_T1)) * 
							((adc_T>>4) - ((int32_t)dig_T1))
						) >> 12
				) *	((int32_t)dig_T3)
			) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

// adc_P provided with m.set_all_measures_8()
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
int32_t	bme_measures_c::compensate_P_int64()
{
	int64_t var1, var2, p;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)dig_P6;
	var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
	var2 = var2 + (((int64_t)dig_P4)<<35);
	var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
	var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576-adc_P;
	p = (((p<<31)-var2)*3125)/var1;
	var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
	var2 = (((int64_t)dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
	return (uint32_t)p;
}

// adc_H provided with m.set_all_measures_8()
// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
int32_t	bme_measures_c::compensate_H_int32()
{
	int32_t v_x1_u32r;
	
	v_x1_u32r = (t_fine - ((int32_t)76800));
	v_x1_u32r = (	(	(	(	(adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)
							) + ((int32_t)16384)
						) >> 15
					) * 
					(	(	(	(	(((v_x1_u32r * ((int32_t)dig_H6)) >> 10) * (((v_x1_u32r *((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10
								) + ((int32_t)2097152)
							) *	((int32_t)dig_H2) + 8192
						) >> 14
					)
				);
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	
	return (uint32_t)(v_x1_u32r>>12);
}


// comp_T : temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
std::string bme_measures_c::get_temperature()
{
	//DEBUG std::cout << "get_temperature(" << comp_T<< ")"<<std::endl;
	float t_f = comp_T;
	t_f = t_f / 100;
	char result[10];
	//DEBUG std::cout << "sprintf()" << std::endl;
	sprintf(result,"%2.2f °C",t_f);
	//DEBUG std::cout << "string()" << std::endl;
	std::string res_text(result);

	//DEBUG std::cout << "return(" << res_text << ")"<< std::endl;
	return res_text;
}

// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
std::string bme_measures_c::get_humidity()
{
	float h_f = comp_H;
	h_f = h_f / 1024;

	char result[16];
	sprintf(result,"%2.3f %%RH",h_f);
	std::string res_text(result);
	
	return res_text;
}

// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
std::string bme_measures_c::get_pressure()
{
	float h_p = comp_P;
	h_p = h_p / 25600;

	char result[16];
	sprintf(result,"%3.3f hPa",h_p);
	std::string res_text(result);
	
	return res_text;
}

void bme_measures_c::set_calib_part1_10(uint8_t *data)
{
	//DEBUG std::cout << "Part1:" << utl::data2hextext(data,10) << std::endl;
	
	//data[10] : 0x88 0x89 0x8A 0x8B 0x8C 0x8D 0x8E 0x8F 0x90 0x91
	dig_T1 = 			(uint16_t)data[1]<<8 | data[0];		//0x89<<8 | 0x88
	dig_T2 = (int16_t)( (uint16_t)data[3]<<8 | data[2]);	//0x8B<<8 | 0x8A
	dig_T3 = (int16_t)( (uint16_t)data[5]<<8 | data[4]);	//0x8D<<8 | 0x8C

	dig_P1 = 			(uint16_t)data[7]<<8 | data[6];		//0x8F<<8 | 0x8E
	dig_P2 = (int16_t)( (uint16_t)data[9]<<8 | data[8]);	//0x91<<8 | 0x90
}


void bme_measures_c::set_calib_part2_10(uint8_t *data)
{
	//DEBUG std::cout << "Part2:" << utl::data2hextext(data,10) << std::endl;

	//data[10] : 0x92 0x93 0x94 0x95 0x96 0x97 0x98 0x99 0x9A 0x9B
	dig_P3 = (int16_t)( (uint16_t)data[1]<<8 | data[0]);	//0x93<<8 | 0x92
	dig_P4 = (int16_t)( (uint16_t)data[3]<<8 | data[2]);	//0x95<<8 | 0x94
	dig_P5 = (int16_t)( (uint16_t)data[5]<<8 | data[4]);	//0x97<<8 | 0x96
	dig_P6 = (int16_t)( (uint16_t)data[7]<<8 | data[6]);	//0x99<<8 | 0x98
	dig_P7 = (int16_t)( (uint16_t)data[9]<<8 | data[8]);	//0x9B<<8 | 0x9A
}

void bme_measures_c::set_calib_part3_6(uint8_t *data)
{
	//DEBUG	std::cout << "Part3:" << utl::data2hextext(data,6) << std::endl;

	//data[6] : 0x9C 0x9D 0x9E 0x9F 0xA0 0xA1
	dig_P8 = (int16_t)( (uint16_t)data[1]<<8 | data[0]);	//0x9D<<8 | 0x9C
	dig_P9 = (int16_t)( (uint16_t)data[3]<<8 | data[2]);	//0x9F<<8 | 0x9E

	dig_H1 = data[5];										//0xA1 : note that 0xA0 (data[4]) is skipped
}

void bme_measures_c::set_calib_part4_8(uint8_t *data)
{
	//DEBUG std::cout << "Part4:" << utl::data2hextext(data,8) << std::endl;

	//   0    1    2    3    4    5    6    7
	//0xE1 0xE2 0xE3 0xE4 0xE5 0xE6 0xE7 0xE8
	dig_H2 = (int16_t)( (uint16_t)data[1]<<8 | data[0]);			//0xE2<<8 | 0xE1
	dig_H3 = data[2];												//0xE3
	dig_H4 = (int16_t)( (uint16_t)data[3]<<4 | (0x0F & data[4]));	//0xE4<<4 | 0xE5[3:0] : (12bits) Exception lower is on msb here
	dig_H5 = (int16_t)( (uint16_t)data[5]<<4 | (data[4]>>4));		//0xE6<<4 | 0xE5[7:4] : (12bits) 4 bits left from 0xE5 used here
	dig_H6 = (int8_t)data[6];										//0xE7 : 0xE8 is unused

}

void bme_measures_c::load_calib_data(std::string filename)
{
	std::ifstream 	calibfile;
	calibfile.open(filename);
	if(calibfile.is_open())
	{
		std::cout << "Loading calib file(" << filename << ")" << std::endl;
		std::string line;
		while ( getline (calibfile,line) )
		{
			strmap params;
			utl::str2map(line,params);
			if(utl::exists(params,"NodeId"))
			{
				NodeId = atoi(params["NodeId"].c_str());
				//line 1 : Reg 0x88
				getline (calibfile,line);
				utl::TakeParseTo(line,':');
				utl::remove_spaces(line);
				utl::remove_0x(line);
				uint8_t data_vals[10];
				std::cout << "T1:" << line << std::endl;
				utl::hextext2data(line,data_vals);
				set_calib_part1_10(data_vals);
				//line 2 : Reg 0x92
				getline (calibfile,line);
				utl::TakeParseTo(line,':');
				utl::remove_spaces(line);
				utl::remove_0x(line);
				std::cout << "T2:" << line << std::endl;
				utl::hextext2data(line,data_vals);
				set_calib_part2_10(data_vals);
				//line 3 : Reg 0x9C
				getline (calibfile,line);
				utl::TakeParseTo(line,':');
				utl::remove_spaces(line);
				utl::remove_0x(line);
				std::cout << "T3:" << line << std::endl;
				utl::hextext2data(line,data_vals);
				set_calib_part3_6(data_vals);
				//line 4 : Reg 0xE1
				getline (calibfile,line);
				utl::TakeParseTo(line,':');
				utl::remove_spaces(line);
				utl::remove_0x(line);
				std::cout << "T4:" << line << std::endl;
				utl::hextext2data(line,data_vals);
				set_calib_part4_8(data_vals);
			}
			
		}
	}
	else
	{
		std::cout << "No Calib file found(" << filename << ")" << std::endl;
	}
	
}

