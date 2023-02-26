#include <Badge2020_TFT.h>

Badge2020_TFT tft;
#include "DHT.h"

#define DHTPIN 27   // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);
const int16_t SCREEN_WIDTH = 240;
const int16_t SCREEN_HEIGHT = 240;
const int16_t BACKGROUND_COLOR = ST77XX_BLACK;

void setup(void) {
  Serial.begin(115200);
  Serial.println("DHTxx test!");
  tft.init(SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.setRotation( 2 );
  dht.begin();

  // Anything from the Adafruit GFX library can go here, see
  // https://learn.adafruit.com/adafruit-gfx-graphics-library

  tft.fillScreen(BACKGROUND_COLOR);
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
  float hi = dht.computeHeatIndex(t, h, false);

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
  Serial.println(" *C");


  int currY = 30;
  centerHorizontalOverWriteExt("Humidity(%):", currY, 2, ST77XX_WHITE);
  currY += 60;
  centerHorizontalOverWriteExt(String(h), currY, 8, ST77XX_WHITE);
  tft.setTextSize(2);
  currY = 190;
  int currX = 10;
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);

  tft.setCursor(currX, currY);
  overWrite("Temp: ");
  tft.print(t);
  tft.print("C ");
  currY += 30;
  tft.setCursor(currX, currY);
  overWrite("Heat: ");
  tft.print(hi);
  tft.println("C");
}


// Center text helper methods

void centerHorizontal(const String &buf, uint16_t y) {
  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &size[0], &size[1]); //calc width of new string
  tft.setCursor((SCREEN_WIDTH / 2) - size[0] / 2, y);
  tft.print(buf);
}
void centerHorizontalExt(const String &buf, uint16_t y, uint8_t size, uint16_t color) {
  tft.setTextColor(color);
  tft.setTextSize(size);
  centerHorizontal(buf, y);
}
void centerHorizontalOverWrite(const String &buf, uint16_t y) {
  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &size[0], &size[1]); //calc width of new string
  tft.fillRect (0, y, SCREEN_WIDTH, size[1], BACKGROUND_COLOR);
  tft.setCursor((SCREEN_WIDTH / 2) - size[0] / 2, y);
  tft.print(buf);
}
void centerHorizontalOverWriteExt(const String &buf, uint16_t y, uint8_t size, uint16_t color) {
  tft.setTextColor(color);
  tft.setTextSize(size);
  centerHorizontalOverWrite(buf, y);
}
void overWriteExt(const String &buf, uint16_t x, uint16_t y) {
  tft.setCursor(x, y);
  overWrite(buf);
}
void overWrite(const String &buf) {
  int16_t x1, y1;
  static uint16_t size[2] = {0, 0};
  tft.getTextBounds(buf, 0, 0, &x1, &y1, &size[0], &size[1]); //calc width of new string
  tft.fillRect (0, tft.getCursorY(), SCREEN_WIDTH, size[1], BACKGROUND_COLOR);
  tft.print(buf);
}