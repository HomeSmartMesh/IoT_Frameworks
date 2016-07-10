#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>


int joy_open(char *device_name) 
{
	int fd = -1;

	if (device_name == NULL) 
	{
		return fd;
	}

	fd = open(device_name, O_RDONLY | O_NONBLOCK);

	if (fd < 0) 
	{
		printf("Could not locate joystick device %s\n", device_name);
		exit(1);
	}

	return fd;
}

void joy_info(int fd) 
{
	int axes=0, buttons=0;
	char name[128];

	ioctl(fd, JSIOCGAXES, &axes);
	ioctl(fd, JSIOCGBUTTONS, &buttons);
	ioctl(fd, JSIOCGNAME(sizeof(name)), &name);

	printf("%s\n  %d Axes %d Buttons\n", name, axes, buttons);
}

void joy_process(struct js_event jse) 
{
	if (jse.type == JS_EVENT_AXIS) 
	{
		float v = jse.value;
		v = v / 32767;
		printf("Axis %d @ %0.2f\n",jse.number,v);
	}
	else if (jse.type == JS_EVENT_BUTTON && jse.value > 0) 
	{
		printf("Button %d\n", jse.number);
	}
}

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