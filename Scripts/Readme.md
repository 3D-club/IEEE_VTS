First we need some softwares for applying these scripts.

### The softwares are listed below: - 
1. Mission PLanner 1.3.73
2. Anaconda 3(64-bit)

#### They can be downloaded from following links: - 
1. Mission Planner - https://firmware.ardupilot.org/Tools/MissionPlanner/MissionPlanner-latest.msi
  Installation: - 
  https://ardupilot.org/planner/docs/mission-planner-installation.html
  Follow the steps from the above link.
  
2. Anaconda3 - https://www.anaconda.com/products/individual
  Installation: - 
  Just keep clicking next except one step where you must ensure to check the 2nd to be ticked and 1st option is not.
  
### Setting up anaconda: - 
1. Open one anaconda3 terminal.
2. Type "conda create -n environment_name python==2.7" and run it. This creates a environment where we can install any other version of python in which we wish to work.
3. Type "conda activate environment_name" to activate the environment.
4. Now we will install some necessary frameworks.
5. Use the following commands one by one and wait for completion: - 
    pip install mavproxy
    pip install dronekit
    pip install dronekit-sitl
6. Also we have to change the version of pymavlink so that it becomes compatible.
7. For removing previous version type "pip uninstall pymavlink"
8. For installing required version type "pip install pymavlink==2.4.6"
9. Your anaconda terminal is set


### Starting simulations: - 
1. Open 6 new terminals and run "conda activate environment_name".
2. Type these commands in every terminal: - 
   **T1** ``` dronekit-sitl copter -I0 --home=10.757575,78.816440,0,180 ```
   **T2** ``` py mavproxy.py --master tcp:127.0.0.1:5760 --out udp:127.0.0.1:14550 --out udp:127.0.0.1:10250 ```
   **T3** ``` python drone_one.py ```
   **T4** ``` dronekit-sitl copter -I1 --home=10.757575,78.816540,0,180 ```
   **T5** ``` py mavproxy.py --master tcp:127.0.0.1:5770 --out udp:127.0.0.1:14551 --out udp:127.0.0.1:10251 ```
   **T6** ``` python drone_two.py ```
3. Run T1, T2, T4, T5.
4. Now open mission planner and wait until it spawns the two drones.
5. When done spawning run T3 and T6.
6. Let the scripts run and you can observe drones.
