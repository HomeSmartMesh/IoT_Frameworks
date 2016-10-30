/*
	File : bme280_server.hpp
	creation date : 2016.10.30
	
*/


#ifndef __BME_280_SERVER__
#define __BME_280_SERVER__

//for uint8_t
#include <cstdint>
#include <string>

namespace bme
{
	void text2data(const std::string &str, uint8_t* data);
}


#endif /*__BME_280_SERVER__*/