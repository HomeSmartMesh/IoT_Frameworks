/*
author : wassfila
date : 16.07.2016

serial port cpp wrapper


*/

#include <linux/joystick.h>
#include <string>

class Serial
{
private:
	int fd;
	char buf [100];
	int n;
public:
	std::string 	Name;
public:
	void start(std::string port_name,bool s_500 = false);
	bool update();
	void print();
	void send(char* buffer,int size);
	
};

