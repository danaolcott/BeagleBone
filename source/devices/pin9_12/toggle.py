import os
import time

for x in range (0,100000):
	os.system('echo 2 > /dev/pin9_12')
	time.sleep(1)

