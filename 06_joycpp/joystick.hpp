/*
author : wassfila
date : 16.07.2016

Joystick cpp wrapper


*/

#include <linux/joystick.h>
#include <string>

class Joystick
{
private:
	int fd;
	struct js_event jse;
public:
	std::string 	Name;
	int		NbAxes;
	int		NbButtons;
public:
	void start(std::string device);
	void info();
	bool update();
	void process();
	
};

