import cfg
import serial_wrapper as ser

nodes = cfg.get_local_nodes("../../config/nodes.json")

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
    "proximity"     : 0x10,
    "test_rf_resp"  : 0x30
}

inv_pid = {v: k for k, v in pid.items()}

exec_cmd = {
    "get_status"    : 0x01,
    "readreg"       : 0x07,
    "writereg"      : 0x08,
    "set_channel"   : 0x09,
    "get_channel"   : 0x0A,
    "send_msg"      : 0x20,
    "set_mode"      : 0x21,
    "get_mode"      : 0x22,
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
mode = {
    "power_down"    : 0x01,
    "standby"       : 0x02,
    "tx_tdby2"      : 0x03,
    "rx"            : 0x04
}
inv_mode = {v: k for k, v in mode.items()}

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

def node_name(byte):
    return nodes[str(byte)]["name"]

def parse_rf_data(data):
    rf_data_text = parse_pid(data[2])
    if(data[2] == pid["test_rf_resp"]):
        rf_data_text += " res="+str(data[5])+" "
    rf_data_text += "(" + node_name(data[3]) + " -> "
    if(not parse_is_broadcast(data[1])):
        rf_data_text += node_name(data[4]) + ") ; "
    else:
        rf_data_text += " X) ; "
    rf_data_text +=  parse_control(data[1])
    return rf_data_text

def parse_rf_key_value(key,value):
    res = ""
    if(key == "mode"):
        res = ("mode="+inv_mode[int(value)])
    elif(key in ["sniff","msg","resp","bcast"]):
        if(value.startswith("0x")):
            value = value[2:]
        data = bytearray.fromhex(value)
        res = (key+">"+parse_rf_data(data))
    return res
def parse_rf_text(line):
    line_parsed = ""
    entries = line.split(';')
    for entry in entries:
        kv = entry.split(':')
        if(len(kv)==2):
            k = kv[0]
            v = kv[1]
            line_parsed += parse_rf_key_value(k,v)
    return line_parsed

def command(cmd,params=[]):
    ser.send([pid["exec_cmd"],exec_cmd[cmd]]+params)
    return

def send_msg(payload):
    print("tx>",parse_rf_data(payload))
    ser.send([pid["exec_cmd"],exec_cmd["send_msg"]] + payload)
    return

def serial_on_line(line):
    line_print = ""
    line_parse = parse_rf_text(line)
    if(line_parse):
        line_print += line_parse + " | "
    line_print += "text>"+line
    print(line_print)
    return

def serial_on_line_dep(line):
    ind_split = line.find(":0x")
    line_type = line[0:ind_split]
    line_data = line[ind_split+3:]
    if( (line_type == "sniff") or (line_type == "msg") or\
        (line_type == "resp") or (line_type == "bcast")  ):
        data = bytearray.fromhex(line_data)
        print(line_type,">",parse_rf_data(data))
    else:
        print("text>%s | %s" % (line,parse_rf_text(line)))
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

