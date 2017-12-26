import paho.mqtt.client as mqtt
import datetime
import logging as log
import cfg
from time import sleep,time
import json
import rules

# -------------------- mqtt events -------------------- 
def on_connect(lclient, userdata, flags, rc):
    log.info("mqtt connected with result code "+str(rc))
    for rule_name,rule in config["rules"].iteritems():
        log.info("Subscription for rule:%s %s -> %s",rule_name,rule["input"],rule["output"])
        lclient.subscribe(rule["input"])

def on_message(client, userdata, msg):
    topic_parts = msg.topic.split('/')
    for rule_name,rule in config["rules"].iteritems():
        if msg.topic == rule["input"]:
            if(rule["enable"]):
                #call the Fuction with the same name as the Rule 
                payload = getattr(rules,rule_name)()
                if(payload):
                    clientMQTT.publish(rule["output"],payload)


def ruler_loop_forever():
    while(True):
        sleep(10)
        config = cfg.get_local_json("config.json")
    return

def mqtt_start():
    clientMQTT = mqtt.Client()
    clientMQTT.on_connect = on_connect
    clientMQTT.on_message = on_message
    clientMQTT.connect(config["mqtt"]["host"], config["mqtt"]["port"], 3600)
    clientMQTT.loop_start()
    return clientMQTT

# -------------------- main -------------------- 
config = cfg.get_local_json("config.json")

cfg.configure_log(config["log"])

log.info("ruler started @ :"+str(datetime.datetime.utcnow()))

#will start a separate thread for looping
clientMQTT = mqtt_start()

#loop forever
ruler_loop_forever()
