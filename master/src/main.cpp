//======= add description =====


#include <main.hpp>

const String board_name = "TB001";
unsigned long tray_timer = 0;
unsigned int open_tray_period = 2000;  // time in milliseconds
unsigned int close_tray_period = 1500;  // time in milliseconds

unsigned long watch_dog_timer = 0; //time in milliseconds
unsigned long watch_dog_period = 18000; //time  in milliseconds
unsigned long period = 4000;  // time in milliseconds
unsigned long position_1_4 = 2600; // time in milliseconds
unsigned long last_time = 0;
unsigned long delay_AC = 2000; //time in miliseconds
unsigned long back_sliding = 4500; //time in miliseconds
unsigned long time_check = 2800; //time in miliseconds
unsigned long silo_run_dir = 200; //time in miliseconds

// unsigned long


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


void setup()
{
  Serial.begin(9600);
  while (!Serial);
  initial_variables();
  init_motors();
  initial_logic();
  on_converyer();
  init_sensors();
  initial_io_control();
  reset_io_control();

  delay(1000);
  open_tray();
  delay(1000);
  on_release_servo();
  on_chuck_servo();
  delay(1000);
  off_release_servo();
  off_chuck_servo();
  close_tray();
  delay(1000);
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
    //Relay_OFF();
    run_machine();
  }
  // if(printer_runing == true)
  // {
  //   if(millis() - watch_dog_timer > watch_dog_period )
  //    {
  //       sliding_motor_backward();
  //       run_sliding_motor();
  //         if(read_over_limit() || (get_proximeter_values()==1))
  //          {
  //            stop_sliding_motor();
  //            asm volatile ("jmp 0");
  //            release_printer();
  //            reset_printer_operation();
  //         }
        
  //    }
  // }
}
//*********************************************//

// void check_sliding(void)
// {
//      switch(running_state)
//   { 
//     {
//       case 1:
       
//        running_state = 2;
//        break;
//     }
  
  
//      default:
//      {

//      }

//   }

// }


void run_machine(void)
{
  switch(running_state)/////strat_work/////
  {
    // case 0:
    // {
    //   operate_printer();  //  สั่งปริ้น
    //   running_state = 1;
    //   break;
    // }
    case 1: // move to specific silo
    { 

      Relay_ON();
      operate_printer();
      //printer_runing = true;
      watch_dog_timer = millis();
      sliding_motor_forward();  //  เปลี่ยนทิศทางsliding
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
        running_state = 3;
      }
      break;
    }
    case 3:
    {
      run_silo_roller(silo_number);
      running_state = 4;
      break;
    }
    case 4:
  {   //infrared detect tube
    int infrared_value = check_tube_drop();
    if(infrared_value == 1)
       {
         stop_silo_roller();
         running_state = 5;
       }
    if(infrared_value == 0)
    {
      running_state = 60;
    }
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////

      // if(check_tube_drop())
      // {
      //   stop_silo_roller();
      //   running_state = 5;
      // }

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
      break;
  }

    case 5:
        {
          last_time = millis();
          running_state = 6;
          break;
        }
    case 6:
        {
          if((millis()-last_time)>=delay_AC)
          {
            running_state = 7;
          }
          break;
        }
    case 7:
    {
        open_tray();
        tray_timer = millis();
        running_state = 8;
        break;
    }
    case 8:
    {
      if((millis()-tray_timer)>=open_tray_period)
      {
        running_state = 9;
      }
      break;
    }
    case 9:
    {
      close_tray();
      running_state = 10;
      tray_timer = millis();
      break;
    }
    case 10:
    {
      if((millis()-tray_timer)>=close_tray_period)
      {
        running_state = 11;
      }
      break;
    }
    case 11:
    { 
      int Read_tube_value = Read_tube();   /////เช็คหลอดใน silo //////
         if(Read_tube_value == 0)
           {
             running_state = 12;
           } 
        else
           {
             running_state = 12; 
           }

      break;
    }
    case 12:
    {
      // move to silo number 4
      run_sliding_motor();
      last_time = millis();
      running_state = 13;
      break;
    }
    case 13:
    { 
       if(millis() - last_time > position_1_4 )
      {
        running_state = 30; ////30
      }
      int sensor_value = get_proximeter_values();
      if(sensor_value == 4)
      {
        stop_sliding_motor();
        //run_sliding_motor2();
        running_state = 14;
      }
      break;
    }
    case 14:
    {
          //sliding_motor_forward(); 
          run_sliding_motor_step2();
          //sliding_motor_forward(); 
          //run_sliding_motor();
          last_time = millis();
          running_state = 15;
      break;
    }
    case 15:
    {
      if(millis() - last_time > period )
      {
        running_state = 30;////30
      }
      if(read_limit_switch()==true)
           {
            running_state = 16;
            stop_sliding_motor_step2();
            sliding_motor_backward(); // ready to return home

        //****** set roller motor parameter *******//
            roller_motor_timer = micros();
            roller_motor_period = 200;
            roller_pulse_counter = 0;
            roller_pulse_target = 4000;
         }
      break;
    }
    case 16:
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
            running_state = 17;
        }
      }
      break;
    }
    case 17:
    {
      // set roller second speed
      roller_motor_timer = micros();
      roller_motor_period = 80;
      roller_pulse_counter = 0;
      roller_pulse_target = 12500;
      running_state = 18;
      break;
    }
    case 18:
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
            running_state = 19;
        }
      }
      break;
    }
    case 19:
    {
      //release_printer();
      run_sliding_motor();
      last_time = millis();
      running_state = 20;
      break;
    }
    case 20:
    {
       if(millis() - last_time > back_sliding )
       {
         running_state = 50;///////////////////////////////////////////////////////////////50
       }
      // set parameter go to origin
        int sensor_value = get_proximeter_values();
        if(sensor_value == 1)
        {
          stop_sliding_motor();
          running_state = 21;
        }
      break;
    }
    case 21:
    {
      release_printer();
      //reset_printer_operation();
      open_tray();
      tray_timer = millis();
      on_release_servo();
      running_state = 22;
      break;
    }
    case 22:
    {
      if((millis()-tray_timer)>=open_tray_period)
      {
        running_state = 23;
      }
      break;
    }
    case 23:
    {
      reset_printer_operation();
      on_chuck_servo();
      chuck_timer = millis();
      running_state = 24;
      break;
    }
    case 24:
    {
      if((millis()-chuck_timer)>= chuck_period)
      {
        running_state = 25;
      }
      break;
    }
    case 25:
    {
      off_chuck_servo();
      off_release_servo();
      close_tray();
      running_state = 26;
      break;
    }




    case 26:
    {
      Relay_OFF();
      //printer_runing = false;
      break;
    }




    case 30: //วนกลับไปทิ้งหลอดและเริ่มใหม่//
    { 
      stop_sliding_motor_step2();
      sliding_motor_backward(); // ready to return home
      release_printer();
      run_sliding_motor(); // return to home

      running_state = 31;
      break;
    }
    case 31:
    {
      // set parameter go to origin
      int sensor_value = get_proximeter_values();
      if(sensor_value == 1)
      {
        stop_sliding_motor();
        running_state = 32;
      }
      break;
    }
     case 32:
    {
      reset_printer_operation();
      open_tray();
      tray_timer = millis();
      on_release_servo();
      running_state = 33;
      break;
    }
    case 33:
    {
      if((millis()-tray_timer)>=open_tray_period)
      {
        running_state = 34;
      }
      break;
    }
    case 34:
    {
      on_chuck_servo();
      chuck_timer = millis();
      running_state = 35;
      break;
    }
    case 35:
    {
      if((millis()-chuck_timer)>= chuck_period)
      {
        running_state = 36;
      }
      break;
    }
    case 36:
    {
      off_chuck_servo();
      off_release_servo();
      close_tray();
      running_state = 26;  //เช็คหลอดใหม่//
      break;
    }


            ///////////////////////////////////////////////////////
case 40:    ////////////////เช็คหลอดค้างในsilo//////////////////////
    {       //////////////////////////////////////////////////////
      int Read_tube_value = Read_tube();   /////เช็คหลอดใน silo //////
        if(Read_tube_value == 0)
        {
          running_state = 26;
        }  
        else
        {
          running_state = 26; 
        } 
    break;
       }
case 41:
    {
      open_tray();
      tray_timer = millis();
      on_release_servo();
      running_state = 42;
      break;
    }
case 42:
    {
      if((millis()-tray_timer)>=open_tray_period)
      {
        running_state = 43;
      }
      break;
    }
case 43:
    {
      on_chuck_servo();
      chuck_timer = millis();
      running_state = 44;
      break;
    }
case 44:
    {
      if((millis()-chuck_timer)>= chuck_period)
      {
        running_state = 45;
      }
      break;
    }
case 45:
    {
      off_chuck_servo();
      off_release_servo();
      close_tray();
      running_state = 40;
      break;
    }


    case 50:////////////หลอดขัด/////////////
    {
      stop_sliding_motor();
      running_state = 51;
      break;
    }    
    case 51:
    {
      reset_printer_operation();
      open_tray();
      tray_timer = millis();
      on_release_servo();
      running_state = 52;
      break;
    }
    case 52:
    {
      if((millis()-tray_timer)>=open_tray_period)
      {
        running_state = 53;
      }
      break;
    }
    case 53:
    {
      on_chuck_servo();
      chuck_timer = millis();
      running_state = 54;
      break;
    }
    case 54:
    {
      if((millis()-chuck_timer)>= chuck_period)
      {
        running_state = 55;
      }
      break;
    }
    case 55:
    {
      off_chuck_servo();
      off_release_servo();
      close_tray();
      running_state = 56;  
      break;
    }
    case 56:
    {
      run_sliding_motor();
      running_state = 57;
      break;
    }
    case 57:
    {
      // set parameter go to origin
        int sensor_value = get_proximeter_values();
        if(sensor_value == 1)
        {
          stop_sliding_motor();
          running_state = 26;
        }
      break;
    }



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////



    case 60:
    {

     if(check_tube_drop())
        {
         stop_silo_roller();
         running_state = 5;
        }
      last_time = millis();
      running_state = 61;
      break;
    }

    case 61:
    { 
      if ((millis()-last_time)>=time_check)
      {
        run_dir();
        last_time = millis();
        running_state = 62;
      }
      
     if(check_tube_drop())
        {
         stop_silo_roller();
         running_state = 5;
        }
      break;
    }

    case 62:
    { 
      if((millis()-last_time)>=silo_run_dir)
      {
        stop_dir();
        running_state = 4;
      }
      
     if(check_tube_drop())
        {
         stop_silo_roller();
         running_state = 5;
        }
      break;
    }
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////        END        ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////

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
    case 'r': // reset mcu
    {
      Serial.println("reset command");
      delay(100);
      asm volatile ("jmp 0");
      break;
    }

    ////////////////////////////////////////////////////

    case 'a':
    {
      on_converyer();
      break;
    }

    //////////////////////////////////////////////////
  
    case 'c': // check sensorrs value
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
        default:
        {

        }
      }
      break;
    }
    case 's': // start machine
    {
      // Serial.println("Start machine");
      silo_number = int(serial_command[1]-'0');
      if(silo_number>0 && silo_number<=6)
      {
        sliding_motor_forward();
        off_converyer();
        // Serial.print("Silo number: ");
        // Serial.println(silo_number);
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
    default:
    {

    }
  }
}
