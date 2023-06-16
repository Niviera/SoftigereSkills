
/* Bibliotheken */
#include <Arduino.h>
#include "TempUndHum.h"
#include "Lufqualität.h"

/* Defines */
#define DHTPIN 2
#define MQPIN 0

/* Globale Variablen */
float temp;
float humi;
TempUndHum dht(DHTPIN);
Lufqualitaet LQuali(MQPIN, &temp, &humi);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  temp = 23.70;
  humi = 44.00;

  LQuali.readSensor();
  Serial.print("PPM: ");
  Serial.println(LQuali.get_ppm());
  Serial.print("Corrected PPM: ");
  Serial.println(LQuali.get_correctedppm());

  delay(5000);
}
