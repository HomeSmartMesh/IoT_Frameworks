from influxdb import InfluxDBClient
import datetime
from time import sleep
import socket 
import rasp

def raspi_loop_forever():
    while(True):
        tnow = datetime.datetime.utcnow()
        value = 0
        posts = [
            {
                "measurement": "cpu_load",
                "time": tnow,
                "tags":{
                    "host":hostname
                }
                "fields": {
                    "value": value
                }
            },
            {
                "measurement": "disk_space",
                "time": tnow,
                "tags":{
                    "host"  :hostname,
                    "target":"free"
                }
                "fields": {
                    "value": value
                }
            },
            {
                "measurement": "disk_space",
                "time": tnow,
                "tags":{
                    "host"  :hostname,
                    "target":"mongodb"
                }
                "fields": {
                    "value": value
                }
            }
        ]
        print(posts)
        #clientDB.write_points(posts)
        sleep(10)
    return

# -------------------- main -------------------- 
print("raspi client started @ :"+str(datetime.datetime.utcnow()))

hostname = socket.gethostname()

# -------------------- influxDB client -------------------- 
clientDB = InfluxDBClient(    "10.0.0.17", 
                            8086, 
                            'root', 'root', 
                            "raspiStatus")

#loop forever
#raspi_loop_forever()
while(True):
    print("cpu temp",rasp.getCPUtemperature())
    sleep(5)
