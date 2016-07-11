* simple demo for joystick usage on Linux
sample based on 
https://www.kernel.org/doc/Documentation/input/joystick-api.txt

* Note that target platform here is :
 - PS4 Wireless controller : used with USB
 - Raspberry pi 3

* environment setup:
sudo apt-get install joystick

* evironment test
"ls /dev/input"
added "event2 js0"

ls /dev/input/by-id/
added "Usb-Sony_Computer_Entertainment_Wireless_Controller-event-joystick" and " -joystick only"

* compilation
"gcc joydemo.c"

* Launching the newly generated file
"./a.out"
