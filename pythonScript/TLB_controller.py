from ast import main
import serial
import os
import time
import datetime
from mysql.connector import connect
import time

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
    sql_query = 'UPDATE print_command SET remark = %s ORDER BY serial DESC LIMIT 1'
    parameter_value = (status_value,)
    database_cursor.execute(sql_query,parameter_value)
    db_connector.commit()
    db_connector.close()

def update_ok():
    db_connector =  connect(host="localhost", user="root", port = 3333, passwd="sbj41310962",  db="sbj",  charset="utf8"  )
    database_cursor = db_connector.cursor()
    sql_query = 'UPDATE print_command SET remark = "OK" ORDER BY serial DESC LIMIT 1'
    database_cursor.execute(sql_query)
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

state_26_counter = 0
# =========== main program ================
main_state = 0
machine_state = 0
comport = 'COM5'
ser = serial.Serial(port=comport,baudrate=9600,timeout=1)
time.sleep(10)
while True:
    time.sleep(0.5)
    print(time.time())
    #print('main state')
    #print(main_state)
    
    if main_state == 0:                         # idle state
        if ser.isOpen():
            time.sleep(5)
            ser.write(b'l0\n')
            main_state = 1
        else:
            main_state = 5
    if main_state == 1:                         # check command from database and take action
        cmd_list = read_cmd()
        if len(cmd_list) > 0 :
            cmd = cmd_list[0][0]
            if cmd >= '1' and cmd <= '4':
                message = "run silo number " + str(cmd)
                #print(message)
                ser.write(b's')
                ser.write(cmd.encode('UTF-8'))
                ser.write(b'\n')
                silo_timer = time.time()
                jam_counter = 0
                current_silo = cmd
                main_state = 6
        else:
            # no command just wait next loop
            pass
    if main_state == 3:                         # check response from silo command
        main_state_temp = 3
        ser.write(b'g\n')
        time.sleep(0.5)
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
                if machine_state == 26:
                    state_26_counter = state_26_counter + 1
                    main_state_temp = 3
                        
        if state_26_counter >= 1:
            state_26_counter = 0
            # silo run complete
            print("Silo run completely")
            main_state_temp = 1
            ser.write(b'l0\n')
            #print("go to state 1")
            update_cmd_flag()
            update_ok()
        # print('jam counter ')
        # print(jam_counter)

        if jam_counter >= 10 :
            jam_counter = 0
            print("jaming")
            ser.write(b'm0\n')
            time.sleep(0.5)
            ser.write(b'r\n')
            time.sleep(0.5)
            ser.readline()
            print("reset command invoke from jam")

            message = "jam " + current_silo
                # wait 10 seconds
            time.sleep(9)
            ser.write(b'l')
            ser.write(current_silo.encode('UTF-8'))
            ser.write(b'\n')
            time.sleep(1)
            ser.write(b'm2\n')
            time.sleep(0.5)
            update_silo_status(message)
            update_cmd_flag()
            main_state_temp = 1
            
        if time.time()-silo_timer >= reset_period:
            ser.write(b'm0\n')
            time.sleep(0.5)
            ser.write(b'r\n')
            time.sleep(1)
            ser.readline()
            time.sleep(10)
            ser.write(b'm2\n')
            time.sleep(0.5)
            main_state_temp = 1
            print("reset command invoke from reset period")
        main_state = main_state_temp
            
    if main_state == 5:                         # can not open serial port
        print(" Please check serial port")
        time.sleep(5)

    if main_state == 6:
        ser.write(b'g\n')
        time.sleep(0.5)
        machine_state_string = ser.readline()
        machine_state_string = machine_state_string.rstrip() # machine_state is string type
        if(len(machine_state_string)<=0):
            print("machine not response")
        else:
            if machine_state_string.isdigit():
                machine_state = int(machine_state_string)
                message = "machine state = " + str(machine_state)
                #print(message)
                if machine_state < 26 or machine_state == 61:
                    main_state = 3

