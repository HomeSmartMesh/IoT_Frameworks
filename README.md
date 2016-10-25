# IoT Frameworks
Complete working samples for specific use cases including
- **Sensors firmware** : for a specific IoT Board such as the [STM8 IoT Boards](https://github.com/wassfila/STM8_IoT_Boards)
- **Server Drivers** : Linux c++ applications on Raspberry Pi
- **Server Back end** : Nodejs servers for back end
- **Client Front End** : Javascript clients for front end

These projects are of intermediate level, for a beginning [STM8 IoT Hello World Samples](https://github.com/wassfila/STM8_IoT_HelloWorld) are available on a separate repository.

# Sensors Logger
## Firmware for the Mobile Node
Here have firmware for the Mobile Node which is the base for the sensors:
 - *BME280* Temperature Humidity Pressure
 - *MAX44009* Ambient Light
 
More details on the [githubio](https://wassfila.github.io/#P2)

<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/sensors_logger/IoT_Node_Mobile_v2.JPG" height=300>
## Raspberry Pi Serial logger
 - C++ Linux based serial port application
 - While at this stage using any serial port logger would do, it is planned to have combined Data/Text connection for which these provided components will be usefull.

# Smartio
Control any thing (up to 48V 500mA) with a smartphone. Both offline, with Raspberry Pi as a wifi access point, or even easier when from internet (Beyond scopes of Router Ports and Security aspects).
More under the [smartio directory](https://github.com/wassfila/IoT_Frameworks/tree/master/smartio).

<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/smartio/WebIO.jpg" height=300>

# Joystick Servo
Control a servo motor using a PS4 controller linked wirelessly to a Raspberry Pi that has a [servo board](https://github.com/wassfila/STM8_IoT_Boards/tree/master/ServoMotors_Controller).
Configuration steps are provided on how to use the PS4 on the Pi.

<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/joystick_servo/ServoController.JPG" height=300>

# Other Helper Projects
## workspace 01 : Weather Logger
ws 01 - Project 00 LibraryCommon	All the common files are now gathered in one library that is used for examples by both transmission and reception projects
ws 01 - Project 01 Temperature RF Sender	This currently combines the Hello Temperature by reading the DS18B20 single temperature sensor, and sending it in two bytes payloads over the nRF module
ws 01 - Project 02 RF Receive Logger	receives the two bytes payload expected for temperature, so converted to text of the float temperature and sent through the serial port
## workspace 02 : Colors Broadcast to RGB Led clients
## workspace 03 : Radio Frequency protocol for synchronisation (draft)
