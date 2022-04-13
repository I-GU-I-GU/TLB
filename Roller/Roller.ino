//#define debug_mode
int main_state = 0;

const int SS_Relay = 5;

const int sensor1 = 2;
const int sensor2 = 3;
const int sensor3 = 4;

const int led_status = 9;

int sensor1_value = 1;
int sensor2_value = 1;
int sensor3_value = 1;

int interprete = 0;
String message = "";

bool pulse_logic = 0;
int pulse_period = 300; // 300 milliseconds
int pulse_timer = 0;
const int pulse_pin = 8;

unsigned long timer_15s = 0;
const int timer_alarm = 15000;

void setup() {
  Serial.begin(9600);
  delay(1000);
  while(Serial.read() >= 0);    // flush input command
  pinMode(sensor1,INPUT_PULLUP);
  pinMode(sensor2,INPUT_PULLUP);
  pinMode(sensor3,INPUT_PULLUP);
  pinMode(pulse_pin,OUTPUT);
  pinMode(led_status,OUTPUT);
  
  pinMode(SS_Relay,OUTPUT);
  digitalWrite(SS_Relay,LOW);
  digitalWrite(led_status,HIGH);
  #ifdef debug_mode
    Serial.println("Start");
  #endif
}

void loop() {
  // ========= read all sensor ========
  sensor1_value = digitalRead(sensor1);
  sensor2_value = digitalRead(sensor2);
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
  // === state transition ==========
    switch(main_state)
  {
    case 0:
    {
      if(message[0]=='1')
      {
        if(sensor1_value == 0)
        {
          main_state = 1;
        }
        else
        {
          main_state = 6;
        }
      }
      break;
    }
    case 1:
    {
      if(sensor3_value == 1)
      {
        digitalWrite(SS_Relay,LOW);
        digitalWrite(led_status,LOW);
        main_state = 2;
        // start timer
        timer_15s = millis();
      }
      else
      {
        main_state = 3;
      }
      break;
    }
    case 2:
    {
      if(sensor2_value==0)
      {
        digitalWrite(SS_Relay,HIGH);
        digitalWrite(led_status,HIGH);
        main_state = 5;
      }
      // check 15 seconds
      if(millis()-timer_15s >= timer_alarm)
      {
       main_state = 4; 
       digitalWrite(led_status,HIGH);
      }
      break;
    }
    case 3:
    {
      break;
    }
    case 4:
    {
      break;
    }
    case 5:
    {
      break;
    }
    case 6:
    {
      break;
    }
    default:
    {
      main_state = 0;
    }
  }
    
  // ========== run main state ======
  switch(main_state)
  {
    case 0:
    {
      break;
    }
    case 1:
    {
      break;
    }
    case 2:
    {
      if(millis()- pulse_timer >= pulse_period)
      {
        pulse_timer = millis();
        pulse_logic =! pulse_logic;
        digitalWrite(pulse_pin,pulse_logic);
      }
      break;
    }
    case 3:
    {
      break;
    }
    case 4:
    {
      break;
    }
    case 5:
    {
      break;
    }
    case 6:
    {
      break;
    }
    default:
    {
      main_state = 0;
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
    if(message[0] == '1')
    {
      interprete = 0;
      message = "";
      Serial.print(main_state);
      Serial.print('\n');
    }
    if(message[0] == '0')
    {
      asm volatile (" jmp 0");
    }
    
  }
}
