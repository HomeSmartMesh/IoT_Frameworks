/*
author : wassfila
date : 16.07.2016

Joystick cpp wrapper


*/

#include <linux/joystick.h>
#include <string>
#include <vector>

#define JOY_MAX_AXIS	20
#define JOY_MAX_BUTTONS	20


class JAxis
{
public:
	int		i_value;
	bool	update;
	int		time;
public:
	JAxis();
	bool	isUpdated();
	float	getValue();
};

class JButton
{
public:
	bool	b_value;
	bool	update;
	int		time;
public:
	JButton();
	bool	isUpdated();
	bool	getState();
	std::string	getStateText();
};

class Joystick
{
private:
	int fd;
	struct js_event jse;
	std::vector<JAxis>		Axes;
	std::vector<JButton> 	Buttons;
public:
	std::string 	Name;
public:
	void start(std::string device);
	void info();
	void printUpdates();
	bool update();
	void consumeAll();
	JAxis	&getAxis(int axisId);
	
};

