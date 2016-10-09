#include <unistd.h>


#include "joystick.hpp"

int main() 
{
	Joystick joy;
	joy.start("/dev/input/js0");
	joy.info();

	while (1) 
	{
		while (joy.update()) 
		{
			joy.process();
		}
	}

	return 0;
}
