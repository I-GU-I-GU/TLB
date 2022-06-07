#include <Arduino.h>
#include <Servo.h>

void init_motors(void);
void reset_motor_variables(void);


void sliding_motor_forward(void);
void sliding_motor_backward(void);

void write_sliding_pulse(bool silo_motor_logic);
void run_sliding_motor(void);
void stop_sliding_motor(void);

void run_sliding_motor_step2(void);
void stop_sliding_motor_step2(void);

// servo functions
void on_release_servo(void);
void on_half_release_servo(void);
void off_release_servo(void);
//void servo_shift(void);
// void on_chuck_servo(void);
// void off_chuck_servo(void);

// roller motor
void write_roller_pulse(bool pulse_logic);
void roller_forward(void);
void roller_backward(void);

void test_servo(int parameter);

