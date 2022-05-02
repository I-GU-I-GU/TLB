// === if infrared sensor detect object, the output logic 1
//#define debug_mode
int main_state = 0;
bool run_state = false;

const int SS_Relay = 5;
const int DIR_pin = 6;

const int sensor1 = 10;
const int sensor2 = 2;
const int sensor3 = 11;

const int led_status = 9;

int sensor1_value = 1;
byte sensor2_value = 0x00;
int sensor3_value = 1;

int interprete = 0;
String message = "";

bool pulse_logic = 0;
int pulse_period = 1000; // 300 milliseconds
unsigned long pulse_timer = 0;
const int pulse_pin = 7;

unsigned long timer_15s = 0;
const int timer_alarm = 15000;
unsigned long pre_run_timer = 0;

void setup() {
  Serial.begin(9600);
  delay(1000);
  while(Serial.read() >= 0);    // flush input command
  pinMode(sensor1,INPUT_PULLUP);
  pinMode(sensor2,INPUT_PULLUP);
  pinMode(sensor3,INPUT_PULLUP);
  pinMode(pulse_pin,OUTPUT);
  pinMode(led_status,OUTPUT);
  pinMode(DIR_pin,OUTPUT);
  
  pinMode(SS_Relay,OUTPUT);
  digitalWrite(SS_Relay,HIGH);
  digitalWrite(led_status,HIGH);
  digitalWrite(DIR_pin,HIGH);
  #ifdef debug_mode
    Serial.println("Start");
  #endif
}

void loop() {
  // ========= read all sensor ========
  sensor1_value = digitalRead(sensor1);
  sensor2_value = (sensor2_value << 1) + digitalRead(sensor2);
  sensor3_value = digitalRead(sensor3);

  // ========= get cmd =============
  if(Serial.available())
  {
    char inByte = Serial.read();
    if(inByte == '\n')
    {
      interprete = 1;
    }
    else
    {
      message = message + String(inByte);
    }
  }
  // ======== send response =========
  if(interprete==1)
  {
    interprete = 0;
    #ifdef debug_mode
      Serial.print("CMD: ");
      Serial.println(message);
    #endif
    char cmd = message[0];
    message = "";
    switch(cmd)
    {
      case '0':
      {
        main_state = 0;
        run_state = false;
        digitalWrite(SS_Relay,HIGH);
        message = "";
        Serial.println("0");
        break;
      }
      case '1':
      {
        run_state = true;
        //Serial.println(main_state);
        break;
      }
      case '2':
      {
        main_state = 0;
        run_state = false;
        digitalWrite(SS_Relay,HIGH);
        if(sensor1_value == 1)
        {
          Serial.println("6");  // box empty
        }
        else
        {
          if(sensor3_value == 0)
          {
            Serial.println("3"); // box full
          }
          else
          {
            Serial.println("7");
          }
        }
        break;
      }
      default:
      {
        
      }
    }
  }
  // === state transition ==========
  if(run_state)
  {
    switch(main_state)
  {
    case 0:
    {
      if(sensor1_value == 1)
        {
          main_state = 1;
        }
        else
        {
          main_state = 6;
        }
      break;
    }
    case 1:
    {
      if(sensor3_value == 0)
      {
        digitalWrite(SS_Relay,LOW);
        digitalWrite(led_status,LOW);
        main_state = 2;
        // start timer
        timer_15s = millis();
        pre_run_timer = millis();
      }
      else
      {
        main_state = 3;
      }
      break;
    }
    case 2:
    {
      // detect box on conveyor
      if(millis()-pre_run_timer>=100)
      {
        if(sensor2_value==0x03)
        {
          digitalWrite(SS_Relay,HIGH);
          digitalWrite(led_status,HIGH);
          main_state = 5;
        }
      }
      // check 15 seconds
      if(millis()-timer_15s >= timer_alarm)
      {
       main_state = 4; 
       digitalWrite(led_status,HIGH);
      }
      // run motor
      if(micros()- pulse_timer >= pulse_period)
      {
        pulse_timer = micros();
        pulse_logic =! pulse_logic;
        digitalWrite(pulse_pin,pulse_logic);
      }
      if(sensor3_value == 1)
      {
        main_state = 3;
      }
      break;
    }
    case 3:
    {
      run_state = false;
      Serial.println("3");
      digitalWrite(SS_Relay,HIGH);
      break;
    }
    case 4:
    {
      run_state = false;
      Serial.println("4");
      digitalWrite(SS_Relay,HIGH);
      break;
    }
    case 5:
    {
      run_state = false;
      Serial.println("5");
      digitalWrite(SS_Relay,HIGH);
      break;
    }
    case 6:
    {
      run_state = false;
      Serial.println("6");
      digitalWrite(SS_Relay,HIGH);
      break;
    }
    default:
    {
      main_state = 0;
    }
  }
 }
}
