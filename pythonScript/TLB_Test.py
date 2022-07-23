
from ast import main
import serial
import os
import time
import datetime

# =========== main program ================

comport = 'COM7'
ser = serial.Serial(port=comport,baudrate=9600,timeout=1)
time.sleep(7)

# on power
ser.write(b'm6\n')
time.sleep(1)
# reset printer
ser.write(b'm2\n')
time.sleep(1)

print("Start here")
# on printer
ser.write(b'm1\n')
time.sleep(5)

print("run roller")         # run roller
ser.write(b'm8\n')
time.sleep(0.1)

# move printer forward
ser.write(b'd6\n')
time.sleep(1.6)
# stop moving printer
ser.write(b'd8\n')
time.sleep(0.5)
# move printer back
ser.write(b'd7\n')
time.sleep(1.3)
# stop moving
ser.write(b'd8\n')
time.sleep(0.5)
print("End process")
# Releas printer
ser.write(b'm0\n')
time.sleep(1)
#========== off power ========
ser.write(b'm7\n')
time.sleep(1)
   

