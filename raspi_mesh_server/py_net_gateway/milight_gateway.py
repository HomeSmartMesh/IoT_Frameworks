#https://pypi.python.org/pypi/paho-mqtt/1.1
import paho.mqtt.client as mqtt
import json
#https://github.com/McSwindler/python-milight
import milight
from milight import Command
#just to get host name
import socket 
from time import sleep
from math import ceil
import logging


def on_connect(lclient, userdata, flags, rc):
    topic_sub = ml["mqtt_client"]["valueActions"]["HeadTopic"] + "+/dimmer"
    lclient.subscribe(topic_sub)
    print("Subscribed to: "+topic_sub)

def on_message(client, userdata, msg):
    topic_parts = msg.topic.split('/')
    if(len(topic_parts) == 3): # no need to check 'Actions',...
        nodeid = topic_parts[1]
        if(nodeid in ml["mapping"]):
            device_name = ml["mapping"][nodeid]["device"]
            channel = ml["mapping"][nodeid]["channel"]
            dimm_val = int(ceil(float(msg.payload)))
            if(dimm_val > 100):
                dimm_val = 100
            print(  "Action to Node: "+nodeid               +
                    " ; through gateway: "+device_name      +
                    " ; on channel: "+str(channel)            +
                    " ; set value: "+str(dimm_val)
                    )
            controller = ml["devices"][device_name]["controller"]
            if(dimm_val == 0):
                controller.send(light.off(channel))
            elif(dimm_val == 1):
                controller.send(light.off(channel))
                sleep(0.100)#100 ms
                controller.send(Command(night_mode[channel]))
            else:
                controller.send(light.brightness(dimm_val,channel))
        else:
            print("Node "+nodeid+" route unknown")
    else:
        print("topic: "+msg.topic + "size not matching")
        

ml = json.load(open('config_milight.json'))

# -------------------- logging -------------------- 
logging.basicConfig(filename=ml["log"]["logfile"],level=ml["log"]["level"])

# -------------------- Milight Client -------------------- 
for key,device in ml["devices"].items():
    device["controller"] = milight.MiLight(device)

light = milight.LightBulb(['rgbw','white','rgb'])
night_mode = [0xC1, 0xC6, 0xC8, 0xCA, 0xCC]
# -------------------- Mqtt Client -------------------- 
host = ml["mqtt_client"]["host"]
port = ml["mqtt_client"]["port"]
client_id = ml["mqtt_client"]["client_id"] +"_"+socket.gethostname()
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect(host, port)
print(  "mqtt connected to "+host+":"+str(port)+" with id: "+ client_id )
client.loop_forever()

