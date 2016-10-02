
//for usleep
#include <unistd.h>
//for printf
#include <stdio.h>
//for stdout
#include <iostream>

//for abs
#include <cmath>

#include "serial.hpp"

int main() 
{
	Serial 		ser;
	
	ser.start("/dev/ttyUSB0");

	
	//std::cout << MAGENTA << "Colored " << CYAN << "Text" << RESET << std::endl;
	
	while (1) 
	{
		//display Received log
		if(ser.update())
		{
			ser.print();
		}
		usleep (10000);
		
	}

	return 0;
}
