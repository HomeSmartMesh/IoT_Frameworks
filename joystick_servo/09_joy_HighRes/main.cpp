
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



int main() 
{
	Joystick 	joy;
	Serial 		ser;
	
	joy.start("/dev/input/js0");
	
	ser.start("/dev/ttyUSB0");

	
	//std::cout << MAGENTA << "Colored " << CYAN << "Text" << RESET << std::endl;
	
	while (1) 
	{
		std::cout << CYAN;
		//Update the Joystick input
		if(joy.update())//multiple events will be filtered, only last would appear afterwards
		{
			joy.printUpdates();
		}
		
		//Send Joystick pos through serial port
		if(joy.getAxis(1).isUpdated())
		{
			const int s_path = 950;
			int val = 1520 + s_path/2 + joy.getAxis(1).getValue()*s_path/2;
			char data[4];
			data[0] = 'H';
			data[3] = 'R';
			data[1] = val / 256;
			data[2] = val % 256;
			ser.send(data,4);
		}
		
		joy.consumeAll();
		
		std::cout << RESET;
		//display Received log
		if(ser.update())
		{
			ser.print();
		}
		usleep (1000);
		
	}

	return 0;
}
