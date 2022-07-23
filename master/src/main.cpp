//======= add description =====

#include <main.hpp>
#include<SoftwareSerial.h>

const byte rxPin = A11;
const byte txPin = 21;

//SoftwareSerial mySerial (rxPin, txPin);
String serial_command = "";

const String board_name = "TB013";
unsigned long tray_timer = 0;
unsigned int open_tray_period = 1000;  // time in milliseconds
unsigned int close_tray_period = 900;  // time in milliseconds


unsigned long period = 4000;  // time in milliseconds
unsigned long position_1_4 = 2600; // time in milliseconds
unsigned long last_time = 0;
unsigned long delay_AC = 1000; //time in miliseconds
unsigned long back_sliding = 4500; //time in miliseconds
unsigned long time_check = 2800; //time in miliseconds
unsigned long silo_run_dir = 200; //time in miliseconds

////////////////////////////////////////////////////////

unsigned long flip_time = 0;
unsigned int flip_period = 4500;

unsigned long linere_time = 0;
unsigned int linere_period = 1500;

unsigned long tube_drop_time = 0;
unsigned int tube_drop_period = 1800;

////////////////////////////////////////////////////////

bool move_more = false;
unsigned long move_more_timer = 0;
const int move_more_time = 5;

int silo_number = 0;
int running_state = 0;

int test_state = 0;
unsigned long test_time = 0;
unsigned int test_period = 1000;


/////////////// sliding motor ///////////////////
bool sliding_motor_logic;
unsigned long sliding_motor_timer;
unsigned  int sliding_motor_period;
/////////////// roller motr ////////////////////
bool roller_motor_logic;
unsigned long roller_motor_timer;
unsigned int roller_motor_period;
unsigned int roller_pulse_counter;
unsigned int roller_pulse_target;

//////////////
unsigned long chuck_timer =0;
unsigned int chuck_period = 500;


bool printer_runing = false;
bool is_origin = false;
bool origin_pulse_logic = false;
unsigned long check_origin_timer = millis();
/// 
bool edge_machine = false;

///
bool run_sticker_roller = false;
unsigned long sticker_roller_timer = 0;
const int sticker_roller_period = 100;
bool sticker_roller_logic = false;

unsigned long free_timer = 0;

void setup()
{
  Serial.begin(9600);
  //mySerial.flush();
  //while (!mySerial);
  initial_variables();
  init_motors();
  init_sensors();
  initial_io_control();
  reset_io_control();
}
//===========================================//

void loop()
{
  if (edge_machine)
  {
    switch(test_state)
      {
        case 1:
        { 
          sliding_motor_forward();  //return sliding
          run_sliding_motor();
          test_state = 2;
          break;
        }
        case 2:
        {
          int sensor_value = get_proximeter_values();
          if(sensor_value == 2)
          {
            stop_sliding_motor();
            test_time= millis();
            test_state = 3;
          }
          break;
        }
        case 3:
        {
          if((millis()-test_time)>=test_period)
          {
            test_state = 4;
          }
          break;
        }
        case 4:
        {
            sliding_motor_backward();  
            run_sliding_motor();  
            test_state = 5;  
          break;
        }

        case 5:
        {
          int sensor_value = get_proximeter_values();
          if(sensor_value == 1)
          {
            stop_sliding_motor();
            test_state = 6;
          }
          break;
        }
        case 6:
        {
          edge_machine = false;
          break;
        }
        default:
        {
          edge_machine = false;
          break;
        }
      }
  }

  if(is_origin == false)
  { 
    Relay_ON();
    sliding_motor_backward();
    run_sliding_motor();
    if(read_over_limit() || (get_proximeter_values()==1))
    {

      stop_sliding_motor();
      is_origin = true;
      delay(1000);
      open_tray();
      delay(500);
      reset_printer_operation();
      release_printer();
      on_release_servo();
      //on_chuck_servo();
      on_motor_flip_backward();
      delay(500);
      ////////////////////////tets flip///////////////////////
      //on_motor_flip_backward();
      linere_backward();
      delay(2000);
      linere_forward();
      reset_printer_operation();
      delay(300);
      off_linere();
      off_motor_flip();
      /////////////////////////test flip//////////////////////
      off_release_servo();
      
      //off_chuck_servo();
      close_tray();
      // ===== move more 10 ms ===========
      move_more = true;
      move_more_timer = millis();
    }
  }

  if (move_more == true)
  {
    run_sliding_motor();
    if (millis()-move_more_timer >= move_more_time)
    {
      stop_sliding_motor();
      move_more = false;
      delay(10);
      Relay_OFF();
    }
  }

  ///===========================================
  if (check_serial_command() == true)
  {
    String serial_command = get_serial_command();
    interprete_command(serial_command);
  }
  if(running_state>0)
  {
    run_machine();
  }

  // ============================================
  if(run_sticker_roller)
  {
    if((micros()-sticker_roller_timer)>=sticker_roller_period)
    {
      sticker_roller_logic =!sticker_roller_logic;
      write_roller_pulse(sticker_roller_logic);
      sticker_roller_timer = micros();
    }
  }
}


void run_machine(void)
{
  switch(running_state)/////strat_work/////
  {
    case 1: // move to specific silo
    { 
      int limit_value = check_printer_out_side();
      if(limit_value == 1)
      {
        running_state = 1;
      }
      else
      {
        Relay_ON();
        off_release_servo();
        close_tray();
        operate_printer();
        running_state = 2;
      }
      break;
    }
    case 2:
    {
      {
        sliding_motor_forward();  
        run_sliding_motor();
        running_state = 3;
      }
      break;
    }
    case 3:
    {   
      run_sticker_roller = true;
      running_state = 4;
    }
    case 4:
    { 
      running_state = 5;
      break;
    }
    case 5:   
    {
     {
      int sensor_value = get_proximeter_values();
      if(sensor_value == 2)
      {
        stop_sliding_motor();
        running_state = 6;
        test_time = millis();
      }
      break;
    }

    case 6:
    { 
      if((millis()-test_time)>=test_period)
        {
          release_printer();
          running_state = 7;
        }
      break;
    }
    case 7:
    {  
      reset_printer_operation(); 
      sliding_motor_backward();
      run_sliding_motor();
      open_tray();
      on_release_servo();
      running_state = 8;
      break;
    }

    case 8:
    {
      int sensor_value = get_proximeter_values();
      if(sensor_value == 1) 
      {
        stop_sliding_motor();
        off_release_servo();
        run_sticker_roller = false;
        running_state = 26;
      }     

      break;
    }

    case 26:        // process end here
    {
      //Serial.print("ok");
      break;
    }

      default:
      {

      }
   }
  }
}
//********************************************//
void initial_variables(void)
{
  reset_serial_variables();
  reset_sensor_variables();
  reset_motor_variables();
  reset_io_control();
}
//********************************************//
void interprete_command(String serial_command)
{
  switch(serial_command[0])
  {
    case 'l': // on off led
    {
      on_lamp(serial_command[1]);
      break;
    }
    case 'r': // reset mcu
    {
      Serial.println("reset command");
      delay(100);
      asm volatile ("jmp 0");
      break;
    }
    //////////////////////////////////////////////////
    case 'c': // check sensors value
    {
      Serial.print(get_proximeter_values(),HEX);
      break;
    }
    case 'p':
    {
      Serial.println(board_name);
      break;
    }
    case 'm': // 
    {
      switch(serial_command[1])
      {
        case '0':
        {
          Serial.println("Relese printer");
          release_printer();
          break;
        }
        case '1':
        {
          Serial.println("Operate printer");
          operate_printer();
          break;
        }
        case '2':
        {
          Serial.println("Reset printer");
          reset_printer_operation();
          break;
        }
        case '3':
        {
          Serial.println("Open tray");
          open_tray();
          break;
        }
        case '4':
        {
          Serial.println("Close tray");
          close_tray();
          break;
        }
        case '5':
        {
          Serial.println("Turnoff actuator");
          turnoff_actuator();
          break;
        }
        case '6':
        {
          Serial.println("On relay");
          Relay_ON();
          break;
        }
        case '7':
        {
          Serial.println("off relay");
          Relay_OFF();
          break;
        }
        case '8':
        {
          Serial.println("Run sticker motor");
          run_sticker_roller = true;
          break;
        }
        case '9':
        {
          Serial.println("Stop sticker roller");
          run_sticker_roller = false;
          break;
        }
        default:
        {

        }
      }
      break;
    }
    case 's': // start machine
    {
      silo_number = int(serial_command[1]-'0');
      if(silo_number>0 && silo_number<=6)             // maximum number of silo is set to 4
      {
        sliding_motor_forward();
        running_state = 1;
        sliding_motor_timer = micros();
        sliding_motor_period = 800;
      }
      break;
    }
    case 'g': // get state
    {
      Serial.println(running_state);
      break;
    }


    case 'h':
    {
      Serial.println("run motor edge");
      edge_machine = false;
      test_state = 1;
      break;
    }
    
    case 't':
    {
      int servo_parameter = int(serial_command[1]-'0');
      test_servo(servo_parameter);
      break;
    }
    case 'u':
    {
      check_conveyor();
      break;
    }
    case 'v':
    {
      reinit_conveyor();
      break;
    }
    case 'd':
    {
      int parameter = int(serial_command[1]-'0');
      debug_motor(parameter);
      break;
    }
    case 'f':
    {
      on_motor_flip_forward();
      break;
    }
    case 'b':
    {
      on_motor_flip_backward();
      break;
    }
    case 'e':
    {
      off_motor_flip();
      break;
    }
    case 'w':
    {
      linere_backward();
      break;
    }
    case 'i':
    {
      linere_forward();
      break;
    }
    case 'q':
    {
      off_linere();
      break;
    }
    // case 'x':
    // {
    //   int roller_state = get_conveyor_status();
    //   Serial.println(roller_state);
    //   break;
    // }
    case 'y':
    {
      on_conveyor();
      break;
    }
    case 'z':
    {
      reset_conveyor();
      break;
    }
    default:
    {

    }
  }
}


void debug_motor(int parameter)
{
    switch(parameter)
    {
        case 1:
        {
          Serial.println("Run silo 1 motor");
          run_silo_roller(1);
          break;
        }
        case 2:
        {
          Serial.println("Run silo 2 motor");
          run_silo_roller(2);
          break;
        }
        case 3:
        {
          Serial.println("Run silo 3 motor");
          run_silo_roller(3);
          break;
        }
        case 4:
        {
          Serial.println("Run silo 4 motor");
          run_silo_roller(4);
          break;
        }
        case 5:
        {
          Serial.println("Stop silo motor");
          stop_silo_roller();
          break;
        }
        case 6:
        {
          Serial.println("Move sliding motor forward");
          sliding_motor_forward();
          run_sliding_motor();
          break;
        }
        case 7:
        {
          Serial.println("Move sliding motor backward");
          sliding_motor_backward();
          run_sliding_motor();
          break;
        }
        case 8:
        {
          Serial.println("Stop sliding motor");
          stop_sliding_motor();
          break;
        }
        default:
        {
            // stop all motors
        }
    }
}




bool check_serial_command(void)
{
    bool interprete_status = false;
    if(Serial.available()){
        char temp_command = Serial.read();
        if(temp_command == '\n')
        {
            interprete_status = true;
        }
        else
        {
            serial_command = serial_command + String(temp_command);
        }  
    }
    return interprete_status;
}


String get_serial_command(void)
{
    String temp_command = serial_command;
    serial_command = "";
    return temp_command;
}

//*****************************
void reset_serial_variables(void){
    String serial_command = "";
    String serial_response = "";
}
