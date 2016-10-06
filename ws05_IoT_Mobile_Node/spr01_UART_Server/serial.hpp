/*
author : wassfila
date : 16.07.2016

serial port cpp wrapper


*/

#include <linux/joystick.h>
#include <string>
//for file
#include <fstream>

#define buf_size 2000

class Serial
{
private:
	int fd;
	char buf [buf_size];
	int n;
	bool newLine;
public:
	std::string 	Name;
	std::ofstream 	logfile;
public:
	void start(std::string port_name,bool s_500 = false);
	void start_logfile(std::string fileName);
	bool update();
	void log(const std::string &str);
	void logBuffer();
	void send(char* buffer,int size);
	
};

