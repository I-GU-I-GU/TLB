#include"motor_drive.hpp"

const int motor1_pulse = 6;
const int motor2_pulse = 9;
const int motor3_pulse = 10;
const int motor4_pulse = 12;

const int pull_motor_pulse = 3;
const int pull_motor_direction = 4;

const int slab_motor_pulse = 5;
const int slab_motor_direction = 6;

void initial_motors(void){
    pinMode(motor1_pulse,OUTPUT);
    pinMode(motor2_pulse,OUTPUT);
    pinMode(motor3_pulse,OUTPUT);
    pinMode(motor4_pulse,OUTPUT);

    // printer control pin
    pinMode(pull_motor_direction,OUTPUT);
    pinMode(pull_motor_pulse,OUTPUT);
    pinMode(slab_motor_direction,OUTPUT);
    pinMode(slab_motor_pulse,OUTPUT);
}

void set_pull_motor_forward(void)
{
    //digitalWrite(pull_motor_direction,LOW);         // 1st version
    digitalWrite(pull_motor_direction,HIGH);             // 2nd version
}

void set_pull_motor_backward(void)
{
    //digitalWrite(pull_motor_direction,HIGH);        // 1st version
    digitalWrite(pull_motor_direction,LOW);
}

void run_pull_motor(bool output_logic)
{
    digitalWrite(pull_motor_pulse,output_logic);
}

void set_slab_motor_forward(void)
{
    digitalWrite(slab_motor_direction,HIGH);
}

void set_slab_motor_backward(void)
{
    digitalWrite(slab_motor_direction,LOW);
}

void run_slab_motor(bool output_logic)
{
    digitalWrite(slab_motor_pulse,output_logic);
}

void run_step_motor(byte motor_number,bool output_logic){
    switch(motor_number){
        case 1:
        {
            digitalWrite(motor1_pulse,output_logic);
            break;
        }
        case 2:
        {
            digitalWrite(motor2_pulse,output_logic);
            break;
        }
        case 3:
        {
            digitalWrite(motor3_pulse,output_logic);
            break;
        }
        case 4:
        {
            digitalWrite(motor4_pulse,output_logic);
            break;
        }
        default:
        {  
            // do nothing
        }
    }
}

void stop_all_motors(void)
{
    digitalWrite(motor1_pulse,LOW);
    digitalWrite(motor2_pulse,LOW);
    digitalWrite(motor3_pulse,LOW);
    digitalWrite(motor4_pulse,LOW);
}