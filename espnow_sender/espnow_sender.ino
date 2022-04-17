//#define debug_mode

#include <ESP8266WiFi.h>
#include <espnow.h>

const int reset_pin = 5; // D1 pin
const int input_pin = 4; // D2 pin
const int status_pin0 = 14; // D5 pin
const int status_pin1 = 12; // D6
const int status_pin2 = 13; // D7

byte input_pin_status = 0xFF;
byte reset_pin_status = 0xFF;
int cmd_from_serial = 0;

uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned long previousMillis = 0;
const int interval = 2000;

int interprete = 0;
int reset_cmd = 0;
String pcMessage = "";

typedef struct struct_message {
    int roller_status;
    int box_status;
} struct_message;

struct_message incomingMessage;
struct_message outgoingMessage;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  #ifdef debug_mode
  if (sendStatus == 0){
    Serial.println("Send success");
  }
  else{
    Serial.println("Send fail");
  }
  #endif
}

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&incomingMessage, incomingData, sizeof(incomingMessage));
  int incomingRoller_status = incomingMessage.roller_status;
  int incomingBox_status = incomingMessage.box_status;
  Serial.print(incomingRoller_status);
  Serial.print(",");
  Serial.println(incomingBox_status);
  //===== convert incomingRoller_status to binary, then send to status pins
  digitalWrite(status_pin0,incomingRoller_status & 0x01);
  digitalWrite(status_pin1,(incomingRoller_status >> 1) & 0x01);
  digitalWrite(status_pin2,(incomingRoller_status >> 2) & 0x01);
}

void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
  while(Serial.read()>0) { }
  pinMode(input_pin,INPUT_PULLUP);
  pinMode(reset_pin,INPUT_PULLUP);
  pinMode(status_pin0,OUTPUT);
  pinMode(status_pin1,OUTPUT);
  pinMode(status_pin2,OUTPUT);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != 0) {
    #ifdef debug_mode
      Serial.println("Error initializing ESP-NOW");
    #endif
    return;
  }
  // Set ESP-NOW Role
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  esp_now_register_send_cb(OnDataSent);
  // Register peer
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0);
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

  // read input pin
  input_pin_status = (input_pin_status << 1) + digitalRead(input_pin);
  if(input_pin_status == 0xF0)
  {
    interprete = 1;
  }
  reset_pin_status = (reset_pin_status << 1) + digitalRead(reset_pin);
  if(reset_pin_status == 0xF0)
  {
    reset_cmd = 1;
  }

  if(Serial.available())
  {
    char inByte = Serial.read();
    if(inByte=='\n')
    {
      interprete = 1;
      cmd_from_serial = 1;
    }
    else
    {
      pcMessage = pcMessage + String(inByte);
    }
  }
  
  if(interprete==1)
  {
    interprete = 0;
    #ifdef debug_mode
    Serial.print("CMD: ");
    Serial.println(pcMessage);
    #endif
    if(cmd_from_serial)
    {
      cmd_from_serial = 0;
      outgoingMessage.roller_status = int(pcMessage[0]-'0');
    }
    else
    {
      outgoingMessage.roller_status = 1;
    }
    #ifdef debug_mode
    Serial.print("outgoing: ");
    Serial.println(outgoingMessage.roller_status);
    #endif
    outgoingMessage.box_status = 0;
    esp_now_send(broadcastAddress, (uint8_t *) &outgoingMessage, sizeof(outgoingMessage));
    pcMessage = "";
  } 

  if(reset_cmd==1)
  {
    reset_cmd = 0;
    #ifdef debug_mode
    Serial.print("reset pin activate");
    #endif
    outgoingMessage.roller_status = 0;
    outgoingMessage.box_status = 0;
    esp_now_send(broadcastAddress, (uint8_t *) &outgoingMessage, sizeof(outgoingMessage));
  }
}
