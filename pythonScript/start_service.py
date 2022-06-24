import os
import time
from mysql.connector import connect

def check_db():
    db_connector =  connect(host="localhost", user="root", port = 3333, passwd="sbj41310962",  db="sbj",  charset="utf8"  )
    database_cursor = db_connector.cursor()
    sql_query = 'SELECT command FROM print_command WHERE flag = "0" ORDER BY serial DESC LIMIT 1'
    database_cursor.execute(sql_query)
    result_list = database_cursor.fetchall()
    db_connector.close()
    return result_list



main_state = 0
while True:
    time.sleep(0.5)
    if main_state == 0:
        try:
            check_db()
            main_state = 1
        except:
            time.sleep(5)
            
    if main_state == 1:
        os.startfile("D:\\TLB-main015\\pythonScript\\load_program.bat")
        break

exit()