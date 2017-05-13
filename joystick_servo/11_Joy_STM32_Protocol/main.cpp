
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

void MapAxis(JAxis &axis,char c,int pos1,int pos2,Serial&ser)
{
	if(axis.isUpdated())
	{
		const int s_path = pos2 - pos1;
		int val = pos1 + s_path/2 + axis.getValue()*s_path/2;
		std::cout << "(" << val << ")";
		char data[4];
		data[0] = 'H';
		data[3] = c;
		data[1] = val / 256;
		data[2] = val % 256;
		ser.send(data,4);
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
			joy.printUpdates();
		}
		
		MapAxis(joy.getAxis(5),'1',1050,1950,ser);//Up
		MapAxis(joy.getAxis(2),'2',1050,1950,ser);//Side
		MapAxis(joy.getAxis(1),'3',500,2500,ser);//servo
	
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
