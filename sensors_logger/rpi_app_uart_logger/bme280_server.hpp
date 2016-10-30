/*
	File : bme280_server.hpp
	creation date : 2016.10.30
	
*/


#ifndef __BME_280_SERVER__
#define __BME_280_SERVER__

//for uint8_t
#include <cstdint>
#include <string>

class bme_measures_c
{
	public:
		float	temperature;
		float	humidity;
		float	pressure;
	private:
		uint8_t data[8];
	public:
		void update_text(std::string text_data);
		void update(uint8_t *v_data);
		std::string get_temperature();
		std::string get_humidity();
		std::string get_pressure();
};



#endif /*__BME_280_SERVER__*/