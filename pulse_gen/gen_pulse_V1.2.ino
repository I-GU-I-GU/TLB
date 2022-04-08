const int input_pin1 = 5;
const int input_pin2 = 9;
const int output_pin = 3;
bool output_logic = false;
unsigned long previous_time = 0;
unsigned long pulse_period1 = 110;
unsigned long pulse_period2 = 200;

void setup() {
  // put your setup code here, to run once:
  pinMode(input_pin1,INPUT);
  pinMode(input_pin2,INPUT);
  pinMode(output_pin,OUTPUT);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  
  //////////////////////step_1/////////////////////////
  if(digitalRead(input_pin1))
  {
    if ((micros()-previous_time)>= pulse_period1)
    {
      output_logic =! output_logic;
      digitalWrite(output_pin,output_logic);
      previous_time = micros();
    }
  }
  //////////////////////step_2/////////////////////////
  if(digitalRead(input_pin2))
  {
    if ((micros()-previous_time)>= pulse_period2)
    {
      output_logic =! output_logic;
      digitalWrite(output_pin,output_logic);
      previous_time = micros();
    }
  }
}

//void loop(){
//
//digitalWrite(output_pin,HIGH);
//delayMicroseconds(80);
//digitalWrite(output_pin,LOW);
//delayMicroseconds(80);
//  
//}
