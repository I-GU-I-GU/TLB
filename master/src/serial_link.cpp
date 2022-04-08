
#include<serial_link.hpp>
String serial_command = "";

bool check_serial_command(void)
{
    bool interprete_status = false;
    if(Serial.available()){
        char temp_command = Serial.read();
        if(temp_command == '\n')
        {
            interprete_status = true;
        }
        else
        {
            serial_command = serial_command + String(temp_command);
        }  
    }
    return interprete_status;
}


String get_serial_command(void)
{
    String temp_command = serial_command;
    serial_command = "";
    return temp_command;
}

//*****************************
void reset_serial_variables(void){
    String serial_command = "";
    String serial_response = "";
}
