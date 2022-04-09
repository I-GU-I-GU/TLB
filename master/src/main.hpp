#include <Arduino.h>
#include<serial_link.hpp>
#include<motors.hpp>
#include<sensors.hpp>
#include<io_control.hpp>

extern bool roller_logic;
extern unsigned long roller_timer;
extern unsigned int roller_period;


extern int silo_number;
extern int running_state;
/////////////// sliding motor ///////////////////
extern bool sliding_motor_logic;
extern unsigned long sliding_motor_timer;
extern unsigned int sliding_motor_period;
/////////////// roller motor //////////////////
extern bool roller_motor_logic;
extern unsigned long roller_motor_timer;
extern unsigned int roller_motor_period;
extern unsigned int roller_pulse_counter;
extern unsigned int roller_pulse_target;


void interprete_command(void);
void initial_variables(void);
void interprete_command(String serial_command);
void run_machine(void);
void run_process_state2(void);

// for debugging
void debug_motor(int parameter);
