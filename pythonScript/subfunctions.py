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

