#include <Arduino.h>

const int address_bit0 = 19;
const int address_bit1 = 20;
const int address_bit2 = 21;

const int motor1_pulse = 3;
const int motor2_pulse = 5;
const int motor3_pulse = 6;
const int motor4_pulse = 9;


unsigned long motor_timer = 0;   // get time from micros() function
const int motor_pulse_period = 500;  // time in microseconds

bool pulse_logic = false;
int roller_state = 0;
bool running_state = false;

//************ release motor *************
int release_state = 0;

void initial_ios(void);
byte get_address(void);

void initial_motors(void);
void run_step_motor(byte motor_number,bool output_logic);
void stop_all_motors(void);

//================== setup function ===================
void setup(){
  //Serial.begin(9600);
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


void initial_ios(void)
{
    // silo number
    pinMode(address_bit2,INPUT_PULLUP);
    pinMode(address_bit1,INPUT_PULLUP);
    pinMode(address_bit0,INPUT_PULLUP);
}

byte get_address(void){
    byte roller_address = 0x00;
    roller_address = (roller_address<<1)+digitalRead(address_bit2);
    roller_address = (roller_address<<1)+digitalRead(address_bit1);
    roller_address = (roller_address<<1)+digitalRead(address_bit0);
    return roller_address;
}


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
