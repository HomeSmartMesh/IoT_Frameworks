/*
	File : bme280_server.hpp
	creation date : 2016.10.30
	Author: wassfila : https://github.com/wassfila
	
	License: 
	* No warranties provided, final user owns the code with its full responsibilities

*/


#ifndef __BME_280_SERVER__
#define __BME_280_SERVER__

//for uint8_t
#include <cstdint>
#include <string>



class bme_measures_c
{
	//----------------------- Measures ----------------------- 
	public:
		void set_all_measures_8(uint8_t *data);
	public:
		int32_t		adc_P;//Pressure
		int32_t		adc_T;//Temperature
		int32_t		adc_H;//Humidity
		int32_t		t_fine;

	//----------------------- Calib Params ----------------------- 
	public: //calib params
		void set_calib_part1_10(uint8_t *data);
		void set_calib_part2_10(uint8_t *data);
		void set_calib_part3_6(uint8_t *data);
		void set_calib_part4_8(uint8_t *data);
		uint16_t	dig_T1;//usigned short
		int16_t		dig_T2;//signed short
		int16_t		dig_T3;//signed short
		
		uint16_t	dig_P1;//usigned short
		int16_t		dig_P2;//signed short
		int16_t		dig_P3;//signed short
		int16_t		dig_P4;//signed short
		int16_t		dig_P5;//signed short
		int16_t		dig_P6;//signed short
		int16_t		dig_P7;//signed short
		int16_t		dig_P8;//signed short
		int16_t		dig_P9;//signed short
		
		uint8_t		dig_H1;//unsigned char
		int16_t		dig_H2;//signed short
		uint8_t		dig_H3;//unsigned char
		int16_t		dig_H4;//signed short
		int16_t		dig_H5;//signed short
		int8_t		dig_H6;//signed char
		
		int32_t		comp_T;
		int32_t		comp_P;
		int32_t		comp_H;
	public:
		int		NodeId;
		float	temperature;
		float	humidity;
		float	pressure;
	public:
		void set_all_measures_Text(std::string Measures);
		void load_calib_data(std::string filename);
		int32_t	compensate_T_int32();//adc_T provided with m.set_all_measures_8()
		int32_t	compensate_P_int64();
		int32_t	compensate_H_int32();
		float 		get_float_temperature();
		float 		get_float_humidity();
		float 		get_float_pressure();
		std::string get_temperature();
		std::string get_humidity();
		std::string get_pressure();
};



#endif /*__BME_280_SERVER__*/