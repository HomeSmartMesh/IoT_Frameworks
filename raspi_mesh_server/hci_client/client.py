import paho.mqtt.client as mqtt
import logging as log
import cfg
from time import sleep,time
import json
from mqtt_wrapper import mqtt_start
import socket
import mesh_wrapper as mesh

def mqtt_on_message(client, userdata, msg):
    log.info("%s : %s",msg.topic,msg.payload)
    return

def hci_loop_forever():
    while(True):
        sleep(0.1)
        mesh.run()
    return

# -------------------- main -------------------- 
config = cfg.get_local_json()

cfg.configure_log(config["log"])

log.info("hci client started")

#will start a separate thread for looping
clientMQTT = mqtt_start(config,mqtt_on_message)

mesh.start(config)

mesh.command("print_test")
mesh.command("print_nrf")
#loop forever
hci_loop_forever()
