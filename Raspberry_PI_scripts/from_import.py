from dronekit import connect,VehicleMode,LocationGlobalRelative
import time
vehicle = connect('/dev/ttyACM0',wait_ready=True,baud=57600)

print("battery: %s " %vehicle.battery)
print("GPS: %s " %vehicle.gps_0)
print("gimbal: %s " %vehicle.gimbal)
print("version: %s " %vehicle.version)
print(" Attitude: %s" % vehicle.attitude)
print(" Is Armable?: %s" % vehicle.is_armable)


print("Arming motors")
vehicle.mode = VehicleMode("GUIDED")
vehicle.armed =  True
while not vehicle.armed:
    print ("waiting for vehicle to arm")
    time.sleep(1)

print("5 seconds")
time.sleep(5)
print("disarming")
vehicle.armed = False
