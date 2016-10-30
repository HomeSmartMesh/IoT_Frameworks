/*
	File : bme280_server.cpp
	creation date : 2016.10.30
	
*/

#include "bme280_server.hpp"
#include <cstring>


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
