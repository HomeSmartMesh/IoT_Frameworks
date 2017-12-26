import logging as log
from time import time
import json

def static_vars(**kwargs):
    def decorate(func):
        for k in kwargs:
            setattr(func, k, kwargs[k])
        return func
    return decorate


def RGB_Tester_Burst():
    log.info("RGB_Tester_Burst() post")
    return "100"

@static_vars(event_time=time())
def Sleeproom_Light_Up():
    result = None
    tnow = time()
    delay = tnow - Sleeproom_Light_Up.event_time
    if(delay > 2):
        Sleeproom_Light_Up.event_time = tnow
        log.info("Sleeproom_Light_Up() post after %f",delay)
        result = "100"
    else:
        log.debug("Sleeproom_Light_Up() skipped sent since %f",delay)
    return result

@static_vars(event_time=time())
def RGB_Tester_Single():
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
