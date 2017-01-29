# IoT Frameworks
Complete working samples for specific use cases including
- **Sensors firmware** : for a specific IoT Board such as the [STM8 IoT Boards](https://github.com/wassfila/STM8_IoT_Boards)
- **Server Drivers** : Linux c++ applications on Raspberry Pi
- **Server Back end** : Nodejs servers for back end
- **Client Front End** : Javascript clients for front end

These projects are of intermediate level, for a beginning [STM8 IoT Hello World Samples](https://github.com/wassfila/STM8_IoT_HelloWorld) are available on a separate repository.

# Sensors Logger
<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/sensors_logger/IoT_Infrastructure.png" height=400>

## Webclient screenshot
<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/sensors_logger/webclient.png" height=400>

###Javascript easy to use configuration API
```javascript
var now = Date.now();
var start_disp = now - 24*60*60*1000;//Get the last day of measures
var ChartsParamsList = 
[
	{
		svgID : "#ChartLivTemp",
		scale_x_domain : [start_disp,now],
		scale_y_domain : [10,25],
		NodeId : 6,
		SensorName : "Temperature",
		Label : "Livingroom Temperature °C"
	},
	{
		svgID : "#ChartLivHum",
		scale_x_domain : [start_disp,now],
		scale_y_domain : [0,100],
		NodeId : 6,
		SensorName : "Humidity",
		Label : "Livingroom Humidity %RH"
	},
 ...
```

## Firmware for the Mobile Node
Here have firmware for the Mobile Node which is the base for the sensors:
 - *BME280* Temperature Humidity Pressure
 - *MAX44009* Ambient Light
 
More details on [Home Smart Mesh](http://www.homesmartmesh.com)

<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/sensors_logger/IoT_Node_Mobile_v2.JPG" height=300>

## Raspberry Pi : C++ Sensors Server
- Serial port reader
- Conversion of sensors registers to values through calibration parameters for BME280
- Database manager : files and memory management of binary data, text std::string request and response
- websocket manager : handles connections with clients for both instant updates and time series request response.

### Environment and Dependencies
- Scons : a single line for the whole compilation, linker stuf,...
- C++11 : Modern c++ is easier than javascript (the luxury of having a compiler)
- Boost 1.60 (filesystem) : A warm welcome to Boost on RPI (see install instructions)
- json for modern c++ : yes, json as easy to use as in javascript https://github.com/nlohmann/json
- C++ websocket client : https://github.com/dhbaird/easywsclient

### Boost installation on the Raspberry pi
Minimal install for filesystem support:
```
mkdir boost
cd boost
wget http://sourceforge.net/projects/boost/files/boost/1.60.0/boost_1_60_0.tar.bz2
tar xvfo boost_1_60_0.tar.bz2
cd boost_1_60_0
./bootstrap.sh --with-libraries=filesystem,system
sudo ./b2 install
```
# Mesh Controller Interface
## Commands help and Protocol details
[Technolab/Mesh Controller Interface wiki](http://www.technolab.ddns.net/display/SSN/Mesh+Controller+Interface)

## Configuration Steps
<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/mesh_controller_interface/configuration_steps.png" height=300>

## Configuration Options
<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/mesh_controller_interface/configuration_options.png" height=300>


# Smartio
Control any thing (up to 48V 500mA) with a smartphone. Both offline, with Raspberry Pi as a wifi access point, or even easier when from internet (Beyond scopes of Router Ports and Security aspects).
More under the [smartio directory](https://github.com/wassfila/IoT_Frameworks/tree/master/smartio).

<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/smartio/WebIO.jpg" height=300>

# Joystick Servo
Control a servo motor using a PS4 controller linked wirelessly to a Raspberry Pi that has a [servo board](https://github.com/wassfila/STM8_IoT_Boards/tree/master/ServoMotors_Controller).
Configuration steps are provided on how to use the PS4 on the Pi.

<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/joystick_servo/ServoController.JPG" height=300>
