#include"io_control.hpp"

const int address_bit2 = A2;
const int address_bit1 = A1;
const int address_bit0 = A0;

//const int printer_control_bit2 = 8;
//const int printer_control_bit1 = A4;
//const int printer_control_bit0 = A3;

//const int limit_switch_pin = 7;

//const int loadcell_pin = A5;


void initial_ios(void)
{
    // silo number
    pinMode(address_bit2,INPUT);
    pinMode(address_bit1,INPUT);
    pinMode(address_bit0,INPUT);
}

byte get_address(void){
    byte roller_address = 0x00;
    roller_address = (roller_address<<1)+digitalRead(address_bit2);
    roller_address = (roller_address<<1)+digitalRead(address_bit1);
    roller_address = (roller_address<<1)+digitalRead(address_bit0);
    return roller_address;
}

