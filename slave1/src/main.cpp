#include <Arduino.h>
//#include <SoftwareSerial.h>
#include"motor_drive.hpp"
#include"io_control.hpp"

//#include"sensor.hpp"

unsigned long motor_timer = 0;   // get time from micros() function
const int motor_pulse_period = 500;  // time in microseconds

bool pulse_logic = false;
int roller_state = 0;
bool running_state = false;


//************ release motor *************
int release_state = 0;

void run_printer(void);
void set_parameter_slab_down(void);
void set_parameter_slab_up(void);
void set_parameter_pull_backward(void);
void set_parameter_pull_forward(void);
void release_pulling_motor(void);

//================== setup function ===================
void setup(){
  Serial.begin(9600);
  initial_ios();
  initial_motors();
  //initial_proximeter_sensor();
  running_state = false;
}
//================== loop function ===================
void loop() {
  //********** process 6 roller motors ***************
  byte roller_address = get_address();
  //Serial.println(roller_address);
  if(running_state == false)
  {
    if(roller_address == 0)
    {
      running_state = false;
      stop_all_motors();// stop motor
      // reset parameters
    }
    else{
      running_state = true;
      roller_state = 1;
      motor_timer = micros();
    }
  }
  if(running_state == true)
  {
    switch(roller_state)
    {
      case 1:
      {
        if((micros()-motor_timer)>= motor_pulse_period)
        {
          pulse_logic =! pulse_logic;
          run_step_motor(roller_address,pulse_logic);
          motor_timer = micros();
        }
        break;
      }
      default:
      {
        running_state = false;
      }
    }
  }
}