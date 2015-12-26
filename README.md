# STM8_IoT_Base
Concrete working samples for specific use cases
For preliminery environment set up and testing please check the Hello world samples first:
https://github.com/wassfila/STM8_IoT_HelloWorld

The IoT node is an STM8 based low cost development platform.
It is an initiative for an open product = open hardware + open source project.
find all the details in this link :
http://www.homesmartmesh.com/mediawiki/index.php/IoT_Node
This project will be incremented gradually to cover more IoT Applications such as :
 - Weather and ambient sensors : temperature, humidity, air quality, Ambient light.
 - User interfaces : RGB Leds, Gyro / Accel, user gesture.

This IoT Node is part of a bigger project that extand the collaboration to a complete IoT environment.
It includes  interfaces to existing products and server applications for merging everything together.
The "Home Smart Mesh" concept is described in the main wiki page
http://www.homesmartmesh.com/mediawiki/index.php/Main_Page

Samples brief description :
workspace 01 : Weather Logger
ws 01 - Project 00 LibraryCommon	All the common files are now gathered in one library that is used for examples by both transmission and reception projects
ws 01 - Project 01 Temperature RF Sender	This currently combines the Hello Temperature by reading the DS18B20 single temperature sensor, and sending it in two bytes payloads over the nRF module
ws 01 - Project 02 RF Receive Logger	receives the two bytes payload expected for temperature, so converted to text of the float temperature and sent through the serial port
