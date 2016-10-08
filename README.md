# IoT Framework Projects

Complete working samples for specific use cases including
- STM8 firmware for a specific IoT Board out of the [STM8 IoT Boards](https://github.com/wassfila/STM8_IoT_Boards)
- Linux c++ applications on Raspberry Pi
- Nodejs servers for back end
- Javascript clients for front end

These projects are of intermediate level, for a beginning [STM8 IoT Hello World Samples](https://github.com/wassfila/STM8_IoT_HelloWorld) are available on a separate repository.

# Projects Overview
## workspace 01 : Weather Logger
ws 01 - Project 00 LibraryCommon	All the common files are now gathered in one library that is used for examples by both transmission and reception projects
ws 01 - Project 01 Temperature RF Sender	This currently combines the Hello Temperature by reading the DS18B20 single temperature sensor, and sending it in two bytes payloads over the nRF module
ws 01 - Project 02 RF Receive Logger	receives the two bytes payload expected for temperature, so converted to text of the float temperature and sent through the serial port
## workspace 02 : Colors Broadcast to RGB Led clients
## workspace 03 : Radio Frequency protocol for synchronisation (draft)
## workspace 04 : IO Expander Firmware
## workspace 05 : Ambient Light Logger
 - Mobile IoT Node firmware with driver for the MAX44009 i²c sensor
 - serial port logger on linux, with command line and config file parameters

