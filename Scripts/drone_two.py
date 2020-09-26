# Import necessary modules
# Do not import any other modules
import time
from dronekit import connect, VehicleMode, LocationGlobalRelative

#conneecting vehicle to a specific port
vehicle = connect('127.0.0.1:10251',wait_ready=True, baud=57600)


#arm the motors
print("Arming motors")
while not vehicle.is_armable:
    time.sleep(1)

#Set the vehicle mode as guided
vehicle.mode = VehicleMode("GUIDED")
vehicle.armed = True

while not vehicle.armed:
    print(" Waiting for arming...")
    time.sleep(1)

#vehicle.airspeed = 7 is default

print('Takeoff')
#the drone takes off to a specific height
vehicle.simple_takeoff(10)
#checks if altitude is reached or not for further tasks
while True:
    altitude = vehicle.location.global_relative_frame.alt
    print(altitude)
    if altitude >= 9.5:
        print("altitude_reached")
	      break
    time.sleep(1)

#opens a file which contains the waypopint where deployment is required
f = open("Temp_Log.txt","r")
reach = []

#This loop checks for the updation of the file initialised, if some new coordinate is found it travels to that waypoint.
for line in f:
    if line not in reach:
        temp = list(line.split())
        waypoint = LocationGlobalRelative(int(temp[0],int(temp[1]),10)
        reach.append(wp)
        i = 0
        vehicle.simple_goto(waypoint)
        print([vehicle.location.global_relative_frame.lat, vehicle.location.global_relative_frame.lon, vehicle.location.global_relative_frame.alt])

        time.sleep(10)
        while True:
            speed = vehicle.groundspeed
            if speed <= 1 and i==0:
                print(vehicle.location.global_relative_frame)
                break
                time.sleep(10)
                i = 1
            elif i == 1:
                break
    time.sleep(2)
time.sleep(30)

#vehicle's mode is changed to Return to location so that the drone return back to home coordinate
vehicle.mode = VehicleMode("RTL")
while True:
    altitude = vehicle.location.global_relative_frame.alt
    print(altitude)
    if altitude <= 1:
        print("altitude_reached")
	break
    time.sleep(1)
vehicle.mode = VehicleMode("LAND")
vehicle.close()

