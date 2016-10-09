#include "joystick.hpp"

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>





void Joystick::start(std::string device_name)
{
	if (!device_name.empty())
	{
		fd = open(device_name.c_str(), O_RDONLY | O_NONBLOCK);

		if (fd < 0)
		{
			printf("Could not locate joystick device %s\n", device_name.c_str());
			exit(1);
		}
	}
}

void Joystick::info()
{
	int axes = 0, buttons = 0;
	char name[128];

	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(sizeof(name)), &name);

	NbAxes = axes;
	NbButtons = buttons;

	printf("%s\n  %d Axes %d Buttons\n", name, axes, buttons);
}

bool Joystick::update()
{
	return (read(fd, &jse, sizeof(jse)) > 0);
}

void Joystick::process()
{
	if (jse.type == JS_EVENT_AXIS)
	{
		float v = jse.value;
		v = v / 32767;
		printf("%d : Axis %d @ %0.2f\n", jse.time, jse.number, v);
	}
	else if (jse.type == JS_EVENT_BUTTON && jse.value > 0)
	{
		printf("%d : Button %d\n", jse.time, jse.number);
	}
}

