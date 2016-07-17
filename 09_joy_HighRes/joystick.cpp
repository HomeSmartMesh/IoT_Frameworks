#include "joystick.hpp"

#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>

#include <iostream>


//----------------------- JAxis --------------------------------
JAxis::JAxis()
{
	i_value = 0;
	update = false;
}

bool JAxis::isUpdated()
{
	return update;
}

float JAxis::getValue()
{
	float v = i_value;
	v = v / 32767;
	return v;
}

//----------------------- JButton --------------------------------
JButton::JButton()
{
	b_value = false;
	update = false;
}

bool JButton::isUpdated()
{
	return update;
}

bool JButton::getState()
{
	return b_value;
}

std::string JButton::getStateText()
{
	std::string str;
	if(b_value)
	{
		str = "isDown";
	}
	else
	{
		str = "isUp";
	}
	return str;
}


//----------------------- Joystick --------------------------------
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
	//call info necessarily to confire the arrays sizes
	info();
}

void Joystick::info()
{
	int axes = 0, buttons = 0;
	char name[128];

	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(sizeof(name)), &name);

	Axes.resize(axes);
	Buttons.resize(buttons);
	Name = name;
	
	printf("%s\n  %d Axes %d Buttons\n", name, axes, buttons);
}

bool Joystick::update()
{
	bool res = false;
	while(read(fd, &jse, sizeof(jse)) > 0)
	{
		if (jse.type == JS_EVENT_AXIS)
		{
			Axes[jse.number].i_value = jse.value;
			Axes[jse.number].update = true;
			Axes[jse.number].time	= jse.time;
			res = true;
		}
		else if (jse.type == JS_EVENT_BUTTON)
		{
			Buttons[jse.number].b_value = (jse.value > 0);//is pressed ?
			Buttons[jse.number].update = true;
			Buttons[jse.number].time	= jse.time;
			res = true;
		}
		//else we ignore the init from the updates
	}
	return res;
}

void Joystick::consumeAll()
{
	for (std::vector<JAxis>::iterator axes_it = Axes.begin() ; axes_it != Axes.end(); ++axes_it)
	{
		axes_it->update = false;
	}
	for (std::vector<JButton>::iterator but_it = Buttons.begin() ; but_it != Buttons.end(); ++but_it)
	{
		but_it->update = false;
	}
}

void Joystick::printUpdates()
{
	std::cout << std::fixed;
	std::cout.precision(2);
   
	int index = 0;
	for (std::vector<JAxis>::iterator axes_it = Axes.begin() ; axes_it != Axes.end(); ++axes_it)
	{
		if(axes_it->update)
		{
			std::cout << axes_it->time << " : Axis " << index << " @ " << axes_it->getValue() << std::endl;
		}
		index++;
	}

	index = 0;
	for (std::vector<JButton>::iterator but_it = Buttons.begin() ; but_it != Buttons.end(); ++but_it)
	{
		if(but_it->update)
		{
			std::cout << but_it->time << " : Button " << index << " @ " << but_it->getStateText() << std::endl;
		}
		index++;
	}
}

JAxis &Joystick::getAxis(int axisId)
{
	return Axes[axisId];
}
