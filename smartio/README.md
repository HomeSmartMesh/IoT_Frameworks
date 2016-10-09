# smartio
web based gui server and client for the Smart IO Expander Board
- Tested on windows
- Tested on Linux Raspberry Pi 3

## Nodejs server
The nodejs server controls the serial port and uses websockets to mirror the serial port tx and rx in text mode for the clients

## Web client
- The client connects to the server to send and receive text commands
- The client has also Leds GUI to control output with clicks that compute the command out of the desired led status
<img src="https://raw.githubusercontent.com/wassfila/smartio/master/client/printscreen.png" height="300">

## Firmware
[The IO Expander Firmware](https://github.com/wassfila/STM8_IoT_Base/tree/master/ws04_IOExpander_Sequencer/02_EESeqence_Console) 
converts the serial commands into PIO output status on or off.

##Printed Circuit Board
[The IO Expander Board wiki page](http://www.technolab.ddns.net/display/SSN/Smart+IO+Expander)

<img src="https://raw.githubusercontent.com/wassfila/smartio/master/firmware/SmartULN.JPG" height="300">
