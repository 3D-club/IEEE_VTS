import time
from dronekit import connect,VehicleMode,LocationGlobalRelative

vehicle = connect('/dev/ttyACM0',wait_ready=True)

print('Arming Motors')

while True:
    switch5 = vehicle.channels['5']
    print(switch5)
    if(switch5>1500):
        print("GUIDED")
        break
    time.sleep(1)

while not vehicle.is_armable:
    time.sleep(1)

vehicle.mode = VehicleMode("GUIDED")

vehicle.armed = True

print("    oof it started")

time.sleep(8)

vehicle.armed = False

vehicle.mode = VehicleMode("STABILIZE")
vehicle.close()
