#include<Arduino.h>
#include"io_control.hpp"

const int AD0=A0;
const int AD1=A2;
const int AD2=A4;

const int reset_pin = A7;

const int actuator_pin1 = 15;
const int actuator_pin2 = 17; 

///////////////////////////////////////////////////////////////////////////////////////////

const int sensor_flip = 36;

const int flip_motor_pin1 = 66;
const int flip_motor_pin2 = 68;

const int slide_flip_pin1 = 46;
const int slide_flip_pin2 = 44;

//////////////////////////////////////////////////////////////////////////////////////////

const int limit_switch_pin = 13;  //limitBack

const int over_limit_pin = 11;    //limitFront

const int SS_relay = 27;               // plese check with developer //// pin 47

const int run_converyer = 46;           // please check with developer

const int printer_control_bit2 = A6;
const int printer_control_bit1 = A8;
const int printer_control_bit0 = A10;

// const int roller_status0 = A1;
// const int roller_status1 = A3;
// const int roller_status2 = A5;

// lamp
const int lamp1 = 55;
const int lamp2 = 57;
const int lamp3 = 59;
const int lamp4 = 61;

/// limit new

const int limit_run_machine = 63;

void initial_io_control(void){
    pinMode(AD0,OUTPUT);
    pinMode(AD1,OUTPUT);
    pinMode(AD2,OUTPUT);

    pinMode(limit_run_machine,INPUT_PULLUP);

    // pinMode(roller_status0,INPUT_PULLUP);
    // pinMode(roller_status1,INPUT_PULLUP);
    // pinMode(roller_status2,INPUT_PULLUP);

    pinMode(reset_pin,OUTPUT);
    pinMode(run_converyer,OUTPUT);

    digitalWrite(reset_pin,HIGH);
    digitalWrite(run_converyer,HIGH);

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
    pinMode(SS_relay,OUTPUT);

    // lamp control
    pinMode(lamp1,OUTPUT);
    pinMode(lamp2,OUTPUT);
    pinMode(lamp3,OUTPUT);
    pinMode(lamp4,OUTPUT);

    digitalWrite(lamp1,LOW);
    digitalWrite(lamp2,LOW);
    digitalWrite(lamp3,LOW);
    digitalWrite(lamp4,LOW);


    /////flip_control/////

    pinMode(flip_motor_pin1,OUTPUT);
    pinMode(flip_motor_pin2,OUTPUT);
    pinMode(slide_flip_pin1,OUTPUT);
    pinMode(slide_flip_pin2,OUTPUT);
    pinMode(sensor_flip,INPUT);

    //////////////////////////////////

    reset_io_control();
}
 //////////////////////////////////////////////// flip tube ///////////////////////////////////////
void on_motor_flip_forward(void)
{
    digitalWrite(flip_motor_pin1,HIGH);
    digitalWrite(flip_motor_pin2,LOW);
}

void on_motor_flip_backward(void)
{
    digitalWrite(flip_motor_pin1,LOW);
    digitalWrite(flip_motor_pin2,HIGH); 
}

void off_motor_flip(void)
{
    digitalWrite(flip_motor_pin1,LOW);
    digitalWrite(flip_motor_pin2,LOW);
}

void linere_forward(void)
{
    digitalWrite(slide_flip_pin1,LOW);
    digitalWrite(slide_flip_pin2,HIGH);
}

void linere_backward(void)
{
    digitalWrite(slide_flip_pin1,HIGH);
    digitalWrite(slide_flip_pin2,LOW);
}

void off_linere(void)
{
    digitalWrite(slide_flip_pin1,LOW);
    digitalWrite(slide_flip_pin2,LOW);
}

bool check_flip_tube(void)
{
    return digitalRead(sensor_flip);  
}

bool check_printer_out_side(void)
{
    return digitalRead(limit_run_machine);
}
 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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


// int get_conveyor_status(void)
// {
//     int roller_status = 0;
//     digitalWrite(reset_pin,HIGH);
//     digitalWrite(run_converyer,HIGH);
//     //==========
//     //Serial.print(digitalRead(roller_status2));
//     //Serial.print(digitalRead(roller_status1));
//     //Serial.println(digitalRead(roller_status0));
//     roller_status = (digitalRead(roller_status2)<<2) + (digitalRead(roller_status1)<<1) + digitalRead(roller_status0);
//     return roller_status;
// }

void reset_conveyor(void)
{
    digitalWrite(run_converyer,LOW);
    digitalWrite(reset_pin,LOW);
}

void on_conveyor(void)
{
    digitalWrite(run_converyer,LOW);
    digitalWrite(reset_pin,HIGH);
}

void check_conveyor(void)
{
    digitalWrite(run_converyer,HIGH);
    digitalWrite(reset_pin,LOW);
}
void reinit_conveyor(void)
{
    digitalWrite(run_converyer,HIGH);
    digitalWrite(reset_pin,HIGH);
}

void clear_lamp(void)
{
    digitalWrite(lamp1,LOW);
    digitalWrite(lamp2,LOW);
    digitalWrite(lamp3,LOW);
    digitalWrite(lamp4,LOW);
}
void on_lamp(char lamp_number)
{
    switch(lamp_number)
    {
           case '0':
        {
            digitalWrite(lamp1,LOW);
            digitalWrite(lamp2,LOW);
            digitalWrite(lamp3,LOW);
            digitalWrite(lamp4,LOW);
            break;
        }
        case '1':
        {
            digitalWrite(lamp1,HIGH);
            digitalWrite(lamp2,LOW);
            digitalWrite(lamp3,LOW);
            digitalWrite(lamp4,LOW);
            break;
        }
        case '2':
        {
            digitalWrite(lamp1,LOW);
            digitalWrite(lamp2,HIGH);
            digitalWrite(lamp3,LOW);
            digitalWrite(lamp4,LOW);
            break;
        }
        case '3':
        {
            digitalWrite(lamp1,LOW);
            digitalWrite(lamp2,LOW);
            digitalWrite(lamp3,HIGH);
            digitalWrite(lamp4,LOW);
            break;
        }
        case '4':
        {
            digitalWrite(lamp1,LOW);
            digitalWrite(lamp2,LOW);
            digitalWrite(lamp3,LOW);
            digitalWrite(lamp4,HIGH);
            break;
        }
        default:
        {
            clear_lamp();
        }
    }

}


