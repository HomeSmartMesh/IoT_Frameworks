import serial_wrapper as ser

pid = {
    "exec_cmd" : 0xEC
}

exec_cmd = {
    "status"    : 0x01,
    "channel"   : 0x04,
    "send"      : 0x20
}

msg = {
    "size":0,
    "payload":[]
}

COMMANDS = {
    "get_status"    :[pid["exec_cmd"],exec_cmd["status"]],
    "set_channel"   :[pid["exec_cmd"],exec_cmd["channel"]],
    "send_msg"      :[pid["exec_cmd"],exec_cmd["send"]]
}

def on_raw(data):
    """ data received from RF in a size,dataarray Fromat"""
    print("data>",data)
    return

def command(cmd,params=[]):
    ser.send(COMMANDS[cmd]+params)
    return

def send_msg(payload):
    ser.send(COMMANDS["send_msg"] + payload)
    return

def serial_on_line(line):
    if(line.startswith("raw:0x")):
        m_line = line[6:]
        data = bytearray.fromhex(m_line)
        on_raw(data)
    else:
        print("text>",line)
    return

def run():
    ser.run()
    return

def start(config):
    ser.serial_start(config,serial_on_line)
    return


def print_nrf():
    ser.send(b"b\x02\x01")
    return
