import paho.mqtt.client as mqtt
import pymongo
from pymongo import MongoClient
import datetime


def on_connect(lclient, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    lclient.subscribe("#")

def on_message(client, userdata, msg):
    topic_parts = msg.topic.split('/')
    if( (len(topic_parts) == 3) and (topic_parts[0] == "Nodes") ):
        nodeid = topic_parts[1]
        sensor = topic_parts[2]
        post = {
            "node": int(nodeid),
            sensor:float(msg.payload),
            "ts":datetime.datetime.utcnow()
        }
        post_id = sensors.insert_one(post).inserted_id
        print msg.topic+" "+str(msg.payload)+" posted @ "+str(post_id)
        

# -------------------- Mongo Client -------------------- 
client = MongoClient('mongodb://10.0.0.44:27017/')
db = client.HomeSmartMesh
sensors = db.sensors

# -------------------- Mqtt Client -------------------- 
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("10.0.0.12", 1883, 60)
client.loop_forever()

