/*
	File : bme280_server.cpp
	creation date : 2016.10.30
	
*/

#include "utils.hpp"
#include "bme280_server.hpp"
#include <cstring>
//for file
#include <fstream>
#include <iostream>


void bme_measures_c::update_text(std::string text_data)
{
	
}
void bme_measures_c::update(uint8_t *v_data)
{
	std::memcpy(data,v_data,8);
}
std::string bme_measures_c::get_temperature()
{
	return "";
}

std::string bme_measures_c::get_humidity()
{
	return "";
}
std::string bme_measures_c::get_pressure()
{
	return "";
}

void set_calib_part1_10(uint8_t *data)
{
	std::cout << "P1:" << utl::data2hextext(data,10) << std::endl;
}

void set_calib_part2_10(uint8_t *data)
{
	std::cout << "P2:" << utl::data2hextext(data,10) << std::endl;
}

void set_calib_part3_6(uint8_t *data)
{
	std::cout << "P3:" << utl::data2hextext(data,6) << std::endl;
}

void set_calib_part4_8(uint8_t *data)
{
	std::cout << "P4:" << utl::data2hextext(data,8) << std::endl;
}

void bme_measures_c::load_calib_data(std::string filename)
{
	std::ifstream 	calibfile;
	calibfile.open(filename);
	if(calibfile.is_open())
	{
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
				//std::cout << "T1:" << line << std::endl;
				utl::hextext2data(line,data_vals);
				set_calib_part1_10(data_vals);
				//line 2 : Reg 0x92
				getline (calibfile,line);
				utl::TakeParseTo(line,':');
				utl::remove_spaces(line);
				utl::remove_0x(line);
				//std::cout << "T2:" << line << std::endl;
				utl::hextext2data(line,data_vals);
				set_calib_part2_10(data_vals);
				//line 3 : Reg 0x9C
				getline (calibfile,line);
				utl::TakeParseTo(line,':');
				utl::remove_spaces(line);
				utl::remove_0x(line);
				//std::cout << "T3:" << line << std::endl;
				utl::hextext2data(line,data_vals);
				set_calib_part3_6(data_vals);
				//line 4 : Reg 0xE1
				getline (calibfile,line);
				utl::TakeParseTo(line,':');
				utl::remove_spaces(line);
				utl::remove_0x(line);
				//std::cout << "T4:" << line << std::endl;
				utl::hextext2data(line,data_vals);
				set_calib_part4_8(data_vals);
			}
			
		}
	}
	else
	{
		std::cout << "No Calib file found " << std::endl;
	}
	
}
