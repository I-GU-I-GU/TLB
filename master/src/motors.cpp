#include<motors.hpp>
#include<sensors.hpp>
#include<io_control.hpp>




const int sliding_motor_pulse_pin = 28;
const int sliding_motor_pulse_pin2 = 26;         // please check with developer

const int sliding_motor_dirction_pin = 24;

const int roller_motor_pulse_pin = 34;

const int release_servo_pin = 38;
const int chuck_servo_pin = 36;

Servo release_servo;
Servo chuck_servo;

void write_roller_pulse(bool pulse_logic){
    digitalWrite(roller_motor_pulse_pin,pulse_logic);
}

void run_sliding_motor(void)
{
    digitalWrite(sliding_motor_pulse_pin,HIGH);
}

void stop_sliding_motor(void)
{
    digitalWrite(sliding_motor_pulse_pin,LOW);
}

void run_sliding_motor_step2(void)
{
    digitalWrite(sliding_motor_pulse_pin2,HIGH);
}

void stop_sliding_motor_step2(void)
{
    digitalWrite(sliding_motor_pulse_pin2,LOW);
}

void write_sliding_pulse(bool sliding_motor_logic)
{
    digitalWrite(sliding_motor_pulse_pin,sliding_motor_logic);
}
void init_motors(void){
    //---------- sliding motor ------------
    pinMode(sliding_motor_dirction_pin,OUTPUT);
    pinMode(sliding_motor_pulse_pin,OUTPUT);
    pinMode(sliding_motor_pulse_pin2,OUTPUT);
    
    pinMode(roller_motor_pulse_pin,OUTPUT);

    pinMode(chuck_servo_pin,OUTPUT);
    pinMode(release_servo_pin,OUTPUT);


    // initial servo here
    release_servo.attach(release_servo_pin);
    chuck_servo.attach(chuck_servo_pin);
    
    on_release_servo();
    off_chuck_servo();
}
//*****************************************
void reset_motor_variables(void){
    stop_sliding_motor();
}

void sliding_motor_forward(void){
    digitalWrite(sliding_motor_dirction_pin,LOW);
}

void sliding_motor_backward(void){
    digitalWrite(sliding_motor_dirction_pin,HIGH);
}

void on_release_servo(void)
{
    release_servo.write(60);
}
void off_release_servo(void)
{
    release_servo.write(28);
}
void on_chuck_servo(void)
{
    chuck_servo.write(35);
}
void off_chuck_servo(void)
{
    chuck_servo.write(180);
}

void test_servo(int parameter)
{
    switch (parameter)
    {
        case 0:
        {
            release_servo.write(0);
            break;
        }
        case 1:
        {
            release_servo.write(15);
            break;
        }
        case 2:
        {
            release_servo.write(50);
            break;
        }
        case 3:
        {
            chuck_servo.write(45);
            break;
        }
        case 4:
        {
            chuck_servo.write(90);
            break;
        }
        case 5:
        {
            chuck_servo.write(180);
            break;
        }
        default:
            break;
    }
}






