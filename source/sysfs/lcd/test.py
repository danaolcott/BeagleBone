import os
import time

for x in range (0,100000):
	os.system('echo -----Line0_____' + str(x) + ' _______> /sys/kernel/lcd/line0')
	os.system('echo -----Line1_____' + str(x/2) + ' _______> /sys/kernel/lcd/line1')
	os.system('echo -----Line2_____' + str(x/3) + ' _______> /sys/kernel/lcd/line2')
	time.sleep(1)

