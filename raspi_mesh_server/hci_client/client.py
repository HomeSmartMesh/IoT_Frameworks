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
    mesh.send_msg([8,0x70,0x0B,28,24,0,0,3])
    hci_loop(2)
    return
def send_request_ping():
    print("send_request_ping:")
    mesh.send_msg([5,0x20,0x01,28,24])
    hci_loop(2)
    return
def send_msg_ping():
    print("send_msg_ping:")
    mesh.send_msg([5,0x70,0x01,28,24])
    hci_loop(2)
    return
def set_channel(chan):
    print("set_channel:",chan)
    mesh.command("set_channel",[chan])
    hci_loop(2)
    mesh.command("get_channel",[])
    hci_loop(2)
    return
def get_channel():
    mesh.command("get_channel",[])
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

def test_channel(target,channel,nb_ping=100):
    """
    Tests the RF connection between the 'hci' node and another 'target' node
    sends a single serial request with :
    - 'target' : the node that will receive the pings and sends back ack
    - 'channel' : frequency channel, from 0 (2.4 GHz) till 125 (2.525 GHz)
    - 'nb_ping' : usually 100 for a significant signal quality estimation
    """
    print("RF Test O->%d, Chan %d" % (target,channel))
    mesh.command("test_rf",[target,channel,nb_ping])
    hci_loop(20)
    return
def remote_test_channel(remote,test_target,channel,nb_ping=100):
    """
    Tests the RF connection between any 'remote' node and another 'test_target' node
    sends an RF message to execute an RF test command:
    - 'remote' : any node from the mesh network that will be sending pings
    - 'test_target' : receiver of the test pings and sender of ack
    - 'channel' : frequency channel, from 0 (2.4 GHz) till 125 (2.525 GHz)
      note that the remote will request the 'test_target' to switch to the test RF and then back
      condition is that the remote and the test_target are initially in the same channel, as there is no search
    - 'nb_ping' : usually 100 for a significant signal quality estimation
    """
    print("Remote RF Test %d->%d, Chan %d" % (remote,test_target,channel))
    mesh.send_msg([7,0x70,mesh.pid["exec_cmd"],28,remote,mesh.exec_cmd["test_rf"],test_target,channel,nb_ping])
    hci_loop(40)
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

#enable to hear feedback of remote test rf request
set_rx("msg",0x01)

remote_test_channel(remote=24,test_target=28,channel=10,nb_ping=100)

#set_rx("bcast",0x01)
#set_rx("resp",0x01)
#send_msg_ping()
#send_RGB_test()
#send_msg_set_retries(2)

#send_msg_set_channel(24,10)
#set_channel(10)

#test_channel(target=24,channel=10,nb_ping=10)

#test_channel(target=24,channel=40,nb_ping=10)

#set_channel(10)
#send_msg_ping()

#set_channel(40)
#send_msg_ping()


#set_channel(10)
#test_rf(24,10)

#loop forever
#hci_loop_forever()
