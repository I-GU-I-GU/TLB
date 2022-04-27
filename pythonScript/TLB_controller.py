import serial
import os
import time
import datetime
from mysql.connector import connect

def save_conveyor_status(conveyor_status):
    if conveyor_status == b'3':
        update_c_status("Full")
        print("Conveyor in full state")
    if conveyor_status == b'4':
        update_c_status("Stuck")
        print("Conveyor in stuck state")
    if conveyor_status == b'5':
        update_c_status("Complete")
        print("Conveyor in complete state")
    if conveyor_status == b'6':
        update_c_status("Empty")
        print("Conveyor in empty state")

def read_cmd():
    db_connector =  connect(host="localhost", user="root", port = 3333, passwd="sbj41310962",  db="sbj",  charset="utf8"  )
    database_cursor = db_connector.cursor()
    sql_query = 'SELECT command FROM print_command WHERE flag = "0" ORDER BY serial DESC LIMIT 1'
    database_cursor.execute(sql_query)
    result_list = database_cursor.fetchall()
    db_connector.close()
    return result_list

def update_c_status(status_value):
    ts = time.time()
    current_timestamp = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')

    db_connector =  connect(host="localhost", user="root", port = 3333, passwd="sbj41310962",  db="sbj",  charset="utf8"  )
    database_cursor = db_connector.cursor()
    sql_query = 'UPDATE print_command SET remark = %s WHERE command = "c" ORDER BY serial DESC LIMIT 1'
    parameter_value = (status_value,)
    database_cursor.execute(sql_query,parameter_value)
    db_connector.commit()
    db_connector.close()

def update_silo_status(status_value):
    ts = time.time()
    current_timestamp = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')

    db_connector =  connect(host="localhost", user="root", port = 3333, passwd="sbj41310962",  db="sbj",  charset="utf8"  )
    database_cursor = db_connector.cursor()
    sql_query = 'UPDATE print_command SET remark = %s WHERE command != "c" ORDER BY serial DESC LIMIT 1'
    parameter_value = (status_value,)
    database_cursor.execute(sql_query,parameter_value)
    db_connector.commit()
    db_connector.close()

def update_cmd_flag():
    db_connector =  connect(host="localhost", user="root", port = 3333, passwd="sbj41310962",  db="sbj",  charset="utf8"  )
    database_cursor = db_connector.cursor()
    sql_query = 'UPDATE print_command SET flag = 1 WHERE flag = 0 ORDER BY serial DESC LIMIT 1'
    database_cursor.execute(sql_query)
    db_connector.commit()
    db_connector.close()

# ============ initial variables ==========
silo_timer = 0
jam_counter = 0
current_silo = '0'
reset_period = 20
# =========== main program ================
main_state = 0
machine_state = 0
comport = 'COM4'
ser = serial.Serial(port=comport,baudrate=9600,timeout=1)
time.sleep(2)
while True:
    time.sleep(1)
    if main_state == 0:                         # idle state
        if ser.isOpen():
            main_state = 1
        else:
            main_state = 5
    if main_state == 1:                         # check command from database and take action
        cmd_list = read_cmd()
        if len(cmd_list) > 0 :
            cmd = cmd_list[0][0]
            if cmd == 'c':
                print("run conveyor")
                ser.write(b'z\n')
                time.sleep(1)
                ser.write(b'y\n')
                time.sleep(1)
                main_state = 2
            if cmd >= '1' and cmd <= '4':
                message = "run silo number " + str(cmd)
                #print(message)
                ser.write(b's')
                ser.write(bytes(cmd,'utf-8'))
                ser.write(b'\n')
                silo_timer = time.time()
                jam_counter = 0
                current_silo = str(cmd)
                main_state = 3
        else:
            # no command just wait next loop
            pass
    if main_state == 2:                         # check response from conveyor
        ser.write(b'x\n')
        time.sleep(1)
        conveyor_status = ser.readline()
        conveyor_status = conveyor_status.strip()
        #print(conveyor_status)
        if conveyor_status <= b'2':
            # conveyor still running
            print("waiting for conveyor")
        if conveyor_status >=b'3' and conveyor_status <=b'6':
            save_conveyor_status(conveyor_status)
            update_cmd_flag()
            main_state = 1
            time.sleep(3)
    if main_state == 3:                         # check response from silo command
        ser.write(b'g\n')
        time.sleep(1)
        machine_state_string = ser.readline()
        machine_state_string = machine_state_string.rstrip() # machine_state is string type
        if(len(machine_state_string)<=0):
            print("machine not response")
        else:
            if machine_state_string.isdigit():
                machine_state = int(machine_state_string)
                message = "machine state = " + str(machine_state)
                print(message)
                if machine_state == 61 :
                    jam_counter = jam_counter + 1
                if machine_state == 0 or machine_state == 22 or machine_state == 26:
                    # silo run complete
                    print("Silo run completely")
                    update_cmd_flag()
                    update_silo_status("OK")
                    main_state = 1
                    time.sleep(2)
        #print(jam_counter)
        if jam_counter >= 6 :
            print("jaming")
            message = "jam " + str(current_silo)
            update_silo_status(message)
            update_cmd_flag()
            main_state = 1
            time.sleep(2)
        
        if time.time()-silo_timer >= reset_period:
            ser.write(b'r\n')
            time.sleep(1)
            ser.readline()
            time.sleep(5)
            main_state = 1
            
    if main_state == 5:                         # can not open serial port
        print(" Please check serial port")
        time.sleep(5)
