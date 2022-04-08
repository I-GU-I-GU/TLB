#include<Arduino.h>
#include"io_control.hpp"

const int AD0=A1;
const int AD1=A2;
const int AD2=A3;

const int actuator_pin1 = 20;
const int actuator_pin2 = 18; 
const int limit_switch_pin = 17;  //limitBack

const int over_limit_pin = 11;    //limitFront

const int SS_relay = 47;

const int run_converyer = 27;

const int printer_control_bit2 = A15;
const int printer_control_bit1 = A14;
const int printer_control_bit0 = A13;

void initial_io_control(void){
    pinMode(AD0,OUTPUT);
    pinMode(AD1,OUTPUT);
    pinMode(AD2,OUTPUT);

    pinMode(actuator_pin1,OUTPUT);
    pinMode(actuator_pin2,OUTPUT);

    // over_limit_pin
    pinMode(over_limit_pin,INPUT_PULLUP);
    // limit switch at printer
    pinMode(limit_switch_pin,INPUT_PULLUP);
    // setup printer control bit
    pinMode(printer_control_bit2,OUTPUT);
    pinMode(printer_control_bit1,OUTPUT);
    pinMode(printer_control_bit0,OUTPUT);
    /////SS_relay//////
//////////////////////////////////
    pinMode(SS_relay,OUTPUT);
//////////////////////////////////

    reset_io_control();
    off_converyer();

}
void initial_logic()
{
   pinMode(run_converyer,OUTPUT);
}

void on_converyer()
{
    digitalWrite(run_converyer,LOW);
}

void off_converyer()
{
    digitalWrite(run_converyer,HIGH);
}

void operate_printer(void)
{
    digitalWrite(printer_control_bit2,LOW);
    digitalWrite(printer_control_bit1,HIGH);
    digitalWrite(printer_control_bit0,LOW);
}

void reset_printer_operation(void)
{
    digitalWrite(printer_control_bit2,LOW);
    digitalWrite(printer_control_bit1,LOW);
    digitalWrite(printer_control_bit0,LOW);
}

void release_printer(void)
{
    digitalWrite(printer_control_bit2,HIGH);
    digitalWrite(printer_control_bit1,LOW);
    digitalWrite(printer_control_bit0,HIGH);
}

bool read_over_limit(void)
{
    return digitalRead(over_limit_pin);
}

bool read_limit_switch(void)
{
    return digitalRead(limit_switch_pin);
}

void open_tray(void){
    digitalWrite(actuator_pin1,LOW);
    digitalWrite(actuator_pin2,HIGH);
}

void close_tray(void){
    digitalWrite(actuator_pin1,HIGH);
    digitalWrite(actuator_pin2,LOW);
}

void turnoff_actuator(void){
    digitalWrite(actuator_pin1,LOW);
    digitalWrite(actuator_pin2,LOW);
}

void Relay_ON(void)
{
    digitalWrite(SS_relay,HIGH);
}

void Relay_OFF(void)
{
    digitalWrite(SS_relay,LOW);
}

void run_silo_roller(int roller_number){
    switch(roller_number)
    {
        case 0:
        {
            digitalWrite(AD2,LOW);
            digitalWrite(AD1,LOW);
            digitalWrite(AD0,LOW);
            break;
        }
        case 1:
        {
            digitalWrite(AD2,LOW);
            digitalWrite(AD1,LOW);
            digitalWrite(AD0,HIGH);
            break;
        }
        case 2:
        {
            digitalWrite(AD2,LOW);
            digitalWrite(AD1,HIGH);
            digitalWrite(AD0,LOW);
            break;
        }
        case 3:
        {
            digitalWrite(AD2,LOW);
            digitalWrite(AD1,HIGH);
            digitalWrite(AD0,HIGH);
            break;
        }
        case 4:
        {
            digitalWrite(AD2,HIGH);
            digitalWrite(AD1,LOW);
            digitalWrite(AD0,LOW);
            break;
        }
        case 5:
        {
            digitalWrite(AD2,HIGH);
            digitalWrite(AD1,LOW);
            digitalWrite(AD0,HIGH);
            break;
        }
        case 6:
        {
            digitalWrite(AD2,HIGH);
            digitalWrite(AD1,HIGH);
            digitalWrite(AD0,LOW);
            break;
        }
        default:
        {
            digitalWrite(AD2,LOW);
            digitalWrite(AD1,LOW);
            digitalWrite(AD0,LOW);
        }
    }
}

void stop_silo_roller(void)
{
    reset_io_control();
}
void reset_io_control(void){
    digitalWrite(AD2,LOW);
    digitalWrite(AD1,LOW);
    digitalWrite(AD0,LOW);

    turnoff_actuator();
}