import paho.mqtt.client as mqtt
import datetime
import logging as log
import cfg
from time import sleep

# -------------------- mqtt events -------------------- 
def on_connect(lclient, userdata, flags, rc):
    log.info("mqtt connected with result code "+str(rc))
    for rule_name,rule in config["rules"].iteritems():
        log.info(rule_name,rule["input"],rule["output"])
    #lclient.subscribe("#")

def on_message(client, userdata, msg):
    topic_parts = msg.topic.split('/')
    try:
        if( (len(topic_parts) == 3) and (topic_parts[0] == "Nodes") ):
            nodeid = topic_parts[1]
            sensor = topic_parts[2]
            measurement = "node"+nodeid
            value = float(str(msg.payload))
            post = [
                {
                    "measurement": measurement,
                    "time": datetime.datetime.utcnow(),
                    "fields": {
                        sensor: value
                    }
                }
            ]
            #TODO use post
            log.debug(msg.topic+" "+str(msg.payload)+" posted")
    except ValueError:
        log.error(" ValueError with : "+msg.topic+" "+str(msg.payload))


def ruler_loop_forever():
    while(True):
        topic = "Nodes/10/power"
        power = 1000
        #clientMQTT.publish(topic,power)
        #log.debug("%s: %s: %s",name, topic, power)
        sleep(10)
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
