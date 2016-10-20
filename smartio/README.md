## SmartIO a Raspberry Pi 3
- RPi 3 wifi used to provide an access point with "hostapd" and "dnsmasq", see this nice [tutorial](https://frillip.com/using-your-raspberry-pi-3-as-a-wifi-access-point-with-hostapd/)
- RPi 3 serves the javascript web client with ["apache2"](https://www.raspberrypi.org/documentation/remote-access/web-server/apache.md)
- connect on "http://172.24.1.1/client/"
<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/smartio/WebIO.jpg" height=300>

## Printed Circuit Board
[The IO Expander Board wiki page](http://www.technolab.ddns.net/display/SSN/Smart+IO+Expander)

<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/smartio/firmware/SmartULN.JPG" height="300">

## Firmware
[The IO Expander Firmware](https://github.com/wassfila/STM8_IoT_Base/tree/master/ws04_IOExpander_Sequencer/02_EESeqence_Console) 
converts the serial commands into PIO output status on or off.

## Web client
- The client connects to the server to send and receive text commands
- The client has also Leds GUI to control output with clicks that compute the command out of the desired led status
<img src="https://github.com/wassfila/IoT_Frameworks/blob/master/smartio/client/printscreen.png" height="300">

## Nodejs server
The nodejs server controls the serial port and uses websockets to mirror the serial port tx and rx in text mode for the clients.
Note that a [new version of nodejs](http://node-arm.herokuapp.com/node_latest_armhf.deb) has to be used (v4.x.x) for the serial_port component to be installed with npm.

