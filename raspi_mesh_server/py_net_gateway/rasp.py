import os
# Return CPU temperature as a character string                                      
def getCPUtemperature():
    res = os.popen('vcgencmd measure_temp').readline()
    return(res.replace("temp=","").replace("'C\n",""))

# Return RAM information (unit=kb)
# used RAM                                                                 
def getUsedRAM():
    p = os.popen('free')
    i = 0
    while 1:
        i = i + 1
        line = p.readline()
        if i==2:
            return(line.split()[2])

#top fails on early startup "top: failed tty get"
def getCPU_Avg1min():
    p = os.popen("top -b -n1")
    line = p.readline()
    ind_cpu = line.find("load average:")
    if(ind_cpu!= -1):
        cpu_vals = line[ind_cpu+13:]
        return(cpu_vals.split(',')[0])
    else:
        return "0"

# Return % of CPU used by user as a character string                                
def getCPUuse():
    return(str(os.popen("top -n1 | awk '/Cpu\(s\):/ {print $2}'").readline().strip()))

def getUsedDisk():
    p = os.popen("df /")
    i = 0
    while 1:
        i = i +1
        line = p.readline()
        if i==2:
            return(line.split()[2])

def getUsedDiskDir(path):
    p = os.popen("sudo du --summarize "+path)
    line = p.readline()
    if(len(line)>0):
        return(line.split()[0])
    else:
        return "0"
