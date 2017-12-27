import logging as log
from time import time
import json

def static_vars(**kwargs):
    def decorate(func):
        for k in kwargs:
            setattr(func, k, kwargs[k])
        return func
    return decorate

def Bathroom_Hum_Status(input):
    red = 0
    green = 0
    blue = 0
    hum = float(input)

    if(hum < 60):
        green = 15
    else:
        green = 0
    if(hum < 50):
        blue = 0
    else:
        trig = (hum - 50.0) / 50.0
        blue = 200 * trig
    jColors = {"Red":red,"Green":green,"Blue":blue}
    return json.dumps(jColors)

def RGB_Tester_Burst(input):
    log.info("RGB_Tester_Burst() post")
    return "100"

@static_vars(event_time=time())
def Sleeproom_Light_Up(input):
    result = None
    tnow = time()
    delay = tnow - Sleeproom_Light_Up.event_time
    if(delay > 1):
        Sleeproom_Light_Up.event_time = tnow
        log.info("Sleeproom_Light_Up() post after %f",delay)
        result = "100"
    else:
        log.debug("Sleeproom_Light_Up() skipped sent since %f",delay)
    return result

@static_vars(event_time=time())
def RGB_Tester_Single(input):
    result = None
    tnow = time()
    delay = tnow - RGB_Tester_Single.event_time
    if(delay > 2):
        RGB_Tester_Single.event_time = tnow
        log.info("RGB_Tester_Single() post after %f",delay)
        payload = {"Red":4,"Green":1,"Blue":3}
        result = json.dumps(payload)
    else:
        log.debug("RGB_Tester_Single() skipped sent since %f",delay)
    return result
