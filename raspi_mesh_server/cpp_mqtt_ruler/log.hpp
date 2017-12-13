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

#include <string>
//for file
#include <fstream>
//for stringstream
#include <sstream>

#include <vector>
#include <map>
#include <list>

#include "utils.hpp"

class Log
{
public:
	static const int loglevel_None 		;
	static const int loglevel_Error 	;
	static const int loglevel_Warning 	;
	static const int loglevel_Info 		;
	static const int loglevel_Debug 	;
public:
	static bool config(strmap &conf);
public:
	static int fd;
public:
	static std::ofstream 	logfile;
	static std::stringstream cout;
	static bool			isLogFile;
	static bool			isLogOut;
	static bool			isReady;
	static int			level_file;
	static int			level_out;
public:
	static void			log(const std::string &str,const int &level);
	static std::string	Error();
	static std::string	Warning();
	static std::string  Info();
	static std::string  Debug();

};

