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


#include "utils.hpp"
#include <ctime>
#include <chrono>

//for file
#include <fstream>
#include <iostream>
#include <sstream>

//for setfill
#include <iomanip>

#include "json.hpp"
using json = nlohmann::json;

using namespace std;

std::chrono::time_point<std::chrono::system_clock> g_start;


utl::time_u	utl::get_start()
{
	return (utl::time_u)std::chrono::system_clock::now();
}

std::string	utl::get_stop(utl::time_u& start_time)
{
	std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start_time;

	std::string resp;
	long long ms;

	if(elapsed_seconds.count() > 1)
	{
		int sec = elapsed_seconds.count();
		resp = std::to_string(sec)+" s";
		ms = elapsed_seconds.count() - sec;
	}
	else
	{
		ms = elapsed_seconds.count() * 1000;
		resp += " " + std::to_string(ms)+" ms";
	}
	return resp;
}

//not thread safe, non sense for multithreading due to locking that spoils the measures !!!!!!
void utl::start()
{
	g_start = std::chrono::system_clock::now();
}

std::string utl::stop()
{
	return get_stop(g_start);
}


std::string TakeParseToLast(std::string &str,char sep)
{
	size_t first = str.find_last_of(sep);
	std::string Parsed = str.substr(0 , first);
	str = str.substr(first+1 ,str.length());
	return Parsed;
}

//take what's left if no separator found 
//important for robust parsing when lat delimiter is forgotten
std::string utl::TakeParseTo(std::string &str,char sep)
{
	std::string Parsed;
	size_t first = str.find_first_of(sep);
	if(first != string::npos)
	{
		Parsed = str.substr(0 , first);
		str = str.substr(first+1 ,str.length());
	}
	else//not found, then take what's left
	{
		Parsed = str;
		str = "";
	}
	return Parsed;
}

strvect utl::split(std::string NodesList,char sep)
{
	strvect strlist;
	while(NodesList != "")
	{
		std::string part = utl::TakeParseTo(NodesList,sep);
		strlist.push_back(part);
	}
	return strlist;
}

int char2int(char input)
{
	if(input >= '0' && input <= '9')
		return input - '0';
	if(input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	if(input >= 'a' && input <= 'f')
		return input - 'a' + 10;
}

void utl::hextext2data(const std::string &str, uint8_t *data)
{
	std::string v_str = str;
	//std::cout << "C1: " << v_str << std::endl;
	utl::remove_spaces(v_str);
	//std::cout << "C2: " << v_str << std::endl;
	utl::remove_0x(v_str);
	//std::cout << "C3: " << v_str << std::endl;
	int i=0;
	while(i<v_str.length()-1)
	{
		*(data++) = char2int(v_str[i])*16 + char2int(v_str[i+1]);
		i += 2;
	}
}

//doesn work
//input : 0x23 0xCA 0x5F
//output : 0x0# 0▒ 0_
std::string data2hextext_(const uint8_t *data,int data_size)
{
	std::stringstream s_text;
	s_text << "0x";
	for(int i=0;i<data_size;i++)
	{
		s_text << std::hex << std::setfill('0') << std::setw(2) << data[i] << " ";
	}
	
	return s_text.str();
}

std::string utl::data2hextext(const uint8_t *data,int data_size)
{
	char text[data_size*3+1];
	char* p_text = text;
	sprintf(p_text,"0x");
	p_text+=2;
	for(int i=0;i<data_size;i++)
	{
		sprintf(p_text,"%02X ",data[i]);
		p_text+=3;
	}
	p_text='\0';
	std::string s_text(text);
	return s_text;
}

void utl::replace(std::string &str,char what,char withwhat)
{
    for(char& c : str)
	{
		if(c == what)
		{
			c = withwhat;
		}
	}
}

std::string utl::remove_spaces(std::string &str)
{
	//remove_if not found
	//str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
	std::string res;
    for(int i=0; i<str.size(); i++)
	{
		if(str[i]!=' ')
			res+=str[i];
	}
	str = res;
    return res;	
}

std::string utl::remove_0x(std::string &str)
{
	//remove_if not found
	//str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
	std::string res;
    for(int i=0; i<str.size()-1; i++)
	{
		if((str[i]=='0') && (str[i+1]=='x'))
		{
			//not only skip, but skip two characters by incrementing the counter
			i++;
		}
		else
		{
			res+=str[i];
			if(i == (str.size()-2))//last pass, and not "0x"
			{
				res+=str[i+1];//don't forget last character
			}
		}
	}
	str = res;
    return res;	
}

void utl::remove(std::string substr, std::string &str)
{
	std::string::size_type i = str.find(substr);

	if (i != std::string::npos)
	   str.erase(i, substr.length());
}

std::string utl::ParseRemTill(std::string &str,char sep,bool &found)
{
	size_t first = str.find_first_of(sep);
	if(first!=std::string::npos)
	{
		found = true;
	}
	else
	{
		found = false;
	}
	std::string Parsed = str.substr(0 , first);
	str = str.substr(first+1 ,str.length());
	return Parsed;
}

//here the parsed line should be as follows
//var1:valx;var3:valy;
//note that a safety limitation restricts the number of params to max 20
void utl::str2map(const std::string &str ,strmap &params)
{
	int max = 0;
	std::string vstr = str;
	//DBG_CMT std::cout << "START with : " << vstr << std::endl;
	while((!vstr.empty()) && (max < 20))
	{
		//DBG_CMT std::cout << "LOOP with : " << vstr << std::endl;
		std::string arg_name = TakeParseTo(vstr,':');
		params[arg_name] = TakeParseTo(vstr,';');
		max++;
	}
	//DBG_CMT std::cout << "DONE with : " << vstr << std::endl;
}

std::string utl::args2map( int argc, char** argv ,strmap &params)
{
	std::string exepath(*argv);//placed here before pointer is shifted
	while(argc--)
	{
		std::string argv_str(*argv++);
		//to debug parameters list
		//std::cout << "[" << argv_str << "]" << std::endl;
		std::string arg_name = TakeParseTo(argv_str,'=');
		params[arg_name] = argv_str;
	}
	//once all params in cmd line parsed check if a config file is available
	std::ifstream 	configfile;
	if(!utl::exists(params,"configfile"))//the parameter 'configfile' was not passed, so check the default one
	{
		exepath = TakeParseToLast(exepath,'/');
		params["exepath"] = exepath;
		params["configfile"] = exepath + "/configfile.txt";
	}
	configfile.open(params["configfile"].c_str());
	if(configfile.is_open())
	{
		std::cout << "configfile = "<< params["configfile"] << std::endl;
		string line;
		while ( getline (configfile,line) )
		{
			line = TakeParseTo(line,'#');//allow comment character
			std::string arg_name = TakeParseTo(line,'=');
			if(!utl::exists(params,arg_name))//if the new parameter from the file was not already provided in cmd line
			{
			params[arg_name] = TakeParseTo(line,'\r');//take the windows line ending out
			}//else ignore it and keep precedence for cmd line
		}
	}
	else
	{
		std::cout << "No Config file in cmd line and not found in exe dir: " <<  params["configfile"] << endl;
	}
		return exepath;
}

bool utl::exists(const strmap &params,const std::string param)
{
	return (params.find(param) != params.end());
}

std::string	utl::getTime(time_t rawtime)
{
	struct tm * timeinfo;
	char buffer[80];

	timeinfo = localtime(&rawtime);

	strftime(buffer,80,"%H:%M:%S",timeinfo);
	std::string str(buffer);
	return str;	
}

std::string	utl::getTime()
{
	time_t rawtime;
	time(&rawtime);
	
	return utl::getTime(rawtime);
}

std::string	utl::getDay(time_t rawtime)
{
	struct tm * timeinfo;
	char buffer[80];

	timeinfo = localtime(&rawtime);

	strftime(buffer,80,"%Y-%m-%d",timeinfo);
	std::string str(buffer);
	return str;	
}

std::string	utl::getDay()
{
	time_t rawtime;
	time (&rawtime);
	return utl::getDay(rawtime);
}

void utl::getYearMonthDay(std::time_t rawtime,std::string &text_year,std::string &text_month,std::string &text_day)
{
	struct tm * timeinfo;
	timeinfo = localtime(&rawtime);

	char buffer[10];
	strftime(buffer,10,"%Y",timeinfo);
	text_year = std::string(buffer);
	strftime(buffer,10,"%m",timeinfo);
	text_month = std::string(buffer);
	strftime(buffer,10,"%d",timeinfo);
	text_day = std::string(buffer);
}

void utl::printTime(std::time_t rawtime)
{
	std::cout << getDay(rawtime) << "\t" << getTime(rawtime) << std::endl;
}

std::string utl::stringify(NodeMap_t &measures,const std::string &type)
{
	json jsonMeasures;
	
	for(auto const& sensorsTables : measures)
	{
		std::string NodeId = std::to_string(sensorsTables.first);
		for(auto const& Table : sensorsTables.second) 
		{
			std::string SensorName = Table.first;
			for(auto const& Measure : Table.second) 
			{
				jsonMeasures[type][NodeId][SensorName]["Times"].push_back(Measure.time);
				jsonMeasures[type][NodeId][SensorName]["Values"].push_back(Measure.value);
			}
		}
	}
	
	return jsonMeasures.dump();
}

void utl::make_json(NodeMap_t &measures,json &jRes,const std::string &type)
{
	for(auto const& sensorsTables : measures)
	{
		std::string NodeId = std::to_string(sensorsTables.first);
		for(auto const& Table : sensorsTables.second) 
		{
			std::string SensorName = Table.first;
			for(auto const& Measure : Table.second) 
			{
				jRes[type][NodeId][SensorName]["Times"].push_back(Measure.time);
				jRes[type][NodeId][SensorName]["Values"].push_back(Measure.value);
			}
		}
	}
	
}

void utl::make_json_resp(	int 				NodeId,
							const std::string 	&SensorName,
							NodeMap_t 	&measures,
							json 				&jRes,
							const std::string 	&type
							)
{
	const sensor_measures_table_t &resp_measures = measures[NodeId][SensorName];
	for(auto const& Measure : resp_measures)
	{
		jRes[type]["Times"].push_back(Measure.time);
		jRes[type]["Values"].push_back(Measure.value);
	}
}
