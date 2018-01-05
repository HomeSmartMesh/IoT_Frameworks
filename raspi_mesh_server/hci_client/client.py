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
def hci_loop(nb):
    while(nb > 0):
        sleep(0.05)
        mesh.run()
        nb = nb - 1
    return

def send_RGB_test():
    mesh.send_msg([8,0x70,0x0B,28,24,1,2,6])
    hci_loop(2)
    return
def send_msg_ping():
    print("send_msg_ping:")
    mesh.send_msg([5,0x20,0x01,28,24])
    hci_loop(2)
    return
def set_channel(chan):
    print("set_channel:")
    mesh.command("set_channel",[chan])
    hci_loop(2)
    return
def set_rx(func,val):
    print("set_rx:",func,val)
    mesh.command("set_rx",[mesh.set_rx[func],val])
    hci_loop(2)
    return
def set_response(val):
    print("set_response:",val)
    mesh.command("set_rx",[0x01,val])
    hci_loop(2)
    return
def test_rf(target,nb_ping):
    print("test_rf:",target)
    mesh.command("test_rf",[target,nb_ping])
    hci_loop(10)
    return
def config_retries(retries,delay):
    print("config_retries:")
    mesh.command("cfg_retries",[retries])
    hci_loop(2)
    print("config_ack_delay:")
    mesh.command("cfg_ack_delay",[0,delay])
    hci_loop(2)
    return
def send_msg_set_retries(nb):
    print("send_msg_set_retries:")
    mesh.send_msg([7,0x70,mesh.pid["exec_cmd"],28,24,mesh.exec_cmd["cfg_retries"],nb])
    hci_loop(2)
    return
def send_msg_set_channel(target,chan):
    print("send_msg_set_channel:")
    mesh.send_msg([7,0x70,mesh.pid["exec_cmd"],28,target,mesh.exec_cmd["set_channel"],chan])
    hci_loop(2)
    return

def test_target_channel(target,channel,nb_tests):
    print("----test_target_channel----")
    send_msg_set_channel(target,channel)
    set_channel(channel)
    test_rf(target,nb_tests)
    return
# -------------------- main -------------------- 
config = cfg.get_local_json()

cfg.configure_log(config["log"])

log.info("hci client started")

#will start a separate thread for looping
clientMQTT = mqtt_start(config,mqtt_on_message)

mesh.start(config)
config_retries(retries=2,delay=2)
set_channel(10)

#set_rx("bcast",0x01)
#set_rx("resp",0x01)
#send_msg_ping()
#send_RGB_test()
#send_msg_set_retries(2)

test_target_channel(target=24,channel=0,nb_tests=100)
test_target_channel(target=24,channel=40,nb_tests=100)
test_target_channel(target=24,channel=80,nb_tests=100)
test_target_channel(target=24,channel=120,nb_tests=100)

#set_channel(10)
#test_rf(24,10)

#loop forever
hci_loop_forever()
