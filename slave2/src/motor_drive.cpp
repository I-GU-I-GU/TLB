#include"motor_drive.hpp"


const int pull_motor_pulse = 3;
const int pull_motor_direction = 2;

const int slab_motor_pulse = 5;
const int slab_motor_direction = 4;

void initial_motors(void){
    // printer control pin
    pinMode(pull_motor_direction,OUTPUT);
    pinMode(pull_motor_pulse,OUTPUT);
    pinMode(slab_motor_direction,OUTPUT);
    pinMode(slab_motor_pulse,OUTPUT);
}

void set_pull_motor_forward(void)
{
    digitalWrite(pull_motor_direction,LOW);         // 1st version
    //digitalWrite(pull_motor_direction,HIGH);             // 2nd version
}

void set_pull_motor_backward(void)
{
    digitalWrite(pull_motor_direction,HIGH);        // 1st version
    //digitalWrite(pull_motor_direction,LOW);
}

void run_pull_motor(bool output_logic)
{
    digitalWrite(pull_motor_pulse,output_logic);
}

void set_slab_motor_forward(void)
{
    digitalWrite(slab_motor_direction,LOW);
}

void set_slab_motor_backward(void)
{
    digitalWrite(slab_motor_direction,HIGH);
}

void run_slab_motor(bool output_logic)
{
    digitalWrite(slab_motor_pulse,output_logic);
}
