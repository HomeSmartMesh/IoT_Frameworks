import sys
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

def loop_forever():
    while(True):
        sleep(0.1)
        mesh.run()
    return
def loop(nb):
    while(nb > 0):
        sleep(0.05)
        mesh.run()
        nb = nb - 1
    return
def readreg(reg):
    mesh.command("readreg",[reg])
    loop(2)
    return
def writereg(reg,val):
    print("write in reg 0x%02X val:%u"%(reg,val))
    mesh.command("writereg",[reg,val])
    loop(2)
    return

def send_RGB_test():
    mesh.send_msg([8,0x70,0x0B,node_id,24,0,0,3])
    loop(2)
    return

def ping(target,ttl=0):
    print("send msg ping:")
    control = 0x70 | ttl
    mesh.send_msg([5,control,0x01,node_id,target])
    loop(10)
    return

def set_mode(mode_txt):
    print("set_mode:",mode_txt)
    mesh.command("set_mode",[mesh.mode[mode_txt]])
    loop(2)
    return
def get_mode():
    print("get_mode:")
    mesh.command("get_mode")
    loop(2)
    return
def set_channel(chan):
    print("set_channel:",chan)
    mesh.command("set_channel",[chan])
    loop(2)
    mesh.command("get_channel",[])
    loop(2)
    return
def get_channel():
    mesh.command("get_channel",[])
    loop(2)
    return
def remote_set_channel(target,chan):
    print("remote_set_channel:")
    mesh.send_msg([7,0x70,mesh.pid["exec_cmd"],node_id,target,mesh.exec_cmd["set_channel"],chan])
    loop(2)
    return

def set_rx(func,val):
    print("set_rx:",func,val)
    mesh.command("set_rx",[mesh.set_rx[func],val])
    loop(2)
    return

def set_retries(retries,delay):
    print("set_retries:")
    mesh.command("cfg_retries",[retries])
    loop(2)
    print("config_ack_delay:")
    mesh.command("cfg_ack_delay",[0,delay])
    loop(2)
    return
def remote_set_retries(remote,retries,delay):
    print("remote_set_retries:")
    mesh.send_msg([7,0x70,mesh.pid["exec_cmd"],node_id,remote,mesh.exec_cmd["cfg_retries"],retries])
    loop(2)
    mesh.send_msg([8,0x70,mesh.pid["exec_cmd"],node_id,remote,mesh.exec_cmd["cfg_ack_delay"],0,delay])
    loop(2)
    return

def test_channel(target,channel,nb_ping=100):
    """
    Tests the RF connection between the 'hci' node and another 'target' node
    sends a single serial request with :
    - 'target' : the node that will receive the pings and sends back ack
    - 'channel' : frequency channel, from 0 (2.4 GHz) till 125 (2.525 GHz)
    - 'nb_ping' : usually 100 for a significant signal quality estimation
    """
    set_rx("msg",1)
    set_rx("bcast",0)
    set_rx("resp",1)
    print("RF Test O->%d, Chan %d" % (target,channel))
    mesh.command("test_rf",[target,channel,nb_ping])
    loop(20)
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
    set_rx("msg",1)
    set_rx("bcast",0)
    set_rx("resp",1)
    print("Remote RF Test %d->%d, Chan %d" % (remote,test_target,channel))
    mesh.send_msg([9,0x70,mesh.pid["exec_cmd"],node_id,remote,mesh.exec_cmd["test_rf"],test_target,channel,nb_ping])
    loop(40)
    return

# ------------------------ test examples ----------------------------
def test1():
    #enable to hear feedback of remote test rf request
    set_rx("msg",0x01)
    set_rx("bcast",0x01)
    set_rx("resp",0x01)
    ping(24)
    send_RGB_test()
    send_msg_set_retries(2)

    set_channel(10)
    ping(24)

    send_msg_set_channel(24,10)
    set_channel(10)

    test_channel(target=24,channel=10,nb_ping=10)
    test_channel(target=24,channel=40,nb_ping=10)

    remote_test_channel(remote=24,test_target=node_id,channel=10,nb_ping=100)

    #loop forever
    loop_forever()
    return
def listen(chan):
    set_retries(retries=2,delay=2)
    set_channel(chan)
    set_rx("msg",0x01)
    set_rx("bcast",0x01)
    set_rx("resp",0x01)
    set_mode("rx")
    loop_forever()
    return
def test_rf_remote():
    #First select the channel of the remote node, it is assumed to be known
    #as there is no scan to guess on which channel is the remote listening
    set_channel(10)
    #enable the reception of messages logs, particularly the test_rf_resp message
    set_rx("msg",1)
    #important as the default delay to wait for ack is designed for mesh ~ 100 ms while 
    #an a ping ack sent back from the stack on point to point can be achieved with a delay of 2 ms
    remote_set_retries(remote=24,retries=5,delay=2)
    #the test will be run between the nodes 'remote' and 'test_target', where 'remote' is
    #sending pings and 'test_target' sedning acks
    # the result will be sent back to the test requester node
    remote_test_channel(remote=24,test_target=27,channel=10)
    return

# -------------------- main -------------------- 
config = cfg.get_local_json()

cfg.configure_log(config["log"])

log.info("hci client started")

#will start a separate thread for looping
clientMQTT = mqtt_start(config,mqtt_on_message)

if(len(sys.argv)>=2):
    port = sys.argv[1]
    config["serial"]["port"] = port

mesh.start(config)

chan = 2

node_id = 22

if(len(sys.argv)>=3):
    node_id = int(sys.argv[2])
if(len(sys.argv)>=4):
    chan = int(sys.argv[3])

print("HCI Node %d on chan %d" % (node_id,chan))
set_rx("msg",0)
set_rx("bcast",0)
set_rx("resp",0)
set_retries(retries=10,delay=20)
#listen(chan)
