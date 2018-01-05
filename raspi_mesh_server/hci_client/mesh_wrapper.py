import serial_wrapper as ser

pid = {
    "exec_cmd"      : 0xEC,
    "ping"          : 0x01,
    "request_pid"   : 0x02,
    "chan_switch"   : 0x03,
    "reset"         : 0x04,
    "alive"         : 0x05,
    "button"        : 0x06,
    "light"         : 0x07,
    "temperature"   : 0x08,
    "heat"          : 0x09,
    "bme280"        : 0x0A,
    "rgb"           : 0x0B,
    "magnet"        : 0x0C,
    "dimmer"        : 0x0D,
    "light_rgb"     : 0x0E,
    "gesture"       : 0x0F,
    "proximity"     : 0x10
}

inv_pid = {v: k for k, v in pid.items()}

exec_cmd = {
    "get_status"    : 0x01,
    "set_channel"   : 0x04,
    "send_msg"      : 0x20,
    "set_rx"        : 0x30,
    "test_rf"       : 0x31,
    "cfg_retries"   : 0x32,
    "cfg_ack_delay" : 0x33
}

set_rx = {
    "sniff" : 0x00,
    "bcast" : 0x01,
    "msg"   : 0x02,
    "resp"  : 0x03
}

msg = {
    "size":0,
    "payload":[]
}

def parse_pid(byte):
    return inv_pid[byte]

def parse_is_broadcast(byte):
    return (byte & 0x80)

def parse_control(byte):
    res = ""
    if(byte & 0x80):
        res = res + "Broadcast "
    else:
        res = res + "Directed "
        if(byte & 0x40):
            res = res + "Msg_Ack "
            if(byte & 0x20):
                res = res + "Message "
                if(byte & 0x10):
                    res = res + "To_Send_Ack "
                else:
                    res = res + "Do_Not_Send_Ack "
            else:
                res = res + "Acknowledge "
        else:
            res = res + "Req_Res "
            if(byte & 0x20):
                res = res + "Request "
            else:
                res = res + "Response "
    ttl = byte & 0x0F
    res = res + "ttl "+str(ttl)
    return res

def parse_rf_data(data):
    rf_data_text = parse_pid(data[2]) + \
                    "(" + str(data[3]) + " -> "
    if(not parse_is_broadcast(data[1])):
        rf_data_text += str(data[4]) + ") ; "
    else:
        rf_data_text += " X) ; "
    rf_data_text +=  parse_control(data[1])
    return rf_data_text

def on_raw(data):
    """ data received from RF in a size,dataarray Fromat"""
    print("rf>",parse_rf_data(data))
    return

def on_broadcast(data):
    print("bcast>",parse_rf_data(data))
    return

def command(cmd,params=[]):
    ser.send([pid["exec_cmd"],exec_cmd[cmd]]+params)
    return

def send_msg(payload):
    print("tx>",parse_rf_data(payload))
    ser.send([pid["exec_cmd"],exec_cmd["send_msg"]] + payload)
    return

def serial_on_line(line):
    ind_split = line.find(":0x")
    line_type = line[0:ind_split]
    line_data = line[ind_split+3:]
    if( (line_type == "sniff") or (line_type == "msg") or\
        (line_type == "resp") or (line_type == "bcast")  ):
        data = bytearray.fromhex(line_data)
        print(line_type,">",parse_rf_data(data))
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
