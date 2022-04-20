import serial
import time

comport = 'COM4'
ser = serial.Serial(port=comport,baudrate=9600,timeout=1)
print("Start debugging")
time.sleep(3)

def check_conveyor_state(response):
    ready_flag = 0
    resp = response.strip()
    if(resp == b'0'):
        ready_flag = 1
    if(resp == b'1'):
        ready_flag = 2
        print('Wait state')
        print('please recheck conveyor state')
    if(resp == b'2'):
        ready_flag = 2
        print('Running state')
        print('please recheck conveyor state')
    if(resp == b'3'):
        ready_flag = 3
        print('Box Full')
    if(resp == b'4'):
        ready_flag = 3
        print('Stuck')
    if(resp == b'5'):
        ready_flag = 1
        print('complete')
    if(resp == b'6'):
        ready_flag = 3
        print('Box empty')
    return ready_flag


command_dict = {
    "get conveyor state":b'x\n',
    "on reset conveyor":b'y\n',
    "off reset conveyor":b'z\n',
    "on conveyor":b'a\n',
    "check mainboard state":b'g\n'                                                                           '
}

main_state = 1
loop_complete = False
fifteen_sec_timer1 = time.perf_counter
fifteen_sec_timer2 = time.perf_counter

run_silo = [b's1\n',b's2\n',b's3\n',b's4\n']
for loop_counter in range(0,2):
    message = "loop number " + str(loop_counter)
    print(message)
    while not loop_complete :
        time.sleep(0.1)
        #=========== state transition ==========
        if main_state == 1:
            cmd = command_dict["get conveyor state"]
            ser.write(cmd)
            time.sleep(1)
            resp = ser.readline()
            check_flag = check_conveyor_state(resp)
            if check_flag == 1:
                print("recheck")
                main_state = 3
            if check_flag == 2:
                print("ready")
                # run conveyor then go to state 4
                cmd = command_dict['on conveyor']
                ser.write(cmd)
                time.sleep(1)
                ser.readline()
                fifteen_sec_timer1 = time.perf_counter
                main_state = 4
            if check_flag == 3:
                print("error")
                main_state = 2
        if main_state == 2:
            time.sleep(20)
            main_state = 1
        if main_state == 3:
            time.sleep(1)
            main_state = 1
        if main_state == 4:
            if time.perf_counter - fifteen_sec_timer1 >= 15:
                main_state = 2
                cmd = command_dict['on reset conveyor']
                ser.write(cmd)
                time.sleep(1)
                ser.readline()
                cmd = command_dict['off reset conveyor']
                ser.write(cmd)
                time.sleep(1)
                ser.readline()
                print("Stuck")
            else:
                cmd = command_dict['get conveyor state']
                ser.write(cmd)
                time.sleep(1)
                resp = ser.readline()
                response = resp.strip()
                if response == b'5':
                    #======== write silo =====
                    ser.write(run_silo.pop(0))
                    time.sleep(1)
                    ser.readline()
                    main_state = 5
                    fifteen_sec_timer2 = time.perf_counter
        if main_state == 5:
            if time.perf_counter - fifteen_sec_timer2 >= 15:
                main_state = 6
                print("Tube stuck")
            else:
                cmd = command_dict['check mainboard state']
                ser.write(cmd)
                time.sleep(1)
                resp = ser.readline()
                response = resp.strip()
                if response == b'26':
                    main_state = 7
        if main_state == 6:
            pass
        if main_state == 7:
            if len(run_silo)>0:
                main_state = 4
            else:
                cmd = command_dict['on reset conveyor']
                ser.write(cmd)
                time.sleep(1)
                ser.readline()
                cmd = command_dict['off reset conveyor']
                ser.write(cmd)
                time.sleep(1)
                ser.readline()
                loop_complete = True





