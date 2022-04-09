import serial
import os
import time

comport = 'COM14'
ser = serial.Serial(port=comport,baudrate=9600,timeout=1)
print("Start debugging")
start_time = time.time()
time.sleep(5)

wait_response_time = 0.3
try:
    print("Start checking motors")
    # ==== turn on relay =======
    ser.write(bytes('m6\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    # === run silo motor 1 ========
    ser.write(bytes('d1\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    # === run silo motor 2 ========
    ser.write(bytes('d2\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    # === run silo motor 3 ========
    ser.write(bytes('d3\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    # === run silo motor 4 ========
    ser.write(bytes('d4\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    # === stop silo motors ========
    ser.write(bytes('d5\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    # ==== run sticker motor =====
    ser.write(bytes('m8\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(2)

    # ===== stop sticker motor =====
    ser.write(bytes('m9\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(2)

    # ===== run sling motor forward =======
    ser.write(bytes('d6\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(0.9)

    # ==== stop sliding motor =======
    ser.write(bytes('d8\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    # ===== release printer =====
    ser.write(bytes('m0\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    # ==== run printer ========
    ser.write(bytes('m1\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(7)

    # ==== stop printer =====
    ser.write(bytes('m2\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)
    ser.write(bytes('r\n','utf-8'))
    time.sleep(3)

    # ===== release printer =====
    ser.write(bytes('m0\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    # === turnoff relay 
    ser.write(bytes('m7\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    # ===== open tray =====
    ser.write(bytes('m3\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(2)

    # ==== close tray ====
    ser.write(bytes('m4\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(2)

    # =========== chuck servo ===========
    #on chuck 90
    ser.write(bytes('t4\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    #on chuck 180
    ser.write(bytes('t5\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    # ========== release servo ========
    ser.write(bytes('t1\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    ser.write(bytes('t2\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    ser.write(bytes('t1\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    ser.write(bytes('t3\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)
    
    ser.write(bytes('t1\n', 'utf-8'))
    time.sleep(wait_response_time)
    response_message = ser.readline()
    print(response_message)
    time.sleep(1)

    # ========== reset ============
    ser.write(bytes('r\n','utf-8'))
    time.sleep(2)


except:
    if ser.isOpen():
        ser.close()

