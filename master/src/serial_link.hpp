#include<Arduino.h>

extern String serial_command;

bool check_serial_command(void);
void reset_serial_variables(void);
String get_serial_command(void);