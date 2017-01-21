/*
The MIT License (MIT)

Copyright (c) 2017 Wassim Filali

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

___________________________________________________________________________________
 dependencies :
 - json.hpp 
 Copyright (c) 2013-2017 Niels Lohmann <http://nlohmann.me>.
 Licensed under the MIT License <http://opensource.org/licenses/MIT>.
___________________________________________________________________________________

 start date : 21.01.2017 port from 2016

 utils :
 * time measures
 * text string processing
 * generic sensors data maps
 * json stringify of sensors data
 * application arguments and file into map
 * time and date routines

*/

#ifndef __UTILS__
#define __UTILS__

#include <string>
#include <map>
#include <vector>
#include <ctime>

#include <ctime>
#include <chrono>

#include "json.hpp"


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
	typedef std::chrono::time_point<std::chrono::system_clock> time_u;
	
	//not thread safe, non sense for multithreading due to locking that spoils the measures !!!!!!
	void 			start();
	std::string 	stop();
	//this is thread safe as variables are kept on user thread only
	time_u			get_start();
	std::string		get_stop(time_u& start_time);
	
	//command line to map including check in configfile.txt
	std::string args2map( int argc, char** argv ,strmap &params);
	void str2map(const std::string &str ,strmap &params);
	
	bool exists(const strmap &params,const std::string param);
	std::string ParseRemTill(std::string &str,char sep,bool &found);
	std::string TakeParseTo(std::string &str,char sep);
	strvect split(std::string NodesList,char sep);
	std::string	getTime(std::time_t rawtime);
	std::string	getTime();
	std::string	getDay(std::time_t rawtime);
	std::string	getDay();
	void 		getYearMonthDay(std::time_t rawtime,std::string &text_year,std::string &text_month,std::string &text_day);
	void printTime(std::time_t rawtime);
	
	std::string remove_spaces(std::string &str);
	void replace(std::string &str,char what,char withwhat);
	std::string remove_0x(std::string &str);
	void remove(std::string substr, std::string &str);
	
	void hextext2data(const std::string &str, uint8_t *data,const int MaxData);
	std::string data2hextext(const uint8_t *data,int data_size);
	
	//json routines
	std::string 	stringify(NodeMap_t &measures,const std::string &type);
	void 			make_json(NodeMap_t &measures,nlohmann::json &jRes,const std::string &type);
	void 			make_json_resp(	int 				NodeId,
									const std::string 	&SensorName,
									 NodeMap_t 	&measures,
									nlohmann::json		&jRes,
									const std::string 	&type
									);
}

#endif /*__UTILS__*/