from subfunctions import *
import serial
import os
import time

silo_file = r"C:\Test_pathfile\labelfile.txt"
path_error_file = r"d:\Log_path\error_file.txt"
current_silo = "0"

main_state = 0


match main_state:
    case 0:
        print('0')
    case 1:
        print('1')
    case 2:
        print('2')
    case 3:
        print('3')
    case _:
        print('Default')