from influxdb import InfluxDBClient
import datetime
from time import sleep
import socket 
import requests
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
                },
                "fields": {
                    "value": float(rasp.getCPU_Avg1min())
                }
            },
            {
                "measurement": "cpu_temp",
                "time": tnow,
                "tags":{
                    "host":hostname
                },
                "fields": {
                    "value": float(rasp.getCPUtemperature())
                }
            },
            {
                "measurement": "disk",
                "time": tnow,
                "tags":{
                    "host"  :hostname,
                    "target":"total"
                },
                "fields": {
                    "value": int(rasp.getUsedDisk())
                }
            },
            {
                "measurement": "disk",
                "time": tnow,
                "tags":{
                    "host"  :hostname,
                    "target":"mongodb"
                },
                "fields": {
                    "value": int(rasp.getUsedDiskDir("/var/lib/mongodb"))
                }
            },
            {
                "measurement": "ram",
                "time": tnow,
                "tags":{
                    "host"  :hostname
                },
                "fields": {
                    "value": int(rasp.getUsedRAM())
                }
            }
        ]
        try:
            clientDB.write_points(posts)
        except requests.exceptions.ConnectionError:
            print("ConnectionError sample skipped @ "+str(datetime.datetime.utcnow()))
        sleep(60)
    return

# -------------------- main -------------------- 
print("raspi client started @ :"+str(datetime.datetime.utcnow()))

hostname = socket.gethostname()

# -------------------- influxDB client -------------------- 
clientDB = InfluxDBClient(    "10.0.0.12", 
                            8086, 
                            'root', 'root', 
                            "raspiStatus")

#loop forever
raspi_loop_forever()
#while(True):
    #print("cpu temp",float(rasp.getCPUtemperature()) )
    #print("used ram",int(rasp.getUsedRAM()) )
    #print("cpu use",rasp.getCPU_Avg1min() )
    #print("used disk /",int(rasp.getUsedDisk()) )
    #print("used disk mongodb /",int(rasp.getUsedDiskDir("/var/lib/mongodb")) )
    #sleep(5)
