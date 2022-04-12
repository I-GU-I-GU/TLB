//#define debug_mode

#include <ESP8266WiFi.h>
#include <espnow.h>

String unoMessage = "";
int interprete = 0;


// REPLACE WITH THE MAC Address of your receiver 
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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
  //Serial.println(incomingRoller_status);
  Serial.println(incomingRoller_status);
}

void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
  delay(500);
  while(Serial.read()>0)
  {  }
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  if (esp_now_init() != 0) {
    #ifdef debugmode
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
  if(Serial.available())
  {
    char inByte = Serial.read();
    if(inByte=='\n')
    {
      interprete = 1;
    }
    else
    {
      unoMessage = unoMessage + String(inByte);
    }
  }

  if(interprete==1)
  {
    #ifdef debug_mode
      Serial.print("CMD: ");
      Serial.println(unoMessage);
    #endif
    interprete = 0;
    outgoingMessage.roller_status = int(unoMessage[0]-'0');
    outgoingMessage.box_status = 0;
    esp_now_send(broadcastAddress, (uint8_t *) &outgoingMessage, sizeof(outgoingMessage));
    unoMessage = "";
  }
}
