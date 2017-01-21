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

 start date : 16.07.2016

 serial port cpp wrapper
 * update() to get data from the serial port buffer
 * collect data into lines and parses each completed line
 * calls the bme_280 as main sensors are calibrated from within that library
 * transforms the registers values into ready to use sensors map Nodes.Sensors.Values,Timestamp
 
*/

#include "bme280_server.hpp"

#include <linux/joystick.h>
#include <string>
//for file
#include <fstream>
//for stringstream
#include <sstream>

#include <vector>
#include <map>

#include "utils.hpp"

#define buf_size 2000

class LogBuffer_c
{
public:
	LogBuffer_c();//constructor
public:
	char buf [buf_size];
	char linebuf [buf_size];
	char * plinebuf;//has to be global to keep half lines position
	int n;
	bool newLine;
	std::time_t	time_now;
	std::string day;
	std::string time;
	std::string line;
	std::vector<std::string> currentlines;
public:
	bool update(int fd);
};

class Serial
{
public:
	Serial();//constructor
	Serial(strmap &conf);//constructor
public:
	int fd;
	LogBuffer_c logbuf;
public:
	std::map<int,bme_measures_c> NodesMeasures;
public:
	std::string 	Name;
	std::ofstream 	logfile;
	bool			isLogFile;
	bool			isLogOut;
	bool			isReady;
public:
	bool 		config(strmap &conf);
	void 		start(std::string port_name,bool s_500 = false);
	void 		start_logfile(std::string fileName);
	bool 		update();
	void 		log(const std::string &str);//append timestamp and output to file and cout
	NodeMap_t 	processBuffer();
	void 		logBuffer();
	void 		clearBuffer();
	void 		send(char* buffer,int size);
	void 		processLine(NodeMap_t &nodes);
	void 		logOneLine(std::string line);
	
public:
	std::string exepath;
};

