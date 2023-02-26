// ArduinoOTA for esp32 - Version: Latest 
#include <ArduinoOTA.h>

#include <Badge2020_TFT.h>

Badge2020_TFT tft;
#include "DHT.h"

#define DHTPIN 27   // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

void setup(void) {
  Serial.begin(115200);
  Serial.println("DHTxx test!");
  tft.init(240, 240);
  tft.setRotation( 2 );
  dht.begin();

  // Anything from the Adafruit GFX library can go here, see
  // https://learn.adafruit.com/adafruit-gfx-graphics-library

  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
  delay(2000);
  Serial.println("Before DHT READ!");

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(3);
  int currY = 10;
  tft.setCursor(0, currY);
  tft.print("Hum: ");
  tft.print(h);
  tft.print("%");
  currY += 60;
  tft.setTextColor(ST77XX_GREEN);
  tft.setCursor(0, currY);
  tft.print("Temp: ");
  tft.print(t);
  tft.print("C ");
  tft.print(f);
  tft.print("F");
  currY += 60;
  tft.setCursor(10, currY);
  tft.print("Heat: ");
  tft.print(hi);
  tft.println("F");
}

