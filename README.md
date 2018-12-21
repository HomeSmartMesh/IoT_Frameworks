# [website with overview, tutorials, mind maps,...](https://www.homesmartmesh.com/)

# [More recent github for Raspberry Pi scripts and nRF52](https://github.com/nRFMesh/nRF52_Mesh)

# Status
This repo is no longer in active maintenance, its two main topics are :
* raspberry pi scripts and services, moved to the nRF52 repo
* STM8 and STM32 based firmwares that are now replaced by [nRF52](https://github.com/nRFMesh/nRF52_Mesh) abd [ESP32_iot](https://github.com/HomeSmartMesh/esp32_iot)

# Hardware
### Wireless Sensor Tag
  * HW: [STM8 IoT Boards](https://github.com/HomeSmartMesh/STM8_IoT_Boards)
  * Firmware: [IoT STM8 ](https://github.com/HomeSmartMesh/IoT_STM8/tree/master/firmware_rf_sensors_node)
  * 8 bit Ultra low power & ultralow cost STM8L
  * BME280 (Pressure, Temperature, Humidity), MAX44009 (Ambient Light) and Magnetic switches
  * Custom RF protocol for efficiency and simplicity of the sensors usecase
  <img src="https://github.com/wassfila/media/blob/master/IoT_Node_Mobile_v2.JPG" height=300>

### STM32 IoT Dongle
  * based on the STM32 bluepill
  * HW: Breadboard for easier RF and sensors modules integration [STM32 IoT Boards](https://github.com/wassfila/STM32_IoT_Boards)
  * Firmware: [stm32_rf_dongle subdirectory](https://github.com/HomeSmartMesh/IoT_Frameworks/tree/master/stm32_rf_dongle)
  * RF Mesh repeater: (decrement time to live, recalculate new CRC)
  * Light intensity, color sensor, proximity and gesture (APDS9960)
  * RGB status (WS2812B)
  <img src="https://github.com/wassfila/media/blob/master/STM32_Dongle.png" height=300>


# Software running on the Raspberry pi
  <img src="https://github.com/wassfila/media/blob/master/IoTSoftware.png" height=500>

### C++ RF Gateway
[submodule rf_gateway](https://github.com/HomeSmartMesh/rf_gateway)
- Serial port line buffer
- Line parsing and sensor data extraction
- Adjustment of BME280 measures with provided calibration
- Publishing to the MQTT broquer

### Python Database proxies
scripts in the [submodule py_db_proxy](https://github.com/HomeSmartMesh/py_db_proxy)
- influx client
- mongo client
- json config and loggging

### React D3js webapp
[submodule web_react](https://github.com/HomeSmartMesh/web_react)
current status :
* automatic nodes config retriaval 
* automatic dashboard construction (but still in text mode)
* d3js still in a separate project, options beeing evaluated (nvd3 and vegas)

### Configuration
Configuration for the following software is available in the [config subdirectory](https://github.com/HomeSmartMesh/IoT_Frameworks/tree/master/config)

 * Mosquitto : MQTT Broquer
 * OpenHAB2 : used only for Wemo switch and other connected devices
 * Node-red : High level sensor to action configuration
 * Influxdb : time series database
 * Mongodb : complex documents database
 * Grafana : dashboard for measurements exploration
 * React+d3js : customised webapp

## Startup Launch on the Raspberry pi
to enable once
```
sudo systemctl enable openhab2
sudo systemctl enable influxdb
```

Others require manual startup added in
```
sudo nano /etc/rc.local
```
Debugging the rc.local log with adding the following lines
```
exec 2> /home/pi/share/rc.local.log   # send stderr from rc.local to a log file
exec 1>&2                             # send stdout to the same log file
set -x                                # tell sh to display commands before execution
```


```bash
#grafana
sudo systemctl start grafana-server.service
#iot_db
/home/pi/IoT_Frameworks/raspi_mesh_server/iot_db/iot_db &
#rf_gateway
sleep 2 && /home/pi/IoT_Frameworks/raspi_mesh_server/rf_gateway/gateway &
#mongodb : for some reason, mongodb service do not attempt reconnection on startup, but works when started a bit later
sleep 15 && sudo systemctl start mongodb.service
#mqtt_mongo
sleep 5 && python /home/pi/IoT_Frameworks/raspi_mesh_server/py_db_proxy/mqtt_mongo.py &
sleep 1 && python /home/pi/IoT_Frameworks/raspi_mesh_server/py_net_gateway/wemo_client.py &
sleep 5 && python /home/pi/IoT_Frameworks/raspi_mesh_server/py_net_gateway/milight_gateway.py &

```

Another example

```
/home/pi/IoT_Frameworks/raspi_mesh_server/rf_gateway/gateway &

python /home/pi/IoT_Frameworks/raspi_mesh_server/py_net_gateway/wemo_client.py &

python /home/pi/IoT_Frameworks/raspi_mesh_server/py_net_gateway/milight_gateway.py &

python /home/pi/IoT_Frameworks/raspi_mesh_server/ruler/ruler.py &

python /home/pi/IoT_Frameworks/raspi_mesh_server/py_net_gateway/raspi_status.py &

sleep 15 && python /home/pi/IoT_Frameworks/raspi_mesh_server/py_db_proxy/influx_client.py &
```


### Protocol helper
0x05 : Alive
0x07 : light
0x0A : bme280

0x06 : 6 - Living room
0x0F : 15- Workplace
0x1A : 26- Sleeping room
0x1D : 29
0x26 : 38- Rep test
