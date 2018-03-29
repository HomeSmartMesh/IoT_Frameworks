# Fitness Trackers

Device e4:ca:1e:32:9a:d4 (random), RSSI=-64 dB
  Complete 128b Services = d0002d121e4b0fa4994eceb5f0ff0579
  Flags = 06
  Complete 16b Services = e7fe
  Complete Local Name = T93proCAE4
  Manufacturer = 3412fee7e4ca1e329ad4

Service         : 6e40fff0-b5a3-f393-e0a9-e50e24dcca9e
Firmware update : 00001530-1212-efde-1523-785feabcd123
Service         : 0000fee7-0000-1000-8000-00805f9b34fb
Service         : de5bf728-d711-4e47-af26-65e3012a5dc7

Device d8:e8:e8:39:2a:3d (random), RSSI=-72 dB
  Appearance = 0000
  Complete 16b Services = f00a
  Complete Local Name = ID115Plus HR
  Manufacturer = 45020000d8e8e8392a3d8f
  Flags = 05

Service :
00000af0-0000-1000-8000-00805f9b34fb
Characteristics :
00000af7-0000-1000-8000-00805f9b34fb
00000af2-0000-1000-8000-00805f9b34fb
00000af1-0000-1000-8000-00805f9b34fb


sudo hciconfig hci0 down
sudo hciconfig hci0 up

sudo hcitool lescan

sudo hcitool lecc D8:E8:E8:39:2A:3D

sudo hcitool lecc 98:4F:EE:0D:06:1A

gatttool -b D8:E8:E8:39:2A:3D --interactive
gatttool -b E4:CA:1E:32:9A:D4 --interactive
gatttool -t random -b D8:E8:E8:39:2A:3D --interactive
gatttool -t random -b E4:CA:1E:32:9A:D4 --interactive



pi@ioserv:~/IoT_Frameworks/bluetooth $ gatttool -b d8:e8:e8:39:2a:3d --interactive
[d8:e8:e8:39:2a:3d][LE]> connect
Attempting to connect to d8:e8:e8:39:2a:3d
[d8:e8:e8:39:2a:3d][LE]> connect
Error: connect error: Connection refused (111)
