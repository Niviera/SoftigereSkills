
/* Bibliotheken */
#include <Arduino.h>
#include "TempUndHum.h"

#include <WiFi.h>
#include <esp_now.h>


/* Defines */
#define DHTPIN 2
#define MQPIN 0

/* Globale Variablen */
float temp;
float humi;
TempUndHum Temp(DHTPIN);



uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0x54, 0x88, 0xBB};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {

float humidity; 
float temperatur;
bool rain;
float brightness;
float co2;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}


void setup()
{
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop()
{
  /* Temperatur und Luftfeuchtigkeit messen */
  if(Temp.readSensor()){
    myData.humidity = 50;
    myData.temperatur = 25;
  }else{
    myData.humidity = 100;
    myData.temperatur = 200;
  }
  
  /* Co2 gehalt messen */
  myData.co2 = 10;


  myData.brightness = 100; 
  
  
  myData.rain = 0;
  
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(5000);
}
