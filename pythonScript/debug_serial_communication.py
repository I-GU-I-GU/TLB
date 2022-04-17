import serial
import os
import time

# add comment

comport = 'COM14'
ser = serial.Serial(port=comport,baudrate=9600,timeout=1)
print("Start debugging")
start_time = time.time()

ping_counter = 0
ping_target = 100
error_counter = 0
debug_period = 1

time.sleep(5)

wait_response_time = 0.1
ser.write(bytes('s1\n', 'utf-8'))
time.sleep(1)

while ping_counter <= ping_target:
    if time.time()-start_time >= debug_period:
        print("send ping")
        ping_counter = ping_counter+1
        ser.write(bytes('g\n', 'utf-8'))
        time.sleep(wait_response_time)
        machine_state_string = ser.readline()
        machine_state_string = str(machine_state_string).rstrip() # machine_state is string type
        machine_state_string = machine_state_string.split("'")[1]
        machine_state_string = machine_state_string.split("\\")[0]
        if(len(machine_state_string)<=0):
            print("error")
            error_counter = error_counter +1
        else:
            message = "state = " + machine_state_string
            print(message)

message = "ping " + str(ping_target) + " times"
print(message)
message = "Error " + str(error_counter) + " times"
print(message)

if ser.isOpen():
    ser.close()

