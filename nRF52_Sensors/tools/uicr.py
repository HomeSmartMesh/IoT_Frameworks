import os
import pylink
import cfg

nodes = cfg.get_local_nodes("../../config/nodes.json")
uicr = cfg.get_local_nodes("../config/uicr_map.json")
jlink = pylink.JLink()
jlink.open(os.environ['SEG_JLEDU'])
node_id = 0
node = {}

def get_node_id(uid):
    res =""
    for key,val in nodes.items():
        if "uid" in val:
            if val["uid"] == uid:
                res = key
    return res

def get_uid_32():
    device_id = jlink.memory_read32(0x100000A4, 2)
    ids1 = "{0:#0{1}X}".format(device_id[0],10)
    ids2 = "{0:#0{1}X}".format(device_id[1],10)
    print("device id : %s %s" % (ids1,ids2) )
    return

def get_uid():
    d = jlink.memory_read8(0x10000060, 8)
    res = "0x%02X %02X %02X %02X %02X %02X %02X %02X"%(d[3],d[2],d[1],d[0],d[7],d[6],d[5],d[4])
    return res

def write_uicr_customer(reg_name,val):
    reg_nb = int(reg_name.lstrip("CUSTOMER_"))
    address = 0x10001080 + 4*reg_nb
    if((type(val) is str) or (type(val) is unicode)):
        words_list = [int(val)]
    else:
        words_list = [val]
    jlink.memory_write32(address,words_list)
    return

def read_uicr_customer(reg_name):
    reg_nb = int(reg_name.lstrip("CUSTOMER_"))
    address = 0x10001080 + 4*reg_nb
    return jlink.memory_read32(address,1)[0]

def start():
    jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)
    jlink.connect('NRF52832_XXAA', verbose=True)
    print('ARM Id: %d' % jlink.core_id())
    print('CPU Id: %d' % jlink.core_cpu())
    print('Core Name: %s' % jlink.core_name())
    print('Device Family: %d' % jlink.device_family())
    return

def read_id():
    global node_id
    global node
    uid = get_uid()
    node_id = get_node_id(uid)
    node = nodes[node_id]
    print("device uid : %s" % uid )
    print("node mesh id : %s" % node_id )
    print("name : %s" % node["name"] )
    print("channel : %s" % node["channel"] )
    return

def write_config():
    print("reg %s <= (mesh_id:%s)"%(uicr["mesh_id"],node_id))
    write_uicr_customer(            uicr["mesh_id"],node_id)
    print("reg %s <= (channel:%s)"%(uicr["mesh_id"],node["channel"]))
    write_uicr_customer(            uicr["channel"],node["channel"])

    test_pass = True
    test_id   = read_uicr_customer(uicr["mesh_id"])
    if(test_id != int(node_id)):
        test_pass = False
    test_chan = read_uicr_customer(uicr["channel"])
    if(test_chan != int(node["channel"])):
        test_pass = False
    if(not test_pass):
        print("Verification failed")
        print("mesh_id => %d"%test_id)
        print("channel => %d"%test_chan)
    else:
        print("write verified")
    return

def read_config():
    test_id   = read_uicr_customer(uicr["mesh_id"])
    test_chan = read_uicr_customer(uicr["channel"])
    print("mesh_id => %d"%test_id)
    print("channel => %d"%test_chan)
    return
