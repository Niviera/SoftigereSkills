/* Bibliotheken */
#include <Arduino.h>
#include "TempUndHum.h"
#include <WiFi.h>
#include <esp_now.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

/* Defines */
#define DHTPIN 2
#define MQPIN 0
#define RECEIVER true

/* Globale Variablen */
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
//Initialization of OLED
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/* Funktion for Reciever*/
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  memcpy(&myData, incomingData, sizeof(myData));
  /* Debug der Sensoren ohne OLED
  Serial.print("CO2: ");
  Serial.println(myData.co2);
  Serial.print("Humidity: ");
  Serial.println(myData.humidity);
  Serial.print("Temperature: ");
  Serial.println(myData.temperatur);
  Serial.print("Rain: ");
  Serial.println(myData.rain);
  Serial.print("Brightness: ");
  Serial.println(myData.brightness);
  Serial.println();
  */
}

/* Funktionen for Sender */
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
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}



/* Controller */
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

  /* Specific Setup*/
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
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
      Serial.println(F("SSD1306 allocation failed"));
      for(;;);
    }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Temp.: " + String(myData.temperatur,2)+"C°");
  display.setCursor(0,15);
  display.println("Hum.: " + String(myData.humidity,2)+"%");
  display.setCursor(0,30);
  if (myData.brightness>30){
     display.println("Brightn.: Tag");
  }else{
    display.println("Brightn.: Nacht");
  } 
  display.setCursor(0,45);
  if (myData.rain==0){
     display.println("Kein Regen");
  }else if (myData.rain>0&&myData.rain<70){
    display.println("Regen");
  }else{
    display.println("Starker Regen");
  } 
  display.setCursor(0,60);
  display.println("CO2: " + String(myData.co2,2)+"ppm");
  display.display(); 

  delay(5000);
  }
  else
  {
    /* Alles was der Sender machen soll*/
    collect_Data();
    send_Data();
  }
  
}
