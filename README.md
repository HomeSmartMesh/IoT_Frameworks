# IoT Frameworks
Complete working samples ready to use on the field
* **Wireless Sensors** : based on the STM8 IoT Boards [STM8 IoT Boards](https://github.com/wassfila/STM8_IoT_Boards)
  * 8 bit Ultra low power & ultralow cost STM8L
  * BME280 (Pressure, Temperature, Humidity), MAX44009 (Ambient Light) and Magnetic switches
  * Custom RF protocol for efficiency and simplicity of the sensors usecase
* **MQTT and OpenHAB2 bindings** : Fully inetgrated with the most popular IoT Ecosystems 
  * Publishes MQTT topics per sensors
  * Subscribe to MQTT topics to control custom RF devices
  * subscribes to other topics coming from On the Shelf IoT devices
* **C++ Application** : Linux c++ app running on the Raspberry Pi 
  * Network with Poco for HTTP server and websockets
  * MQTT cpp client : publish and subscribes to Mosquitto broquer
  * Custom Persistence : performance Databse (ROM/RAM mirror) optimised for requests handling with minimal latency and maximum memory efficiency
  * serial port : buffer management, receptions and lines parsing, sending commands to the RF dongle
* **Webclient Front End with d3js** 
  * Real time Javascript app with server update through websocket
  * Custom charts and Panels with simple configuration and open to further d3js customisations

# IoT Environment

<img src="https://github.com/wassfila/media/blob/master/IoT_Hardware.PNG" height=400>

<img src="https://github.com/wassfila/media/blob/master/IoT_Software.PNG" height=400>

## Webclient screenshot
<img src="https://github.com/wassfila/media/blob/master/webclient.png" height=400>

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

## IoT Data Flow
<img src="https://github.com/wassfila/media/blob/master/IoT_Dataflow.png" height=400>

## Firmware for the Sensors Node
Here have firmware for the Mobile Node which is the base for the sensors:
 - *BME280* Temperature Humidity Pressure
 - *MAX44009* Ambient Light

[STM8L Mobile Node wiki page](http://technolab.ddns.net/display/SSN/STM8L+Mobile+Node)


<img src="https://github.com/wassfila/media/blob/master/IoT_Node_Mobile_v2.JPG" height=300>

## Raspberry Pi : C++ Sensors Server
- Serial port reader
- Conversion of sensors registers to values through calibration parameters for BME280
- Database manager : files and memory management of binary data, text std::string request and response
- websocket manager : handles connections with clients for both instant updates and time series request response.

### Environment and Dependencies
- Scons : a single line for the whole compilation, linker stuf,...
- C++11 : Modern c++ is easier than javascript (the luxury of having a compiler)
- Boost 1.60 (filesystem) : A warm welcome to Boost on RPI (see install instructions)
- Poco-1.7.7 : HTTP server&client, websocket server : https://pocoproject.org/
- json for modern c++ : json as easy to use as in javascript https://github.com/nlohmann/json (to be replaced by Poco::JSON)

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
___
# Other misc Projects : 
## Mesh Controller Interface
### Commands help and Protocol details
[Technolab/Mesh Controller Interface wiki](http://www.technolab.ddns.net/display/SSN/Mesh+Controller+Interface)

### Configuration Steps
<img src="https://github.com/wassfila/media/blob/master/configuration_steps.png" height=300>

### Configuration Options
<img src="https://github.com/wassfila/media/blob/master/configuration_options.png" height=300>


## Smartio
Control any thing (up to 48V 500mA) with a smartphone. Both offline, with Raspberry Pi as a wifi access point, or even easier when from internet (Beyond scopes of Router Ports and Security aspects).
More under the [smartio directory](https://github.com/wassfila/IoT_Frameworks/tree/master/smartio).

<img src="https://github.com/wassfila/media/blob/master/WebIO.jpg" height=300>

## Joystick Servo
Control a servo motor using a PS4 controller linked wirelessly to a Raspberry Pi that has a [servo board](https://github.com/wassfila/STM8_IoT_Boards/tree/master/ServoMotors_Controller).
Configuration steps are provided on how to use the PS4 on the Pi.

<img src="https://github.com/wassfila/media/blob/master/ServoController.JPG" height=300>
