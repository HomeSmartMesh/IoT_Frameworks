
#ifndef __UTILS__
#define __UTILS__

#include <string>
#include <map>
#include <vector>

typedef std::map<std::string,std::string> strmap;

typedef std::vector<std::string> strvect;

// temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH

struct sensor_measure_t
{
	std::time_t time;
	float		value;
};

typedef std::vector<sensor_measure_t> sensor_measures_table_t;

typedef std::map<std::string,sensor_measures_table_t> sensors_tables_t;

typedef std::map<int,sensors_tables_t> NodeMap_t;


namespace utl
{
	std::string args2map( int argc, char** argv ,strmap &params);
	void str2map(const std::string &str ,strmap &params);
	bool exists(const strmap &params,const std::string param);
	std::string ParseRemTill(std::string &str,char sep,bool &found);
	std::string TakeParseTo(std::string &str,char sep);
	strvect split(std::string NodesList,char sep);
	std::string	utl::getTime(time_t rawtime);
	std::string	getTime();
	std::string	getDay(time_t rawtime);
	std::string	getDay();
	
	std::string remove_spaces(std::string &str);
	std::string remove_0x(std::string &str);
	void remove(const std::string &substr, std::string &str);
	
	void hextext2data(const std::string &str, uint8_t *data);
	std::string data2hextext(const uint8_t *data,int data_size);
}

#endif /*__UTILS__*/