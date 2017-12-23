from ouimeaux.environment import Environment

def on_switch(switch):
    print("Switch found!", switch.name)

def on_motion(motion):
    print("Motion found!", motion.name)

print("Environment()")
env = Environment(on_switch, on_motion)

print("start()")
env.start()

print("discover()")
env.discover(seconds=3)

print("complete()")

#env.list_switches()
#switch = env.get_switch('TV Room')
#switch
