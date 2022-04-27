import serial
import os
import time
from mysql.connector import connect

def read_cmd():
    db_connector =  connect(host="localhost", user="root", port = 3333, passwd="sbj41310962",  db="sbj",  charset="utf8"  )
    database_cursor = db_connector.cursor()
    sql_query = 'SELECT command FROM print_command WHERE flag = 0 ORDER BY time_stamp DESC'
    database_cursor.execute(sql_query)
    result_list = database_cursor.fetchall()
    for result in result_list:
        print(result)
    db_connector.close()



current_silo = "0"
state_counter = 0
del_cfile_flag = 0
del_rfile_flag = 0
del_sfile_flag = 0
send_file = 0
main_state = 0
byte_silo_number = b'9'
comport = 'COM4'
ser = serial.Serial(port=comport,baudrate=9600,timeout=1)
print("master")
time.sleep(5)
reset_period = 25
start_time = time.time()
check_cloop = False
check_sloop = False

while True:
    try:
        if main_state == 0:     # idle
            if ser.isOpen():
                main_state = 1
            else:
                ser.open()
            time.sleep(5)
        if main_state == 1:     # waiting state
            if ser.isOpen():
                if os.path.isfile(path_error_file):
                    time.sleep(5)
                else:
                    if os.path.isfile(silo_file):
                        state_counter = 0
                        send_file = 1
                        main_state = 2
                        start_time = time.time()
                        time.sleep(1)
                    else:
                        pass
            else:
                main_state = 0
            time.sleep(1)
        if main_state == 2:     # running
            if send_file == 1:
                with open(silo_file) as f:
                    silo_number = f.readline().strip()
                    byte_silo_number = silo_number.encode()
                    if byte_silo_number == b'r':
                        ser.write(b'r\n')
                        main_state = 1
                        time.sleep(0.5)
                    else:
                        if (byte_silo_number)>= b'1' and (byte_silo_number)<= b'4':
                            ser.write(b's')
                            ser.write(byte_silo_number)
                            ser.write(b'\n')
                            current_silo =  byte_silo_number.decode("utf-8")
                            send_file = 0
                            time.sleep(0.5)
                        else:
                            print('check file')
                            if byte_silo_number == b'c':
                                del_cfile_flag = 1
                                time.sleep(0.5)
                            if byte_silo_number == b's':
                                del_sfile_flag = 1
                                time.sleep(0.5)
                if del_cfile_flag == 1:
                    del_cfile_flag = 0
                    if not os.path.isfile(path_error_file):
                        if os.path.isfile(silo_file):
                            os.remove(silo_file)
                            print('remove c file')
                        main_state = 1
                        check_cloop = True
                        # reset conveyor status
                        ser.write(b'z\n')
                        time.sleep(1)
                        ser.write(b'y\n')
                        time.sleep(3)
                        # check conveyor status
                        while check_cloop:
                            ser.write(b'x\n')
                            time.sleep(0.5)
                            conveyor_status = ser.readline()
                            conveyor_status = conveyor_status.strip()
                            print(conveyor_status)
                            if conveyor_status == b'0':
                                print("Conveyor in idle state")
                                with open(path_status_file, 'w') as status_file:
                                    error_file.write("Wait")
                            if conveyor_status == b'1':
                                print("Conveyor in waiting state")
                                with open(path_status_file, 'w') as status_file:
                                    error_file.write("Wait")
                            if conveyor_status == b'2':
                                print("Conveyor in running state")
                                with open(path_status_file, 'w') as status_file:
                                    error_file.write("Wait")
                            if conveyor_status == b'3':
                                print("Conveyor in full state")
                                with open(path_error_file, 'a+') as error_file:
                                    error_file.write("Full")
                            if conveyor_status == b'4':
                                print("Conveyor in stuck state")
                                with open(path_error_file, 'a+') as error_file:
                                    error_file.write("Stuck")
                            if conveyor_status == b'5':
                                print("Conveyor run complete")
                                with open(path_status_file, 'a+') as complete_file:
                                    complete_file.write("Complete")
                            if conveyor_status == b'6':
                                print("Conveyor in Empty state")
                                with open(path_error_file, 'a+') as error_file:
                                    error_file.write("Empty")
                            
                            if conveyor_status >= b'3' :
                                check_cloop = False
                            else:
                                time.sleep(2)

                if del_sfile_flag == 1:
                    del_sfile_flag = 0
                    if not os.path.isfile(path_error_file):
                        if os.path.isfile(silo_file):
                            os.remove(silo_file)
                            print('remove s file')
                        main_state = 1
                        # reset conveyor status
                        ser.write(b'v\n')
                        time.sleep(0.5)
                        # check conveyor status
                        ser.write(b'x\n')
                        time.sleep(0.5)
                        conveyor_status = ser.readline()
                        conveyor_status = conveyor_status.strip()
                        if conveyor_status == b'3':
                            print("Conveyor in full state")
                            with open(path_error_file, 'a+') as error_file:
                                error_file.write("Full")
                        if conveyor_status == b'6':
                            print("Conveyor in Empty state")
                            with open(path_error_file, 'a+') as error_file:
                                error_file.write("Empty")
                        
                if del_rfile_flag == 1:
                    del_rfile_flag = 0
                    main_state = 1
                    if os.path.isfile(silo_file):
                        os.remove(silo_file)
                        print('remove r file')         
                time.sleep(5)
            else:
                if time.time()-start_time >= reset_period:
                    if state_counter >= 6:
                        print("box jaming")
                        with open(path_error_file, 'a+') as error_file:
                            error_file.write("jam ")
                            error_file.write(current_silo)
                        print('Reset 15 seconds')
                        ser.write(b'r\n')
                    else:
                        print('time more than 15 seconds')
                        with open(path_error_file, 'a+') as error_file:
                            error_file.write("Error")
                        print('Reset 15 seconds')
                        ser.write(b'r\n')
                    main_state = 1
                    time.sleep(5)
                else:
                    ser.write(bytes('g\n', 'utf-8'))
                    time.sleep(1)
                    machine_state_string = ser.readline()
                    machine_state_string = str(machine_state_string).rstrip() # machine_state is string type
                    machine_state_string = machine_state_string.split("'")[1]
                    machine_state_string = machine_state_string.split("\\")[0]
                    if(len(machine_state_string)<=0):
                        machine_state = 99
                    else:
                        machine_state = int(machine_state_string)
                    message = "state " + str(machine_state)
                    if machine_state == 61 :
                        state_counter = state_counter + 1
                        
                    print(message)
                    if machine_state == 0 or machine_state == 26:
                        if os.path.isfile(silo_file):
                            os.remove(silo_file)
                            print('remove file')
                        main_state = 1
                        if byte_silo_number == b'4':
                            time.sleep(5)
                        else:
                            time.sleep(3)
                    time.sleep(0.3)
    except:
        print("Error")
        main_state =0
        if ser.isOpen():
            ser.close()

