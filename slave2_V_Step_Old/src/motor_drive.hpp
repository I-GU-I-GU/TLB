
#include<Arduino.h>

void initial_motors(void);
void run_step_motor(byte motor_number,bool output_logic);
void stop_all_motors(void);
int get_weight(void);
void run_pull_motor(bool output_logic);
void run_slab_motor(bool output_logic);

void set_pull_motor_forward(void);
void set_pull_motor_backward(void);
void set_slab_motor_forward(void);
void set_slab_motor_backward(void);