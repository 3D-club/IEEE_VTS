#Import necessary modules
#Do not import any other modules
import time
from dronekit import connect, VehicleMode, LocationGlobalRelative

#connect vehicle to a specific port
vehicle = connect('127.0.0.1:10250',wait_ready=True, baud=57600)

#initiate two files, one for log files and one for storing waypoints
log = open("Permanent_Log.txt", "a+")
f = open("Temp_Log.txt","a+")
f.truncate(0)
f.close()

def follow_waypoint(coordinates):
    """
	Purpose:
	---
	This governs the working of survey drone

	Input Arguments:
	---
	'wp' : [list]
  list of waypoints to be surveyed

	Does:
	---
  gives out the coordinates where deployment is required to a temporary log file which stores waypoints
	
  Example call:
  follow_waypoint(waypoint_list)

	"""
    for each_coordinate in coordinates:
        i = 0
        waypoint = LocationGlobalRelative(each_coordinate[0], each_coordinate[1], 10)
        vehicle.simple_goto(waypoint)
        info = [vehicle.location.global_relative_frame.lat, vehicle.location.global_relative_frame.lon, vehicle.location.global_relative_frame.alt]

        log.write(str(info))
        print([vehicle.location.global_relative_frame.lat, vehicle.location.global_relative_frame.lon, vehicle.location.global_relative_frame.alt])

        time.sleep(10)
        while True:
            speed = vehicle.groundspeed
            if speed <= 1:
                print(vehicle.location.global_relative_frame)
                variable = vehicle.location.global_relative_frame
                temp = str(variable.lat) + ' ' + str(variable.lon) + ' ' + str(10) + '\n'
                while True:
                    try:
                        with open("Temp_Log.txt", "a+") as f:
                            f.write(str(temp))
                            f.close()
                            break
                    except IOError:
                        pass
                break
            time.sleep(10)
            

#arm the motors
print("Arming motors")
while not vehicle.is_armable:
    time.sleep(1)

#set the vehicle mode to guided
vehicle.mode = VehicleMode("GUIDED")
vehicle.armed = True

while not vehicle.armed:
    print(" Waiting for arming...")
    time.sleep(1)

#vehicle.airspeed = 7 is default

print('Takeoff')

vehicle.simple_takeoff(10)

while True:
    altitude = vehicle.location.global_relative_frame.alt
    print(altitude)
    if altitude >= 9.5:
        print("altitude_reached")
	break
    time.sleep(1)
#pre-define the waypoints for survey drone  
coordinates = [[10.7575664, 78.8160476], [10.7573649, 78.8158120], [10.7571541, 78.8159917], [10.7571448, 78.8163967], [10.7573820, 78.8163364]]

#call the function which governs the working of survey drone
follow_waypoint(wp)

time.sleep(30)
f = open("Temp_Log.txt","a+")
f.write('completed')
f.close()

#after completing the task the vehicl's mode is set to Return to location which is the home loctaion
vehicle.mode = VehicleMode("RTL")
#vehicle lands
while True:
    altitude = vehicle.location.global_relative_frame.alt
    print(altitude)
    if altitude <= 1:
        print("altitude_reached")
	break
    time.sleep(1)
vehicle.mode = VehicleMode("LAND")
vehicle.close()
