#include"motor_drive.hpp"

const int motor1_pulse = 3;
const int motor2_pulse = 5;
const int motor3_pulse = 6;
const int motor4_pulse = 9;

void initial_motors(void){
    pinMode(motor1_pulse,OUTPUT);
    pinMode(motor2_pulse,OUTPUT);
    pinMode(motor3_pulse,OUTPUT);
    pinMode(motor4_pulse,OUTPUT);

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