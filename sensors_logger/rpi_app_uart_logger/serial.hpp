/*
author : wassfila
date : 16.07.2016

serial port cpp wrapper


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
public:
	bool config(strmap &conf);
	void start(std::string port_name,bool s_500 = false);
	void start_logfile(std::string fileName);
	bool update();
	void log(const std::string &str);//append timestamp and output to file and cout
	void processBuffer();
	void logBuffer();
	void clearBuffer();
	void send(char* buffer,int size);
	void processLine();
	void logOneLine(std::string line);
public:
	std::string exepath;
};

