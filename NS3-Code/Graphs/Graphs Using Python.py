#Throughput and Delay of 802.11p(33dBm) vs Distance Using Nakagami Propagation loss model

import matplotlib.pyplot as plt
x = [10,20,30,40,50,60,70,80,90,100,110,120,130,140 ,150 ,160 ,170 ,180 ,190 ,200 ,210 ,220 ,230 ,240 ,250 ,260 ]
y1 = [201,201,201,201,201,201,201,201,201,201,201,201,179,179,179,179,156,134,134,134,89,89,89,67,67,44]
y2 = [88.0470 ,88.0740 ,88.1050 ,88.1370 ,88.1700 ,88.2020 ,88.2350 ,88.2680 ,88.3020 ,88.3350 ,88.3680 ,88.4010 ,88.4340 ,412.4680 ,88.5010 ,88.5340 ,88.5680 ,88.6010 ,399.6340 ,88.6670 ,412.7010 ,88.7340 ,88.7670 ,801.8010 ,1073.8340 ,762.8670]
fig,ax = plt.subplots()
ax.plot(x, y1, color="orange", marker="o")

ax.set_xlabel("Distance (m)",fontsize=14)

ax.set_ylabel("Throughput (bits/s)",color="orange",fontsize=14)

ax2=ax.twinx()
ax2.plot(x, y2,color="blue",marker="o")
ax2.set_ylabel("Delay (microseconds)",color="blue",fontsize=14)

ax3=ax.twinx()
ax3.plot(x, y2,color="blue",marker="o")
ax3.set_ylabel("Delay (microseconds)",color="blue",fontsize=14)

plt.title("Throughput and Delay of 802.11p(33dBm) vs Distance \n Using Nakagami Propagation loss model")
plt.grid()
plt.show()


#Range of 802.11p for different power levels Using Nakagami propagation loss model
import matplotlib.pyplot as plt

y1 = [40,60,90,140,170,230,270]
x1 = [1,2,3,4,5,6,7]
y2 = [40,60,90,140,220 ,310 ,390]

fig,ax = plt.subplots()
ax.plot(x1, y2, label = "Maximum Transmission Distance", color="orange", marker="o")
ax.plot(x1, y1, color="blue",label = "Distance of First Packet Drop", marker="*")

ax.set_xlabel("Tx power level (1->8.3dBm(min)  to  7->33dBm(max))",fontsize=14)
ax.set_ylabel("Distance (m)",fontsize=14)
plt.grid()
plt.legend()

plt.title("Range of 802.11p for different power levels \n Using Nakagami propagation loss model",fontsize=14)
plt.show()
