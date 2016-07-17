#include <unistd.h>


#include "joystick.hpp"

int main() 
{
	int fd;
	struct js_event jse;

	fd = joy_open("/dev/input/js0");
	joy_info(fd);

	while (1) 
	{
		while (read(fd, &jse, sizeof(jse)) > 0) 
		{
			joy_process(jse);
		}
	}

	return 0;
}
