
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

int main() 
{
	Joystick 	joy;
	Serial 		ser;
	
	joy.start("/dev/input/js0");
	
	ser.start("/dev/ttyUSB0");

	while (1) 
	{
		//Update the Joystick input
		if(joy.update())//multiple events will be filtered, only last would appear afterwards
		{
			joy.printUpdates();
		}
		
		//Send Joystick pos through serial port
		if(joy.getAxis(1).isUpdated())
		{
			char val = std::abs(joy.getAxis(1).getValue()*255);
			char data[4];
			data[0] = 'S';
			data[1] = 'r';
			data[2] = val;
			data[3] = ~val;
			ser.send(data,4);
		}
		
		joy.consumeAll();
		
		//display Received log
		if(ser.update())
		{
			ser.print();
		}
		usleep (1000);
		
	}

	return 0;
}
