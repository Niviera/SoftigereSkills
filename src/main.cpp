
/* Bibliotheken */
#include <Arduino.h>
#include "TempUndHum.h"

#include <WiFi.h>
#include <esp_now.h>

/* Defines */
#define DHTPIN 2
#define MQPIN 0
#define RECEIVER true

/* Globale Variablen */
float temp;
float humi;
TempUndHum Temp(DHTPIN);

uint8_t broadcastAddress[] = {0xC8, 0xC9, 0xA3, 0x54, 0x88, 0xBB};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message
{
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
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
/* callback when data is recieved */
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
}

/* */
void collect_Data()
{
  /* Lese DHT11 aus */
  if (Temp.readSensor())
  {
    myData.humidity = 50;
    myData.temperatur = 25;
  }
  else
  {

    myData.humidity = 100;
    myData.temperatur = 200;
  }

  /* Co2 gehalt messen */
  myData.co2 = 10;

  myData.brightness = 100;

  myData.rain = 0;
}
void send_Data()
{
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));

  if (result == ESP_OK)
  {
    Serial.println("Sent with success");
  }
  else
  {
    Serial.println("Error sending the data");
  }
}

void setup()
{
  /* Allgemeines Setup */
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  if (RECEIVER)
  {
    /* Setup Reciecver */
    esp_now_register_recv_cb(OnDataRecv);
  }
  else
  {
    /* Setup Sender */
    esp_now_register_send_cb(OnDataSent);
    // Register peer
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    // Add peer
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      Serial.println("Failed to add peer");
      return;
    }
  }

}

void loop()
{
  if (RECEIVER)
  {
    /* Alles was der Empfänger machen soll */
  }
  else
  {
    /* Alles was der Sender machen soll*/
    collect_Data();
    send_Data();
  }

  delay(5000);
}
