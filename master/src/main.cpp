//======= add description =====

#include <main.hpp>

const String board_name = "TB001";
unsigned long tray_timer = 0;
unsigned int open_tray_period = 800;  // time in milliseconds
unsigned int close_tray_period = 900;  // time in milliseconds


unsigned long period = 4000;  // time in milliseconds
unsigned long position_1_4 = 2600; // time in milliseconds
unsigned long last_time = 0;
unsigned long delay_AC = 500; //time in miliseconds
unsigned long back_sliding = 4500; //time in miliseconds
unsigned long time_check = 2800; //time in miliseconds
unsigned long silo_run_dir = 200; //time in miliseconds

bool move_more = false;
unsigned long move_more_timer = 0;
const int move_more_time = 20;

int silo_number = 0;
int running_state = 0;
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
unsigned int chuck_period = 1000;


bool printer_runing = false;
bool is_origin = false;
bool origin_pulse_logic = false;
unsigned long check_origin_timer = millis();


///
bool run_sticker_roller = false;
unsigned long sticker_roller_timer = 0;
const int sticker_roller_period = 200;
bool sticker_roller_logic = false;

void setup()
{
  Serial.begin(9600);
  Serial.flush();
  while (!Serial);
  initial_variables();
  init_motors();
  init_sensors();
  initial_io_control();
  reset_io_control();
}
//===========================================//

void loop()
{
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
      delay(1000);
      on_release_servo();
      on_chuck_servo();
      delay(1000);
      off_release_servo();
      off_chuck_servo();
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
    if(micros()-sticker_roller_period>=sticker_roller_period)
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
      Relay_ON();
      operate_printer();
      sliding_motor_forward();  //   reverse direction of sliding motor
      run_sliding_motor();
      running_state = 2;
      break;
    }
    case 2:
    {
      int sensor_value = get_proximeter_values();
      if(sensor_value == silo_number)
      {
        stop_sliding_motor();
        run_silo_roller(silo_number);
        running_state = 3;
      }
      break;
    }
    case 3:
    {   // infrared sensor detect tube
      int infrared_value = check_tube_drop();
      if(infrared_value == 1)
        {
          stop_silo_roller();
          last_time = millis();
          running_state = 5;
        }
      if(infrared_value == 0)
      {
        running_state = 61;
      }
      break;
    }
    case 5:   // wait tube drop on actuator
    {
      if((millis()-last_time)>=delay_AC)
      {
        open_tray();
        tray_timer = millis();
        running_state = 6;
      }
      break;
    }
    case 6:
    {
      if((millis()-tray_timer)>=open_tray_period)
      {
        close_tray();
        tray_timer = millis();
        running_state = 7;
      }
      break;
    }
    case 7:
    {
      if((millis()-tray_timer)>=close_tray_period)
      {
        run_sliding_motor();
        last_time = millis();
        running_state = 8;
      }
      break;
    }
    case 8:
    { 
      int sensor_value = get_proximeter_values();
      if(sensor_value == 4)
      {
        stop_sliding_motor();
        run_sliding_motor_step2();
        last_time = millis();
        running_state = 9;
      }
      break;
    }
    case 9:
    {
      if(read_limit_switch()==true)
        {
          running_state = 10;
          stop_sliding_motor_step2();
          sliding_motor_backward(); // ready to return home
        //****** set roller motor parameter *******//
          roller_motor_timer = micros();
          roller_motor_period = 200;
          roller_pulse_counter = 0;
          roller_pulse_target = 2000;       // 0.8 seconds
         }
      break;
    }
    case 10:
    {
      // run roller first step
      if((micros()-roller_motor_timer)>=roller_motor_period)
      {
        roller_motor_logic =! roller_motor_logic;
        write_roller_pulse(roller_motor_logic);
        roller_motor_timer = micros();
        // check pulse counter
        roller_pulse_counter = roller_pulse_counter +1;
        if(roller_pulse_counter>=roller_pulse_target)
        {
          // set roller second speed
          roller_motor_timer = micros();
          roller_motor_period = 80;
          roller_pulse_counter = 0;
          roller_pulse_target = 6250;     // 1 second
          running_state = 11;
        }
      }
      break;
    }
    case 11:
    {
      // run roller second step
      if((micros()-roller_motor_timer)>=roller_motor_period)
      {
        roller_motor_logic =! roller_motor_logic;
        write_roller_pulse(roller_motor_logic);
        roller_motor_timer = micros();
        // check pulse counter
        roller_pulse_counter = roller_pulse_counter +1;
        if(roller_pulse_counter>=roller_pulse_target)
        {
          running_state = 12;
          open_tray();
          run_sliding_motor();
          last_time = millis();
        }
      }
      break;
    }
    case 12:
    {
      int sensor_value = get_proximeter_values();
      if(sensor_value == 3)
        {
          on_release_servo();
          on_chuck_servo();
          release_printer();
          running_state = 13;
        }
      break;
    }
    case 13:
    {
      int sensor_value = get_proximeter_values();
      if(sensor_value == 1)
      {
        stop_sliding_motor();
        running_state = 14;
      }
      break;
    }
    case 14:
    {
      if((millis()-tray_timer)>=open_tray_period)
      {
        reset_printer_operation();
        running_state = 15;
      }
      break;
    }
    case 15:
    {
      if((millis()-chuck_timer)>= chuck_period)
      {
        running_state = 16;
      }
      break;
    }
    case 16:
    {
      off_release_servo();
      off_chuck_servo();
      close_tray();
      move_more = true;
      move_more_timer = millis();
      running_state = 26;
      break;
    }
    case 26:        // process end here
    {
      break;
    }

    // wait tube ============
    case 61:
    { 
     if(check_tube_drop())
        {
         stop_silo_roller();
         running_state = 3;
        }
      break;
    }
    default:
    {

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
    case 'x':
    {
      int roller_state = get_conveyor_status();
      Serial.println(roller_state);
      break;
    }
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