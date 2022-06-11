#include"io_control.hpp"


const int printer_control_bit2 = A4;
const int printer_control_bit1 = A3;
const int printer_control_bit0 = A2;

const int loadcell_pin = A1;
const int SS_Relay = 10;
const int limit_switch_pin = 6;


void Relay(void)
{
    pinMode(SS_Relay,OUTPUT);
}


void Relay_ON(void)
{
    digitalWrite(SS_Relay,HIGH);

}


void Relay_OFF(void)
{
    digitalWrite(SS_Relay,LOW);
}


void initial_ios(void)
{
    // loadcell
    pinMode(loadcell_pin,INPUT);
    // control printer state
    pinMode(printer_control_bit2,INPUT);
    pinMode(printer_control_bit1,INPUT);
    pinMode(printer_control_bit0,INPUT);

    pinMode(limit_switch_pin,INPUT_PULLUP);
}

int get_limit_switch(void){
    return !digitalRead(limit_switch_pin);
}


int get_weight(void)
{
    return analogRead(loadcell_pin);
}

byte get_printer_control_signal(void)
{
    byte control_signal_value = 0x00;
    control_signal_value = digitalRead(printer_control_bit2);
    control_signal_value = (control_signal_value <<1) + digitalRead(printer_control_bit1);
    control_signal_value = (control_signal_value <<1) + digitalRead(printer_control_bit0);

    byte return_value = 0x00;
    switch (control_signal_value)
    {
        case 0x02:
        {
            return_value = 1;
            break;
        }
        case 0x01:
        {
            return_value = 2;
            break;
        }
        case 0x05:
        {
            return_value = 2;
            break;
        }
        default:
        {
            return_value = 0;
        }
    }
    return return_value;
}
