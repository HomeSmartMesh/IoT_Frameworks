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

def send_RGB_test():
    mesh.send_msg([8,0x70,0x0B,28,24,1,2,6])
    return
def send_ping_test():
    print("send_ping_test:")
    mesh.send_msg([5,0x20,0x01,28,24])
    return
def set_channel(chan):
    print("set_channel:")
    mesh.command("set_channel",[chan])
    return
# -------------------- main -------------------- 
config = cfg.get_local_json()

cfg.configure_log(config["log"])

log.info("hci client started")

#will start a separate thread for looping
clientMQTT = mqtt_start(config,mqtt_on_message)

mesh.start(config)

#mesh.command("get_status")

#send_RGB_test()

set_channel(10)
sleep(1)
send_ping_test()

#loop forever
hci_loop_forever()
