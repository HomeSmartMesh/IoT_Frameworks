
#include <linux/joystick.h>

void joy_process(struct js_event jse);
void joy_info(int fd);
int joy_open(char *device_name);