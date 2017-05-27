
//for usleep
#include <unistd.h>
//for printf
#include <stdio.h>
//for stdout
#include <iostream>

//for abs
#include <cmath>

#include "joystick.hpp"
#include "serial.hpp"
#include "log.hpp"
#include "utils.hpp"


#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
 
#define CLEAR "\033[2J"  // clear screen escape code 

//Axis 5 : 1520 - 1950 

void printf_tab(const char* data, int size)
{
	char line_print[400];
	sprintf(line_print,"0x %02x %02x %02x %02x %02x %02x %02x\n",data[0],data[1],data[2],data[3],data[4],data[5],data[6]);
	std::string line(line_print);
	Log::cout << line << Log::Info();
}

void printf_tab2(const char* data, int size)
{
	char line_print[400];
	sprintf(line_print,"0x %02x %02x\n",data[0],data[1]);
	std::string line(line_print);
	Log::cout << line << Log::Info();
}

void MapAxis(JAxis &axis,char s_id,Serial&ser)
{
	if(axis.isUpdated())
	{
		int val = 10000*(axis.getValue()+1)/2;//[-1,+1] => [0, 1]
		uint8_t data[7];
		data[0] = 5;	//size is 5
		data[1] = 'S';	//Protocol 'Servos'
		data[2] = s_id;	//Servo Id = '1'
		data[3] = val / 256;	//16 bit val
		data[4] = val % 256;
		printf_tab2((const char*)&data[3],2);
		utl::crc_set(data);
		//printf_tab((const char*)data,7);
		ser.send((const char*)(data),5+2);
	}
}

int main( int argc, char** argv ) 
{
	std::cout << "______________________Config______________________" << std::endl;
	strmap conf;
	utl::args2map(argc,argv,conf);//here is checked './configfile.txt'

	Log::config(conf);

	Serial 			ser(conf);	// - process serial port stream

	Joystick 	joy;
	
	joy.start("/dev/input/js0");
	
	//std::cout << MAGENTA << "Colored " << CYAN << "Text" << RESET << std::endl;
	
	while (1) 
	{
		std::cout << CYAN;
		//Update the Joystick input
		if(joy.update())//multiple events will be filtered, only last would appear afterwards
		{
			//joy.printUpdates();
		}
		
		MapAxis(joy.getAxis(5),1,ser);//Up
	
		joy.consumeAll();
		
		std::cout << RESET;
		//display Received log
		if(ser.update())
		{
			ser.processBuffer();
			ser.logBuffer();//log received line
		}
		usleep (10000);
		
	}

	return 0;
}
