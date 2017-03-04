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
 - bme_280 with datasheet's associated rights see bme280_server.hpp
___________________________________________________________________________________

 start date : 19.02.2017

 logger
 
*/

#include "log.hpp"

//for ios::out,... #issue once placed in the end of the includes, it does not recognise cout part of std::
#include <iostream>
#include <fstream>
#include <string>

//for getTime
#include "utils.hpp"
#include <stdio.h>
#include <string.h>

const int Log::loglevel_None 	= 0;
const int Log::loglevel_Error 	= 1;
const int Log::loglevel_Warning = 2;
const int Log::loglevel_Info 	= 3;
const int Log::loglevel_Debug 	= 4;

std::ofstream 	Log::logfile;
bool	Log::isLogFile = false;
bool	Log::isLogOut = false;
bool	Log::isReady = false;
int		Log::level_file = Log::loglevel_None;
int		Log::level_out = Log::loglevel_None;
std::stringstream Log::cout;

bool Log::config(strmap &conf)
{
	bool res = true;
	
	//logfile : log into a file------------------------------------------------------
	if(utl::exists(conf,"logfile"))
	{
		std::cout << "log> logfile = " << conf["logfile"] << std::endl;
		std::string fileName = conf["logfile"];
		logfile.open(fileName.c_str(), (std::ios::out|std::ios::app) );
		if(!logfile.is_open())
		{
			printf("could not open log file:%s\r\n",fileName.c_str());
		}
		else
		{
			if(utl::exists(conf,"level_file"))
			{
				level_out = std::stoi(conf["level_file"]);
				std::cout << "log> level_file = " << level_file << std::endl;
			}
			else
			{
				level_file = loglevel_Warning;
			}
		}
	}

	//logout is on by default, only a 0 stops it------------------------------------
	isLogOut = true;//by default
	if(utl::exists(conf,"level_out"))
	{
		level_out = std::stoi(conf["level_out"]);
		std::cout << "log> level_out = " << level_out << std::endl;
	}
	else
	{
		level_out = loglevel_Debug;
	}
	
	isReady = res;
	return isReady;
}

std::string logstr(int level)
{
	switch(level)
	{
		case Log::loglevel_Error :
			return "Error";
		case Log::loglevel_Warning :
			return "Warning";
		case Log::loglevel_Info :
			return "Info";
		case Log::loglevel_Debug :
			return "Debug";
		default: 
			return "";
	}
}

void Log::log(const std::string &str,const int &level)
{
	if (!isReady)
	{
		return;
	}
	std::string d = utl::getDay();
	std::string t = utl::getTime();
	//Debug 4 >= Info 3
	if(level_out >= level)
	{
		std::cout << d << " " << t << "\t";
		std::cout << logstr(level) << "\t" << str << std::endl;
	}
	if( (level_file >= level) && logfile.is_open())
	{
		logfile << d << " " << t << "\t";
		logfile << logstr(level) << "\t"<< str << std::endl;
		logfile.flush();
	}
}

std::string Log::Error()
{
	log(cout.str(),loglevel_Error);
	cout.str(std::string());
	return "";
}

std::string Log::Warning()
{
	log(cout.str(),loglevel_Warning);
	cout.str(std::string());
	return "";
}

std::string Log::Info()
{
	log(cout.str(),loglevel_Info);
	cout.str(std::string());
	return "";
}

std::string Log::Debug()
{
	log(cout.str(),loglevel_Debug);
	cout.str(std::string());
	return "";
}
