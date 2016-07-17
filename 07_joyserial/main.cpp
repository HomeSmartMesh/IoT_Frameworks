
//for usleep
#include <unistd.h>
//for printf
#include <stdio.h>
//for stdout
#include <iostream>


#include "joystick.hpp"
#include "serial.hpp"

int main() 
{
	Joystick 	joy;
	Serial 		ser;
	
	joy.start("/dev/input/js0");
	joy.info();
	
	ser.start("/dev/ttyUSB0");

	while (1) 
	{
		while(joy.update())
		{
			joy.process();
		}
		if(ser.update())
		{
			ser.print();
		}
		usleep (1000);
		
	}

	return 0;
}
