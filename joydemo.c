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

void process_event(struct js_event jse) 
{
	if (jse.type == JS_EVENT_AXIS) 
	{
		if (jse.number == 0) 
		{
			if (jse.value < 0) 
			{
				printf("LEFT\n");
			} else if (jse.value > 0) 
			{
				printf("RIGHT\n");
			}
		} else 
		{
			if (jse.value < 0) 
			{
				printf("UP\n");
			} else if (jse.value > 0) 
			{
				printf("DOWN\n");
			}
		}
	}

	if (jse.type == 1 && jse.value > 0) 
	{
		printf("%d\n", jse.number);
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
			process_event(jse);
		}
	}

	return 0;
}