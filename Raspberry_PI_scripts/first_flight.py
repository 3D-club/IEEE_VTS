from dronekit import connect,VehicleMode,LocationGlobalRelative
import time
vehicle = connect('/dev/ttyACM0',wait_ready=True,baud=57600)

while True:
    switch5 = vehicle.channels['5']
    print(switch5)
    if(switch5>1500):
	print("GUIDED")
	break


time.sleep(5)
print("Arming Motors")
while not vehicle.is_armable:
    time.sleep(1)

vehicle.mode = VehicleMode("GUIDED")
vehicle.armed = True

print("Takeoff")

vehicle.simple_takeoff(2)

while True:
    print(vehicle.location.global_relative_frame.alt)
    if((vehicle.location.global_relative_frame.alt)>=(1.5)):
        print("altitude reached")
	break
    time.sleep(1)

vehicle.mode = VehicleMode("RTL")

time.sleep(4)
print("disarming")
vehicle.armed = False
vehicle.mode = VehicleMode("STABILIZE")

vehicle.close()
