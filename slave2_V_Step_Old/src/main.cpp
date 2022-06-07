#include <Arduino.h>
#include"motor_drive.hpp"
#include"io_control.hpp"
//#include"sensor.hpp"

unsigned long motor_timer = 0;   // get time from micros() function
const int motor_pulse_period = 300;  // time in microseconds


bool pulse_logic = false;
int roller_state = 0;
bool running_state = false;

unsigned int free_run_counter = 0;
const int free_run_target = 1500;

// printer control state
int run_printer_state = 0;
unsigned long pull_motor_timer = 0;
unsigned int pull_motor_period = 50;
unsigned int pull_motor_target = 0;
unsigned int pull_motor_counter = 0;
bool pull_motor_logic = false;

unsigned long slab_motor_timer = 0;
unsigned long slab_motor_period = 50;
unsigned int slab_motor_target = 0;
unsigned int slab_motor_counter = 0;
bool slab_motor_logic = false;

const int loadcell_target = 10;
const int slap_up_down = 1500;
const int slab_down = 1500;

unsigned long delay_slab = 500;
unsigned long delay_slab_time = 0;


unsigned long delay_off = 6000;
unsigned long delay_ST = 0;

const int delay_startup = 1300;
unsigned long startup_timer = 0;

const int slab_down1 = 800;
const int pull_back1 = 200;
const int slap_period = 100;
const int slab_up =40;
const int slab_down_period = 140;
const int pull_period = 100;
const int pull_period_end = 100;
//************ release motor *************
int release_state = 0;

void run_printer(void);
void set_parameter_slab_down(void);
void set_parameter_slab_up(void);
void set_parameter_pull_backward(void);
void set_parameter_pull_forward(void);
void release_pulling_motor(void);

//================== setup function ===================
void setup() {
  Serial.begin(9600);
  initial_ios();
  initial_motors();
  Relay_OFF();
  running_state = false;
  run_printer_state = 0;
}
//================== loop function ===================
void loop() {
  //********** process sticker state ******************
  byte printer_control_signal = get_printer_control_signal();
  //printer_control_signal = 1;
  switch(printer_control_signal)
  {
    case 1:
    {
      Relay_ON();
      // running printer control state machine
      run_printer();
      break;
    }
    case 2:
    {
      // release pulling motor
      release_pulling_motor();
      break;
    }
    default:
    {
      // reset pull motor parameters
      run_printer_state = 0;
      release_state = 0;
    }
  }
  //int weight_value = get_weight();
}

void release_pulling_motor(void)
{
  switch(release_state)
  {
    case 0:
    {
      
      set_pull_motor_backward();
      pull_motor_timer = micros();
      pull_motor_period = 30;     // first version pulse_period = 30
      pull_motor_target = 2500;
      pull_motor_counter = 0;
      release_state = 1;
      break;
    }
    case 1:
    {
      if((micros()-pull_motor_timer)>=pull_motor_period)
      {
        pull_motor_timer = micros();
        pull_motor_logic = !pull_motor_logic;
        run_pull_motor(pull_motor_logic);

        pull_motor_counter = pull_motor_counter + 1;
        if(pull_motor_counter>=pull_motor_target)
        {
          release_state = 3;
        }
      }
      break;
    }
    case 3:
    {
      Relay_OFF();
      break;
    }
    default:
    {

    }
  }
}

void run_printer(void)
  {    
    switch(run_printer_state)
      {
        case 0:
        {
          //Serial.println("state 0");
          if(get_limit_switch())
          {
            //set_slab_motor_forward();
            run_printer_state = 2;
          }
          else
          {
            set_slab_motor_backward();
            slab_motor_period = slap_period;
            slab_motor_counter = 0;
            slab_motor_timer = micros();
            run_printer_state = 1;
          }
          break;
        }
        case 1:
        {                // move slap down first
          if((micros()-slab_motor_timer)>=slab_motor_period)
          {
            slab_motor_logic =! slab_motor_logic;
            run_slab_motor(slab_motor_logic);
            int limit_logic = get_limit_switch();
            if(limit_logic>0)
            {
              run_printer_state = 2;
            }
          }
          break;
        }
        case 2:
        {
          startup_timer = millis();
          run_printer_state = 3;
          break;
        }
        case 3:
        {
          if((millis()-startup_timer)>=delay_startup)
          {
            run_printer_state = 4;
          }
          break;
        }
        case 4:
        {
          set_slab_motor_forward(); 
          slab_motor_period = slab_up;
          slab_motor_target = slap_up_down;  
          slab_motor_counter = 0;
          run_printer_state = 5;
          break;
        }
        case 5: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {
            slab_motor_timer = micros();
            slab_motor_logic =! slab_motor_logic;
            run_slab_motor(slab_motor_logic);
            slab_motor_counter = slab_motor_counter + 1;
            if(slab_motor_counter>=1.1*slab_motor_target)
            {
              run_printer_state = 6;
            }
          }
          break;
        }
        case 6:            
        {
          set_pull_motor_forward();   
          pull_motor_timer = micros();
          pull_motor_period = pull_period;
          run_printer_state = 7;
          break;
        }
        case 7: 
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);

            int loadcell_weight = get_weight();
            if(loadcell_weight>=loadcell_target)
            {
              run_printer_state = 8;
            }
          }
          break;
        }
        case 8:
        {
          delay_slab_time = millis();
          delay_slab = 300;   // previous = 300
          run_printer_state = 9;
          break;
        }
        case 9:
        {
          if((millis()-delay_slab_time)>=delay_slab)
          {
            run_printer_state = 10;
          }
          break;
        }
        case 10: 
        {
          set_pull_motor_forward();
          pull_motor_timer = micros();
          pull_motor_period = pull_period;
          pull_motor_counter =0;
          pull_motor_target = 500;      // prevoius => 200
          run_printer_state = 11;
          break;
        }
        case 11:
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);
            pull_motor_counter = pull_motor_counter + 1;
            if(pull_motor_counter>=pull_motor_target)
            {
              run_printer_state = 12;
            }
          }
          break;
        }
            case 12:
        {
          delay_slab_time = millis();
          delay_slab = 300;
          run_printer_state = 13;
          break;
        }
        case 13:
        {
          if((millis()-delay_slab_time)>=delay_slab)
          {
            run_printer_state = 14;
          }
          break;
        }

       case 14:
        {
          set_slab_motor_backward(); 
          slab_motor_period = slab_up;
          slab_motor_target = 800;   // previous slab_down1 = 800
          slab_motor_counter = 0;
          run_printer_state = 15;
          break;
        }
        case 15: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {
            slab_motor_timer = micros();
            slab_motor_logic =! slab_motor_logic;
            run_slab_motor(slab_motor_logic);
            slab_motor_counter = slab_motor_counter + 1;
            if(slab_motor_counter>=slab_motor_target)
            {
              run_printer_state = 16;
            }
          }
          break;
        }
        case 16:
        {
          delay_slab_time = millis();
          run_printer_state = 17;
          break;
        }
        case 17:
        {
          if((millis()-delay_slab_time)>=delay_slab)
          {
            run_printer_state = 18;
          }
          break;
        }
        case 18: 
        {
          set_pull_motor_forward();
          pull_motor_timer = micros();
          pull_motor_period = pull_period;
          pull_motor_counter =0;
          pull_motor_target = 300;
          run_printer_state = 19;
          break;
        }
        case 19:
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);
            pull_motor_counter = pull_motor_counter + 1;
            if(pull_motor_counter>=pull_motor_target)
            {
              run_printer_state = 20;
            }
          }
          break;
        }

        case 20:
        {
          set_slab_motor_backward();
          slab_motor_timer = micros();
          slab_motor_period = slap_period;
          run_printer_state = 21;
          break;
        }
        case 21: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {                                                 
            slab_motor_timer = micros();                    
            slab_motor_logic = !slab_motor_logic;           
            run_slab_motor(slab_motor_logic);               
            run_pull_motor(slab_motor_logic);
            int limit_pressed = get_limit_switch();         
            if(limit_pressed>0)
            {
              run_printer_state = 22;
            }
          }
          break;
        }
        case 22:
        {
          set_pull_motor_backward();
          pull_motor_timer=micros();
          pull_motor_period = pull_period;
          pull_motor_counter =0;
          pull_motor_target = 1300;
          run_printer_state = 23;

          break;
        }
        case 23:
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);
            pull_motor_counter = pull_motor_counter + 1;
            if(pull_motor_counter>=pull_motor_target)
            {
              run_printer_state = 24;
            }
          }
          break;
        }
         case 24:
        {
          set_slab_motor_forward(); 
          slab_motor_period = slab_up;
          slab_motor_target = slap_up_down;  
          slab_motor_counter = 0;
          run_printer_state = 25;
          break;
        }
        case 25: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {
            slab_motor_timer = micros();
            slab_motor_logic =! slab_motor_logic;
            run_slab_motor(slab_motor_logic);
            slab_motor_counter = slab_motor_counter + 1;
            if(slab_motor_counter>=1.1*slab_motor_target)
            {
              run_printer_state = 26;
            }
          }
          break;
        }
        case 26:            
        {
          set_pull_motor_forward();   
          pull_motor_timer = micros();
          pull_motor_period = pull_period;
          run_printer_state = 27;
          break;
        }
        case 27: 
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);

            int loadcell_weight = get_weight();
            if(loadcell_weight>=loadcell_target)
            {
              run_printer_state = 28;
            }
          }
          break;
        }
        case 28:
        {
          delay_slab_time = millis();
          run_printer_state = 29;
          break;
        }
        case 29:
        {
          if((millis()-delay_slab_time)>=delay_slab)
          {
            run_printer_state = 30;
          }
          break;
        }
        case 30: 
        {
          set_pull_motor_forward();
          pull_motor_timer = micros();
          pull_motor_period = pull_period;
          pull_motor_counter =0;
          pull_motor_target = 300;
          run_printer_state = 31;
          break;
        }
        case 31:
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);
            pull_motor_counter = pull_motor_counter + 1;
            if(pull_motor_counter>=pull_motor_target)
            {
              run_printer_state = 32;
            }
          }
          break;
        }
            case 32:
        {
          delay_slab_time = millis();
          run_printer_state = 33;
          break;
        }
        case 33:
        {
          if((millis()-delay_slab_time)>=delay_slab)
          {
            run_printer_state = 34;
          }
          break;
        }

       case 34:
        {
          set_slab_motor_backward(); 
          slab_motor_period = slab_up;
          slab_motor_target = slab_down1;  
          slab_motor_counter = 0;
          run_printer_state = 35;
          break;
        }
        case 35: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {
            slab_motor_timer = micros();
            slab_motor_logic =! slab_motor_logic;
            run_slab_motor(slab_motor_logic);
            slab_motor_counter = slab_motor_counter + 1;
            if(slab_motor_counter>=slab_motor_target)
            {
              run_printer_state = 36;
            }
          }
          break;
        }
        case 36:
        {
          delay_slab_time = millis();
          run_printer_state = 37;
          break;
        }
        case 37:
        {
          if((millis()-delay_slab_time)>=delay_slab)
          {
            run_printer_state = 38;
          }
          break;
        }
        case 38: 
        {
          set_pull_motor_forward();
          pull_motor_timer = micros();
          pull_motor_period = pull_period;
          pull_motor_counter =0;
          pull_motor_target = 300;
          run_printer_state = 39;
          break;
        }
        case 39:
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);
            pull_motor_counter = pull_motor_counter + 1;
            if(pull_motor_counter>=pull_motor_target)
            {
              run_printer_state = 40;
            }
          }
          break;
        }

        case 40:
        {
          set_slab_motor_backward();
          slab_motor_timer = micros();
          slab_motor_period = slap_period;
          run_printer_state = 41;
          break;
        }
        case 41: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {                                                 
            slab_motor_timer = micros();                    
            slab_motor_logic = !slab_motor_logic;           
            run_slab_motor(slab_motor_logic);               
            run_pull_motor(slab_motor_logic);
            int limit_pressed = get_limit_switch();         
            if(limit_pressed>0)
            {
              run_printer_state = 58;
            }
          }
          break;
        }
        /////////////////////////////////////////////////////// SKIP TO STATE 58 /////////////////////////////////////////////////////////
        case 42:
        {
          set_pull_motor_backward();
          pull_motor_timer=micros();
          pull_motor_period = pull_period;
          pull_motor_counter =0;
          pull_motor_target = 1300;
          run_printer_state = 43;

          break;
        }
        case 43:
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);
            pull_motor_counter = pull_motor_counter + 1;
            if(pull_motor_counter>=pull_motor_target)
            {
              run_printer_state = 44;
            }
          }
          break;
        }
        case 44:
        {
          set_slab_motor_forward(); 
          slab_motor_period = slab_up;
          slab_motor_target = slap_up_down;  
          slab_motor_counter = 0;
          run_printer_state = 45;
          break;
        }
        case 45: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {
            slab_motor_timer = micros();
            slab_motor_logic =! slab_motor_logic;
            run_slab_motor(slab_motor_logic);
            slab_motor_counter = slab_motor_counter + 1;
            if(slab_motor_counter>=1.1*slab_motor_target)
            {
              run_printer_state = 46;
            }
          }
          break;
        }
        case 46:            
        {
          set_pull_motor_forward();   
          pull_motor_timer = micros();
          pull_motor_period = pull_period;
          run_printer_state = 47;
          break;
        }
        case 47: 
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);

            int loadcell_weight = get_weight();
            if(loadcell_weight>=loadcell_target)
            {
              run_printer_state = 48;
            }
          }
          break;
        }
        case 48:
        {
          delay_slab_time = millis();
          run_printer_state = 49;
          break;
        }
        case 49:
        {
          if((millis()-delay_slab_time)>=delay_slab)
          {
            run_printer_state = 50;
          }
          break;
        }
        case 50: 
        {
          set_pull_motor_forward();
          pull_motor_timer = micros();
          pull_motor_period = pull_period;
          pull_motor_counter =0;
          pull_motor_target = 300;
          run_printer_state = 51;
          break;
        }
        case 51:
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);
            pull_motor_counter = pull_motor_counter + 1;
            if(pull_motor_counter>=pull_motor_target)
            {
              run_printer_state = 52;
            }
          }
          break;
        }
            case 52:
        {
          delay_slab_time = millis();
          run_printer_state = 53;
          break;
        }
        case 53:
        {
          if((millis()-delay_slab_time)>=delay_slab)
          {
            run_printer_state = 54;
          }
          break;
        }

       case 54:
        {
          set_slab_motor_backward(); 
          slab_motor_period = slab_up;
          slab_motor_target = slab_down;  
          slab_motor_counter = 0;
          run_printer_state = 55;
          break;
        }
        case 55: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {
            slab_motor_timer = micros();
            slab_motor_logic =! slab_motor_logic;
            run_slab_motor(slab_motor_logic);
            slab_motor_counter = slab_motor_counter + 1;
            if(slab_motor_counter>=slab_motor_target)
            {
              run_printer_state = 56;
            }
          }
          break;
        }
        case 56:
        {
          set_slab_motor_backward();
          slab_motor_timer = micros();
          slab_motor_period = slap_period;
          run_printer_state = 57;
          break;
        }
        case 57: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {                                                 
            slab_motor_timer = micros();                    
            slab_motor_logic = !slab_motor_logic;           
            run_slab_motor(slab_motor_logic);               
            run_pull_motor(slab_motor_logic);
            int limit_pressed = get_limit_switch();         
            if(limit_pressed>0)
            {
              run_printer_state = 58;
            }
          }
          break;
        }
        case 58: 
        {
          set_pull_motor_backward();
          pull_motor_timer = micros();
          pull_motor_period = pull_period;
          pull_motor_counter =0;
          pull_motor_target = 100;
          run_printer_state = 59;
          break;
        }
        case 59:
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);
            pull_motor_counter = pull_motor_counter + 1;
            if(pull_motor_counter>=pull_motor_target)
            {
              run_printer_state = 60;
            }
          }
          break;
        }
        case 60:
        {
          set_slab_motor_forward(); 
          slab_motor_period = slab_up;
          slab_motor_target = 400;  
          slab_motor_counter = 0;
          run_printer_state = 61;
          break;
        }
        case 61: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {
            slab_motor_timer = micros();
            slab_motor_logic =! slab_motor_logic;
            run_slab_motor(slab_motor_logic);
            slab_motor_counter = slab_motor_counter + 1;
            if(slab_motor_counter>=slab_motor_target)
            {
              run_printer_state = 62;
            }
          }
          break;
        }
         case 62: 
        {
          set_pull_motor_forward();
          pull_motor_timer = micros();
          pull_motor_period = pull_period;
          pull_motor_counter =0;
          pull_motor_target = 400;
          run_printer_state = 63;
          break;
        }
        case 63:
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);
            pull_motor_counter = pull_motor_counter + 1;
            if(pull_motor_counter>=pull_motor_target)
            {
              run_printer_state = 64;
            }
          }
          break;
        }
        ////////////////////////////////////////////
        ////////////////////////////////////////////
        ////////////////////////////////////////////


        
        case 64:
        {
          break;
        }



        ////////////////////////////////////////////
        ////////////////////////////////////////////
        ////////////////////////////////////////////


        case 401:
        {
          set_pull_motor_forward(); 
          pull_motor_timer = micros();
          pull_motor_period = pull_period;
          run_printer_state = 412;
          break;
        }
        case 412:
        {
          if((micros()-pull_motor_timer)>=pull_motor_period)
          {
            pull_motor_timer = micros();
            pull_motor_logic =! pull_motor_logic;
            run_pull_motor(pull_motor_logic);

            int loadcell_weight = get_weight();
            if(loadcell_weight>=loadcell_target)
            {
              run_printer_state =423;
            }
          }
          break;
        }









        case 501:
        {
          set_slab_motor_forward(); 
          slab_motor_period = slap_period;
          slab_motor_target = slap_up_down;  
          slab_motor_counter = 0;
          run_printer_state = 51;
          break;
        }
        case 511: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {
            slab_motor_timer = micros();
            slab_motor_logic =! slab_motor_logic;
            run_slab_motor(slab_motor_logic);
            slab_motor_counter = slab_motor_counter + 1;
            if(slab_motor_counter>=1.1*slab_motor_target)
            {
              run_printer_state = 52;
            }
          }
          break;
        }
         case 521: 
        {
          set_slab_motor_backward();
          slab_motor_timer = micros();
          slab_motor_period = slap_period;
          run_printer_state = 53;
          break;
        }
        case 531: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {                                                 
            slab_motor_timer = micros();                    
            slab_motor_logic = !slab_motor_logic;         
            run_slab_motor(slab_motor_logic);               
            run_pull_motor(slab_motor_logic);
            int limit_pressed = get_limit_switch();         
            if(limit_pressed>0)
            {
              run_printer_state = 54;
            }
          }
          break;
        }
        case 541:
        {
          set_slab_motor_forward(); 
          slab_motor_period = slap_period;
          slab_motor_target = slap_up_down;  
          slab_motor_counter = 0;
          run_printer_state = 55;
          break;
        }
        case 551: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {
            slab_motor_timer = micros();
            slab_motor_logic =! slab_motor_logic;
            run_slab_motor(slab_motor_logic);
            slab_motor_counter = slab_motor_counter + 1;
            if(slab_motor_counter>=1.1*slab_motor_target)
            {
              run_printer_state = 56;
            }
          }
          break;
        }
         case 561: 
        {
          set_slab_motor_backward();
          slab_motor_timer = micros();
          slab_motor_period = slap_period;
          run_printer_state = 57;
          break;
        }
        case 571: 
        {
          if((micros()-slab_motor_timer)>= slab_motor_period)
          {                                                 
            slab_motor_timer = micros();                    
            slab_motor_logic = !slab_motor_logic;         
            run_slab_motor(slab_motor_logic);               
            run_pull_motor(slab_motor_logic);
            int limit_pressed = get_limit_switch();         
            if(limit_pressed>0)
            {
              run_printer_state = 30;
            }
          }
          break;
        }  




         case 601:
        {
          delay_ST = millis();
          run_printer_state = 61;
          break;
        }
        case 611:
        {
          if((millis()-delay_ST)>=delay_off)
          {
            run_printer_state = 34;
          }
          break;
        }




        default:
        {

        }
      }
  }

