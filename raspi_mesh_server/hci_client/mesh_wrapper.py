import serial_wrapper as ser

RF_ID = {
    "print_test" :(0x01,0x03),
    "print_nrf" :0x01
}

RF_CMD = {
    "print_nrf" : (0x02,RF_ID["print_nrf"])
}

def on_raw(data):
    """ data received from RF in a size,dataarray Fromat"""
    print("data>",data)
    return

def command(cmd):
    ser.send(RF_ID[cmd])
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
